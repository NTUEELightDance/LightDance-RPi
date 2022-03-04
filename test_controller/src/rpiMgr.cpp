#include "../inc/rpiMgr.h"

RPiMgr::RPiMgr(const string& dancerName) : _dancerName(dancerName) {
    ifstream infile("./asset/LED.json", ios::in);
    if (!infile)
        cerr << "Error: cannot open ./asset/LED.json" << endl;
    else {
        infile >> LEDJson;
        for (json::iterator it = LEDJson.begin(); it != LEDJson.end(); ++it) {
            for (json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
                json j;
                for (int i = 0; i < it2.value().size(); ++i) {
                    uint8_t B = ((unsigned long)it2.value()[i]) % 256;
                    uint8_t G = (((unsigned long)it2.value()[i]) >> 8) % 256;
                    uint8_t R = (((unsigned long)it2.value()[i]) >> 16) % 256;
                    j.push_back(R);
                    j.push_back(G);
                    j.push_back(B);
                }
                it2.value() = j;
            }
        }
    }
    infile.close();
}

bool RPiMgr::setDancer() {
    string path = "./data/dancers/" + _dancerName + ".json";
    ifstream infile(path.c_str());
    if (!infile) {
        string msg = "file " + path + " not found";
        logger->error(msg.c_str());
        return false;
    }
    json j;
    infile >> j;
    _ELparts.clear();
    _OFparts.clear();
    _LEDparts.clear();
    int* OFchannelOrder = new int[26];
    int i = 0;
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        if (it.key() == "ELPARTS")
            _ELparts = it.value();
        if (it.key() == "OFPARTS") {
            _OFparts = it.value();
            OFchannelOrder[i] = i;
        }
        if (it.key() == "LEDPARTS")
            _LEDparts = it.value();
        i++;
    }

    uint8_t strips = _LEDparts.size();
    vector<uint16_t> nLEDs(strips);
    for (json::iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++it) {
        // cout << it.value()["id"] << " " << it.value()["len"] << endl;
        nLEDs[(int)(it.value()["id"])] = uint16_t(it.value()["len"]);
    }

    LED_buf.resize(nLEDs.size());
    for (int i = 0; i < nLEDs.size(); ++i) {
        LED_buf[i].resize(nLEDs[i] * 3, (char)0);
    }
    led_strip = new LED_Strip;
    led_strip->initialize(nLEDs);

    int** pcaTypeAddr = new int*[4];

    pcaTypeAddr[0] = new int[2];
    pcaTypeAddr[0][0] = 9956;
    pcaTypeAddr[0][1] = 0x3f;
    pcaTypeAddr[1] = new int[2];
    pcaTypeAddr[1][0] = 9956;
    pcaTypeAddr[1][1] = 0x2b;
    pcaTypeAddr[2] = new int[2];
    pcaTypeAddr[2][0] = 9956;
    pcaTypeAddr[2][1] = 0x56;
    pcaTypeAddr[3] = new int[2];
    pcaTypeAddr[3][0] = 9956;
    pcaTypeAddr[3][1] = 0x32;

    of = new PCA(OFchannelOrder, pcaTypeAddr);

    cout << "successfully set dancer\n";
    return true;
}

void RPiMgr::pause() {
    _playing = false;
}

void RPiMgr::load(const string& path) {
    string msg = "loading " + path;
    cout << "rpiMgr.cpp 62: " << msg << endl;
    // logger->log(msg.c_str());
    ifstream infile(path.c_str());
    if (!infile) {
        msg = "failed cannot open file " + path;
        cout << msg << endl;
        logger->error(msg.c_str());
        return;
    }
    infile >> _ctrlJson;
    _loaded = true;
}

void RPiMgr::play(const bool& givenStartTime, const unsigned& start, const unsigned& delay) {
    long timeIntoFunc = getsystime();
    if (!_loaded) {
        logger->error("play failed, need to load first");
        return;
    }
    if (_ctrlJson.size() == 0) {
        logger->log("Warning: control.json is empty\nend of playing\nsuccess");
        return;
    }
    if (givenStartTime)
        _startTime = start;
    if (_startTime > (unsigned long)_ctrlJson[_ctrlJson.size() - 1]["start"]) {
        logger->log("Warning: startTime excess totalTime\nend of playing\nsuccess");
        return;
    }

    size_t currentFrameId = getFrameId();
    long hadDelay = getsystime() - timeIntoFunc;
    if (hadDelay < delay)
        this_thread::sleep_for(chrono::microseconds(delay - hadDelay));
    logger->success("play success");

    if (_ctrlJson[currentFrameId]["fade"])
        lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
    else
        lightOneStatus(_ctrlJson[currentFrameId]["status"]);

    long sysStartTime = getsystime();
    _playing = true;
    long startTime = (long)_startTime;

    thread loop(&RPiMgr::play_loop, this, ref(startTime), currentFrameId);
    loop.detach();
    return;
}

