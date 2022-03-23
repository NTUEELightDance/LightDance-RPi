#include "rpiMgr.h"

#include "OFrgba_to_rgb.hpp"

RPiMgr::RPiMgr(const string& dancerName) : _dancerName(dancerName) {}

bool RPiMgr::setDancer() {
    string path = "./data/dancers/" + _dancerName + ".json";
    ifstream infile(path.c_str());
    if (!infile) {
        cout << "Cannot open file: " << path << endl;
        return false;
    }

    json j;
    infile >> j;

    // FPS
    fps = int(j["fps"]);

    // LED
    ledPlayers.clear();
    for (json::iterator it = j["LEDPARTS"].begin(); it != j["LEDPARTS"].end(); ++it)
        ledPlayers.push_back(LEDPlayer(it.key(), it.value()["len"], it.value()["id"]));

    vector<uint16_t> nLEDs(TOTAL_LED_PARTS);
    for (auto& lp : ledPlayers)
        nLEDs[lp.channelId] = lp.len;
    LEDBuf.resize(nLEDs.size());
    for (int i = 0; i < LEDBuf.size(); ++i)
        LEDBuf[i].resize(nLEDs[i] * 3, (char)0);

    // OF
    ofPlayer.init(j["OFPARTS"]);
    OFBuf.resize(NUM_OF);
    for (int i = 0; i < OFBuf.size(); ++i)
        OFBuf[i].resize(NUM_OF_PARAMS);

    // hardware
    led_strip = new LED_Strip;
    led_strip->initialize(nLEDs);
    of = new PCA;

    return true;
}

void RPiMgr::pause() {
    _playing = false;
}

void RPiMgr::load(const string& path) {
    // While playing, you cannot load
    if (_playing) {
        logger->error("Load", "Cannot load while playing");
        return;
    }
    // Files
    ifstream LEDfile(path + "LED.json");
    ifstream OFfile(path + "OF.json");
    string msg = "Loading dir: " + path;
    if (!LEDfile) {
        msg += "\nFailed to open file: " + path + "LED.json";
        logger->error("Load", msg);
        return;
    }
    if (!OFfile) {
        msg += "\nFailed to open file: " + path + "OF.json";
        logger->error("Load", msg);
        return;
    }

    // LED
    json _LEDJson, _OFJson;
    LEDfile >> _LEDJson;
    for (auto& lp : ledPlayers)
        lp.load(_LEDJson[lp.name]);

    // OF
    OFfile >> _OFJson;
    ofPlayer.load(_OFJson);

    _loaded = true;
    logger->success("Load", msg);
}

void RPiMgr::play(const bool& givenStartTime, const unsigned& start, const unsigned& delay) {
    long timeIntoFunc = getsystime();
    if (!_loaded) {
        logger->error("Play", "Play failed, need to load first");
        return;
    }
    size_t totalLedFrame = 0;
    size_t ledEndTime = 0;
    for (auto& lp : ledPlayers) {
        totalLedFrame += lp.getFrameNum();
        ledEndTime = max(ledEndTime, lp.getEndTime());
    }

    if (totalLedFrame == 0 && ofPlayer.getFrameNum() == 0) {
        logger->log("Play", "Warning: LED.json and OF.json is empty\nend of play");
        return;
    }
    if (givenStartTime)
        _startTime = start;

    if (_startTime > max(ledEndTime, ofPlayer.getEndTime())) {
        logger->log("Play", "Warning: startTime excess both LED.json and OF.json totalTime\nend of playing");
        return;
    }

    long hadDelay = getsystime() - timeIntoFunc;
    if (hadDelay < delay)
        this_thread::sleep_for(chrono::milliseconds(delay - hadDelay));
    logger->success("Play", "Start play loop thread");

    for (auto& lp : ledPlayers)
        lightLEDStatus(lp.getFrame(_startTime), lp.channelId);

    lightOFStatus(ofPlayer.getFrame(_startTime));

    long sysStartTime = getsystime();
    _playing = true;
    long startTime = (long)_startTime;

    cout << "start play loop\n";
    thread loop(&RPiMgr::playLoop, this, startTime);
    loop.detach();
    return;
}

void RPiMgr::stop() {
    _playing = false;
    _startTime = 0;
    darkAll();
    return;
}

void RPiMgr::statuslight() {
    // TODO
}

