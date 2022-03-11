#include "rpiMgr.h"

#include "OFrgba_to_rgb.hpp"

RPiMgr::RPiMgr(const string& dancerName) : _dancerName(dancerName) {}

bool RPiMgr::setDancer() {
    string path = "../data/dancers/" + _dancerName + ".json";
    ifstream infile(path.c_str());
    if (!infile) {
        cout << "Cannot open file: " << path << endl;
        return false;
    }

    json j;
    infile >> j;

    // LED
    led_players.clear();
    for (json::iterator it = j["LEDPARTS"].begin(); it != j["LEDPARTS"].end(); ++it)
        led_players.push_back(LEDPlayer(it.key(), it.value()["len"], it.value()["id"]));

    vector<uint16_t> nLEDs(TOTAL_LED_PARTS);
    for (auto& lp : led_players)
        nLEDs[lp.channel_id] = lp.len;
    LED_buf.resize(nLEDs.size());
    for (int i = 0; i < LED_buf.size(); ++i)
        LED_buf[i].resize(nLEDs[i] * 3, (char)0);

    // OF
    of_player.init(j["OFPARTS"]);
    OF_buf.resize(NUM_OF);
    for (int i = 0; i < OF_buf.size(); ++i)
        OF_buf[i].resize(NUM_OF_PARAMS);

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
    for (auto& lp : led_players)
        lp.load(_LEDJson[lp.name]);

    // OF
    OFfile >> _OFJson;
    of_player.load(_OFJson);

    _loaded = true;
    logger->success("Load", msg);
}

void RPiMgr::play(const bool& givenStartTime, const unsigned& start, const unsigned& delay) {
    long timeIntoFunc = getsystime();
    if (!_loaded) {
        logger->error("Play", "Play failed, need to load first");
        return;
    }
    size_t total_led_frame = 0;
    size_t led_end_time = 0;
    for (auto& lp : led_players) {
        total_led_frame += lp.getFrameNum();
        led_end_time = max(led_end_time, lp.getEndTime());
    }

    if (total_led_frame == 0 && of_player.getFrameNum() == 0) {
        logger->log("Play", "Warning: LED.json and OF.json is empty\nend of play");
        return;
    }
    if (givenStartTime)
        _startTime = start;

    if (_startTime > max(led_end_time, of_player.getEndTime())) {
        logger->log("Play", "Warning: startTime excess both LED.json and OF.json totalTime\nend of playing");
        return;
    }

    // size_t currentOFId = getOFId();
    long hadDelay = getsystime() - timeIntoFunc;
    if (hadDelay < delay)
        this_thread::sleep_for(chrono::microseconds(delay - hadDelay));
    logger->success("Play", "Start play loop thread");

    for (auto& lp : led_players)
        lightLEDStatus(lp.getFrame(_startTime), lp.channel_id);

    lightOFStatus(of_player.getFrame(_startTime));

    long sysStartTime = getsystime();
    _playing = true;
    long startTime = (long)_startTime;

    cout << "start play loop\n";
    thread loop(&RPiMgr::play_loop, this, startTime);
    loop.detach();
    return;
}

void RPiMgr::stop() {
    _playing = false;
    _startTime = 0;
    // TODO: set LED and OF to 0
    return;
}

void RPiMgr::statuslight() {
    // TODO
}

void RPiMgr::LEDtest() {
    // TODO
    return;
}

void RPiMgr::OFtest() {
    // TODO
    return;
}

void RPiMgr::list() {
    string mes = "LEDPARTS:\n";
    for (auto& lp : led_players) {
        string part = "\t";
        part += lp.name;
        for (int i = 0; i < 15 - lp.name.size(); ++i)
            part += ' ';
        part += "channel: " + to_string(lp.channel_id) + ", len: " + to_string(lp.len) + '\n';
        mes += part;
    }
    mes += "OFPARTS:\n";
    mes += of_player.getParts();
    logger->success("List", mes);
    return;
}

void RPiMgr::quit() {
    return;
}

// private function
void RPiMgr::lightLEDStatus(const LEDPlayer::frame& frame, const int& channel_id) {
    for (int i = 0; i < frame.status.size(); ++i) {
        char R, G, B;
        const float alpha = float(frame.status[i].alpha) / ALPHA_RANGE;
        colorCode2RGB(frame.status[i].colorCode, R, G, B);
        LEDrgba_to_rgb(LED_buf[channel_id], i, R, G, B, alpha);
    }

    led_strip->sendToStrip(LED_buf);
}

void RPiMgr::lightOFStatus(const OFPlayer::frame& frame) {
    for (auto it = frame.status.begin(); it != frame.status.end(); ++it) {
        char R, G, B;
        const float alpha = float(it->second.alpha) / ALPHA_RANGE;
        colorCode2RGB(it->second.colorCode, R, G, B);
        OFrgba_to_rgbiref(OF_buf[of_player.getChannelId(it->first)], R, G, B, alpha);
    }
    of->WriteAll(OF_buf);
}

// For threading
void RPiMgr::play_loop(const long startTime) {
    const long sysStartTime = getsystime();
    const long localStartTime = startTime;
    while (_playing) {
        bool is_finished = of_player.is_finished();
        for (auto& lp : led_players)
            is_finished &= lp.is_finished();
        if (is_finished) {
            _playing = false;
            break;
        }

        // LED
        for (auto& lp : led_players)
            lightLEDStatus(lp.getFrame(_startTime), lp.channel_id);

        // OF
        lightOFStatus(of_player.getFrame(_startTime));

        // Calculate startTime
        _startTime = localStartTime + (getsystime() - sysStartTime);
    }
    cout << "end playing\n";
}