void RPiMgr::stop() {
    _playing = false;
    _startTime = 0;
    eltest(-1, 0);
    return;
}

void RPiMgr::statuslight() {
    ifstream infile("./data/status.json");
    if (!infile) {
        logger->error("cannot open ./data/status.json");
        return;
    }
    json status;
    infile >> status;
    lightOneStatus(status);
    return;
}

void RPiMgr::eltest(int id, unsigned brightness) {
    if (brightness > 4095) {
        // logger->log("Warning: brightness is bigger than 4095, light brightness as 4095");
        brightness = 4095;
    }
    if (id < 0) {
        for (int i = 0; i < 32; ++i) {
            // if (i < 16)
            //     el1.setEL(i, brightness);
            // else
            //     el2.setEL(i % 16, brightness);
        }
        return;
    }
    // if (id < 16)
    //     el1.setEL(id, brightness);
    // else
    //     el2.setEL(id % 16, brightness);

    return;
}

void RPiMgr::ledtest() {
    // TODO
    return;
}

void RPiMgr::list() {
    logger->log("ELPARTS:");
    for (json::const_iterator it = _ELparts.begin(); it != _ELparts.end(); ++it) {
        string part = "\t";
        part += it.key();
        for (int i = 0; i < 15 - it.key().size(); ++i)
            part += " ";
        part += it.value();
        logger->log(part.c_str());
    }
    logger->log("LEDPARTS:");
    for (json::const_iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++it) {
        string part = "\t";
        part += it.key();
        for (int i = 0; i < 15 - it.key().size(); ++i)
            part += " ";
        part += it.value();
        logger->log(part.c_str());
    }
    return;
}

void RPiMgr::quit() {
    return;
}

// private function
size_t RPiMgr::getFrameId() const {
    size_t totalFrame = _ctrlJson.size();
    if (totalFrame == 0) {
        logger->log("Warning: totalFrame is 0\n");
        return 0;
    }
    if (_startTime > (unsigned long)_ctrlJson[totalFrame - 1]["start"]) {
        logger->log("Warning: startTime exceed total time\n");
        return 0;
    }
    if (_startTime == 0)
        return 0;

    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first <= last) {
        size_t mid = (first + last) / 2;
        if (_startTime > (unsigned long)_ctrlJson[mid]["start"])
            first = mid + 1;
        else if (_startTime == (unsigned long)_ctrlJson[mid]["start"])
            return mid;
        else
            last = mid - 1;
    }
    if (_ctrlJson[first]["start"] > (unsigned long)_startTime)
        --first;
    return first;
}

json RPiMgr::getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& secondStatus) const {
    size_t firstTime = firstStatus["start"];
    size_t secondTime = secondStatus["start"];
    float rate = (float)(currentTime - firstTime) / (float)(secondTime - firstTime);
    json ret;
    for (json::const_iterator it = firstStatus["status"].begin(); it != firstStatus["status"].end(); ++it) {
        if (_ELparts.find(it.key()) != _ELparts.end()) {
            json::const_iterator it2 = secondStatus["status"].find(it.key());
            float temp = (1 - rate) * float(it.value()) + rate * float(it2.value());
            ret[it.key()] = roundf(temp * 10) / 10.0;
        } else if (_LEDparts.find(it.key()) != _LEDparts.end()) {
            json LEDinfo;
            LEDinfo["src"] = it.value()["src"];
            json::const_iterator it2 = secondStatus["status"].find(it.key());
            float temp = (1 - rate) * float(it.value()["alpha"]) + rate * float(it2.value()["alpha"]);
            LEDinfo["alpha"] = roundf(temp * 10) / 10.0;
            ret[it.key()] = LEDinfo;
        } else
            cerr << "Error: light name " << it.key() << " not found!" << endl;
    }
    return ret;
}