void RPiMgr::LEDtest(const int& channel, int colorCode, int alpha) {
    // Will change led buf;
    const float _alpha = float(alpha) / ALPHA_RANGE;
    char R, G, B;
    colorCode2RGB(colorCode, R, G, B);
    ledDark();
    for (int i = 0; i < LEDBuf[channel].size() / 3; ++i)
        LEDrgba_to_rgb(LEDBuf[channel], i, R, G, B, _alpha);
    led_strip->sendToStrip(LEDBuf);
    return;
}

void RPiMgr::OFtest(const int& channel, int colorCode, int alpha) {
    // Will not change of buf
    vector<char> buf(6);
    char R, G, B;
    colorCode2RGB(colorCode, R, G, B);
    // ofDark();
    OFrgba2rgbiref(buf, R, G, B, alpha);
    // OFrgba2rgbiref(OFBuf[channel], R, G, B, alpha);
    // of->WriteAll(OFBuf);
    of->WriteChannel(buf, channel);
    return;
}

void RPiMgr::list() {
    string mes = "LEDPARTS:\n";
    for (auto& lp : ledPlayers) {
        string part = "\t";
        part += lp.name;
        for (int i = 0; i < 15 - lp.name.size(); ++i)
            part += ' ';
        part += "channel: " + to_string(lp.channelId) + ", len: " + to_string(lp.len) + '\n';
        mes += part;
    }
    mes += "OFPARTS:\n";
    mes += ofPlayer.getParts();
    logger->success("List", mes);
    return;
}

void RPiMgr::quit() {
    return;
}

void RPiMgr::darkAll() {
    ledDark();
    ofDark();
}

void RPiMgr::lightAll(int colorCode, int alpha) {
    char R, G, B;
    const float _alpha = float(alpha) / ALPHA_RANGE;
    colorCode2RGB(colorCode, R, G, B);
    for (int i = 0; i < LEDBuf.size(); ++i)
        for (int j = 0; j < LEDBuf[i].size(); ++j)
            LEDrgba_to_rgb(LEDBuf[i], j, R, G, B, _alpha);
    for (int i = 0; i < OFBuf.size(); ++i)
        OFrgba2rgbiref(OFBuf[i], R, G, B, alpha);
    led_strip->sendToStrip(LEDBuf);
    of->WriteAll(OFBuf);
    return;
}

// private function
void RPiMgr::lightLEDStatus(const LEDPlayer::Frame& frame, const int& channelId) {
    for (int i = 0; i < frame.status.size(); ++i) {
        char R, G, B;
        const float alpha = float(frame.status[i].alpha) / ALPHA_RANGE;
        colorCode2RGB(frame.status[i].colorCode, R, G, B);
        LEDrgba_to_rgb(LEDBuf[channelId], i, R, G, B, alpha);
    }

    led_strip->sendToStrip(LEDBuf);
}

void RPiMgr::lightOFStatus(const OFPlayer::Frame& frame) {
    for (auto it = frame.status.begin(); it != frame.status.end(); ++it) {
        char R, G, B;
        const float alpha = float(it->second.alpha);
        colorCode2RGB(it->second.colorCode, R, G, B);
        OFrgba2rgbiref(OFBuf[ofPlayer.getChannelId(it->first)], R, G, B, alpha);
    }
    of->WriteAll(OFBuf);
}

void RPiMgr::ledDark() {
    for (int i = 0; i < LEDBuf.size(); ++i)
        for (int j = 0; j < LEDBuf[i].size(); ++j)
            LEDBuf[i][j] = 0;
    led_strip->sendToStrip(LEDBuf);
}

void RPiMgr::ofDark() {
    for (int i = 0; i < OFBuf.size(); ++i)
        for (int j = 0; j < OFBuf[i].size(); ++j)
            OFBuf[i][j] = 0;
    of->WriteAll(OFBuf);
}

// For threading
void RPiMgr::playLoop(const long startTime) {
    const long sysStartTime = getsystime();
    const long localStartTime = startTime;
    while (_playing) {
        bool isFinished = ofPlayer.isFinished();
        for (auto& lp : ledPlayers)
            isFinished &= lp.isFinished();
        if (isFinished) {
            _playing = false;
            _startTime = 0;
            break;
        }

        // LED
        for (auto& lp : ledPlayers)
            lightLEDStatus(lp.getFrame(_startTime), lp.channelId);

        // OF
        lightOFStatus(ofPlayer.getFrame(_startTime));

        // Calculate startTime
        _startTime = localStartTime + (getsystime() - sysStartTime);
        this_thread::sleep_for(chrono::milliseconds(1000 / fps));
    }
    cout << "end playing\n";
}