void RPiMgr::lightOneStatus(const json& status) {
    cout << _OFparts["OF1"] << endl;
    for (json::const_iterator it = status.begin(); it != status.end(); ++it) {
        cout << "finding: " << it.key() << endl;
        json::const_iterator ELid = _ELparts.find(it.key());
        json::const_iterator OFid = _OFparts.find(it.key());
        json::const_iterator LEDid = _LEDparts.find(it.key());
        if (ELid != _ELparts.end()) {
            // ELparts
            cout << "EL" << endl;
            uint8_t id = ELid.value();
            cout << "id: " << unsigned(id) << endl;
            // uint16_t dt = getELBright(it.value());
            // if (id < 16)
            //     el1.setEL(id, dt);
            // else
            //     el2.setEL(id % 16, dt);
        } else if (OFid != _OFparts.end()) {
            // OFparts
            cout << "OF" << endl;
            uint8_t id = OFid.value();
            cout << "id: " << unsigned(id) << endl;

            int OFs[156] = {
                255, 255, 255, 80, 70, 55,  // PWM_RED PWM_GREEN PWM_BLUE IREF_RED IREF_GREEN IREF_BLUE of OF0
                255, 255, 255, 80, 70, 55,  // PWM_RED PWM_GREEN PWM_BLUE IREF_RED IREF_GREEN IREF_BLUE of OF1
                255, 255, 255, 70, 70, 55,  // PWM_RED PWM_GREEN PWM_BLUE IREF_RED IREF_GREEN IREF_BLUE of OF2
                255, 255, 255, 70, 70, 55,  // PWM_RED PWM_GREEN PWM_BLUE IREF_RED IREF_GREEN IREF_BLUE of OF3
                255, 255, 255, 70, 55, 50,  //...
                255, 255, 255, 70, 55, 50,
                255, 255, 255, 50, 55, 70,
                255, 255, 255, 50, 55, 70,
                255, 255, 255, 55, 50, 70,
                255, 255, 255, 55, 50, 70,
                255, 255, 255, 70, 50, 55,
                255, 255, 255, 70, 50, 55,
                255, 255, 255, 70, 85, 50,
                255, 255, 255, 70, 85, 50,
                255, 255, 255, 85, 70, 50,
                255, 255, 255, 85, 70, 50,
                255, 255, 255, 70, 50, 85,
                255, 255, 255, 70, 50, 85,
                255, 255, 255, 85, 50, 70,
                255, 255, 255, 85, 50, 70,
                255, 255, 255, 50, 70, 85,
                255, 255, 255, 50, 70, 85,
                255, 255, 255, 50, 85, 70,
                255, 255, 255, 50, 85, 70,
                255, 255, 255, 85, 85, 85,
                255, 255, 255, 85, 85, 85,  // 26 OFs in total
            };

            of->Write(OFs);
            // uint16_t dt = getOFBright(it.value());
            // if (id < 16)
            //     el1.setEL(id, dt);
            // else
            //     el2.setEL(id % 16, dt);
        } else if (LEDid != _LEDparts.end()) {
            // LEDparts
            cout << "LED" << endl;
            uint8_t id = LEDid.value()["id"];
            size_t len = LEDid.value()["len"];
            cout << "id: " << unsigned(id) << "len: " << unsigned(len) << endl;
            // uint8_t* color = new uint8_t[len];
            // for (int i = 0; i < len; ++i) {
            //     color[i] = uint8_t(LEDJson[it.key()][string(it.value()["src"])][i]);
            // }
            // // led_strip->sendToStrip(id, color);
            // delete[] color;

            // uint32_t color;
            // std::stringstream ss;
            // ss << std::hex << ;
            // ss >> x;

            for (int i = 0; i < _LEDparts.size(); ++i) {
                for (int j = 0; j < len; ++j) {
                    if (i == id) {
                        LED_buf[i][3 * j] = 0;        // g
                        LED_buf[i][3 * j + 1] = 0;    // r
                        LED_buf[i][3 * j + 2] = 100;  // b
                    }
                }
            }
            led_strip->sendToStrip(LED_buf);

        } else {
            cerr << "Error: lightName " << it.key() << " not found!" << endl;
        }
    }
}

// For threading
void RPiMgr::play_loop(const long& startTime, size_t currentFrameId) {
    const long sysStartTime = getsystime();
    while (_playing) {
        if (_startTime >= (unsigned long)_ctrlJson[_ctrlJson.size() - 1]["start"]) {
            lightOneStatus(_ctrlJson[_ctrlJson.size() - 1]["status"]);
            _playing = false;
            break;
        }
        if (_startTime >= (unsigned long)_ctrlJson[currentFrameId + 1]["start"]) {
            ++currentFrameId;
            if (_ctrlJson[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
            else
                lightOneStatus(_ctrlJson[currentFrameId]["status"]);
        } else {
            if (_ctrlJson[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
        }

        _startTime = startTime + (getsystime() - sysStartTime);
    }
    cout << "end playing\n";
}