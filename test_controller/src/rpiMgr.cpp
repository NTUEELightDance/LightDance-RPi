#include "../inc/rpiMgr.h"

RPiMgr::RPiMgr(const string& dancerName) : _dancerName(dancerName) {}

bool RPiMgr::setDancer() {
    string path = "./data/dancers/" + _dancerName + ".json";
    ifstream infile(path.c_str());
    if (!infile)
        return false;

    json j;
    infile >> j;
    _LEDparts.clear();
    _OFparts.clear();
    _LEDparts = j["LEDPARTS"];
    _OFparts = j["OFPARTS"];

    vector<uint16_t> nLEDs(TOTAL_LED_PARTS);
    for (json::iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++it)
        nLEDs[it.value()["id"]] = it.value()["len"];

    LED_buf.resize(nLEDs.size());
    for (int i = 0; i < LED_buf.size(); ++i)
        LED_buf[i].resize(nLEDs[i] * 3, (char)0);
    OF_buf.resize(TOTAL_OF_PARTS);
    for (int i = 0; i < OF_buf.size(); ++i)
        OF_buf[i].resize(OF_PARAMS, (char)0);

    led_strip = new LED_Strip;
    led_strip->initialize(nLEDs);
    of = new PCA;

    cout << "Successfully set dancer\n";
    return true;
}

void RPiMgr::pause() {
    _playing = false;
}

void RPiMgr::load(const string& path) {
    if (_playing) {
        logger->error("Load", "Cannot load while playing");
        return;
    }
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
    LEDfile >> _LEDJson;
    OFfile >> _OFJson;
    _loaded = true;
    logger->success("Load", msg);
}

void RPiMgr::play(const bool& givenStartTime, const unsigned& start, const unsigned& delay) {
    long timeIntoFunc = getsystime();
    if (!_loaded) {
        logger->error("Play", "Play failed, need to load first");
        return;
    }
    if (_LEDJson.size() == 0 && _OFJson.size() == 0) {
        logger->log("Play", "Warning: LED.json and OF.json is empty\nend of play");
        return;
    }
    if (givenStartTime)
        _startTime = start;
    if (_startTime > (size_t)max(_LEDJson[_LEDJson.size() - 1]["start"], _OFJson[_OFJson.size() - 1]["start"])) {
        logger->log("Play", "Warning: startTime excess both LED.json and OF.json totalTime\nend of playing");
        return;
    }

    size_t currentLEDId = getLEDId();
    size_t currentOFId = getOFId();
    long hadDelay = getsystime() - timeIntoFunc;
    if (hadDelay < delay)
        this_thread::sleep_for(chrono::microseconds(delay - hadDelay));
    logger->success("Play", "Start play loop thread");

    if (_LEDJson[currentLEDId]["fade"])
        lightLEDStatus(getLEDFadeStatus(_startTime, _LEDJson[currentLEDId], _LEDJson[currentLEDId + 1]));
    else
        lightLEDStatus(_LEDJson[currentLEDId]["status"]);
    if (_OFJson[currentOFId]["fade"])
        lightOFStatus(getOFFadeStatus(_startTime, _OFJson[currentOFId], _OFJson[currentOFId + 1]));
    else
        lightOFStatus(_OFJson[currentOFId]["status"]);

    long sysStartTime = getsystime();
    _playing = true;
    long startTime = (long)_startTime;

    thread loop(&RPiMgr::play_loop, this, startTime, currentLEDId, currentOFId);
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
    for (json::const_iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++it) {
        string part = "\t";
        part += it.key();
        for (int i = 0; i < 15 - it.key().size(); ++i)
            part += " ";
        part += it.value();
        mes += part + "\n";
    }
    mes += "OFPARTS:\n";
    for (json::const_iterator it = _OFparts.begin(); it != _OFparts.end(); ++it) {
        string part = "\t";
        part += it.key();
        for (int i = 0; i < 15 - it.key().size(); ++i)
            part += " ";
        part += "{ id: " + it.value()["id"] + ", len: " + it.value()["len"] + " }\n";
        mes += part;
    }
    logger->success("List", mes);
    return;
}

void RPiMgr::quit() {
    return;
}

// private function
size_t RPiMgr::getLEDId() const {
    size_t totalFrame = _LEDJson.size();

    if (totalFrame == 0)
        return 0;
    if (_startTime > (size_t)_LEDJson[totalFrame - 1]["start"])
        return 0;
    if (_startTime == 0)
        return 0;

    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first <= last) {
        size_t mid = (first + last) >> 1;
        if (_startTime > (size_t)_LEDJson[mid]["start"])
            first = mid + 1;
        else if (_startTime == (size_t)_LEDJson[mid]["start"])
            return mid;
        else
            last = mid - 1;
    }
    if (_startTime < (size_t)_LEDJson[first]["start"])
        --first;
    return first;
}

size_t RPiMgr::getOFId() const {
    size_t totalFrame = _OFJson.size();

    if (totalFrame == 0)
        return 0;
    if (_startTime > (size_t)_OFJson[totalFrame - 1]["start"])
        return 0;
    if (_startTime == 0)
        return 0;

    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first <= last) {
        size_t mid = (first + last) >> 1;
        if (_startTime > (size_t)_OFJson[mid]["start"])
            first = mid + 1;
        else if (_startTime == (size_t)_OFJson[mid]["start"])
            return mid;
        else
            last = mid - 1;
    }
    if (_startTime < (size_t)_OFJson[first]["start"])
        --first;
    return first;
}

json getLEDFadeStatus(const size_t& currentTime, const json& firstFrame, const json& secondFrame) const {
    const size_t firstTime = firstFrame["start"];
    const size_t secondTime = secondFrame["start"];
    const float rate = (float)(currentTime - firstTime) / (float)(secondTime - firstFrame);

    json status;
    for (json::const_iterator it = firstFrame["status"].begin(); it != firstFrame["status"].end(); ++it) {
        json::const_iterator it2 = secondFrame["status"].find(it.key());

        json part_arr = json::array();
        for (int i = 0; i < _LEDparts[it.key()]["len"]; ++i) {
            json part;
            part["colorCode"] = rgbHexInterpolate(it.value()[i]["colorCode"], it2.value()[i]["colorCode"], rate);
            part["alpha"] = (1 - rate) * float(it.value()[i]["alpha"]) + rate * float(it2.value()[i]["alpha"]);
            part_arr.push_back(part);
        }

        status[it.key()] = part_arr;
    }
    return status;
}

json getOFFadeStatus(const size_t& currentTime, const json& firstFrame, const json& secondFrame) const {
    const size_t firstTime = firstFrame["start"];
    const size_t secondTime = secondFrame["start"];
    const float rate = (float)(currentTime - firstTime) / (float)(secondTime - firstFrame);

    json status;
    for (json::const_iterator it = firstFrame["status"].begin(); it != firstFrame["status"].end(); ++it) {
        json::const_iterator it2 = secondFrame["status"].find(it.key());

        json part;
        part["colorCode"] = rgbHexInterpolate(it.value()["colorCode"], it2.value()["colorCode"], rate);
        part["alpha"] = (1 - rate) * float(it.value()["alpha"]) + rate * float(it2.value()["alpha"]);

        status[it.key()] = part;
    }
    return status;
}

void RPiMgr::lightLEDStatus(const json& LEDstatus) const {
    for (json::const_iterator it = LEDstatus.begin(); it != LEDstatus.end(); ++it) {
        const int len = _LEDparts[it.key()]["len"];
        const int id = _LEDparts[it.key()]["id"];
        for (int i = 0; i < _LEDparts[it.key()]["len"]; ++i) {
            int colorCode = it.value()[i]["colorCode"];
            const float alpha = it.value()[i]["alpha"] / ALPHA_RANGE;
            const char B = colorCode % 256;
            colorCode >>= 8;
            const char G = colorCode % 256;
            colorCode >>= 8;
            const char R = colorCode % 256;

            LEDrgba_to_rgb(LED_buf[id], i, R, G, B, alpha);
        }
    }
    led_strip->sendToStrip(LED_buf);
}

void RPiMgr::lightOFStatus(const json& OFstatus) const {
    for (json::const_iterator it = OFstatus.begin(); it != OFstatus.end(); ++it) {
        int colorCode = it.value()["colorCode"];
        const float alpha = it.value()["alpha"] / ALPHA_RANGE;
        const char B = colorCode % 256;
        colorCode >>= 8;
        const char G = colorCode % 256;
        colorCode >>= 8;
        const char R = colorCode % 256;

        const int id = _OFparts[it.key()];  // 0 ~ 25
        OFrgba_to_rgbiref(OF_buf[id], R, G, B, alpha);
    }
    of->Write(OF_buf);
}

// For threading
void RPiMgr::play_loop(const long startTime, size_t currentLEDId, size_t currentOFId) {
    const long sysStartTime = getsystime();
    const long localStartTime = startTime;
    while (_playing) {
        // both
        if (_startTime >= (size_t)max(_LEDJson[_LEDJson.size() - 1]["start"], _OFJson[_OFJson.size() - 1]["start"])) {
            lightLEDStatus(_LEDJson[_LEDJson.size() - 1]["status"]);
            lightOFStatus(_OFJson[_OFJson.size() - 1]["status"]);
            _playing = false;
            break;
        }
        // LED
        if (_startTime >= (size_t)_LEDJson[currentLEDId + 1]["start"]) {
            ++currentLEDId;
            if (_LEDJson[currentLEDId]["fade"])
                lightLEDStatus(getLEDFadeStatus(_startTime, _LEDJson[currentLEDId], _LEDJson[currentLEDId + 1]));
            else
                lightLEDStatus(_LEDJson[currentLEDId]["status"]);
        } else {
            if (_LEDJson[currentLEDId]["fade"])
                lightLEDStatus(getLEDFadeStatus(_startTime, _LEDJson[currentLEDId], _LEDJson[currentLEDId + 1]));
        }
        // OF
        if (_startTime >= (size_t)_OFJson[currentOFId + 1]["start"]) {
            ++currentOFId;
            if (_OFJson[currentOFId]["fade"])
                lightOFStatus(getOFFadeStatus(_startTime, _OFJson[currentOFId], _OFJson[currentOFId + 1]));
            else
                lightOFStatus(_OFJson[currentOFId]["status"]);
        } else {
            if (_OFJson[currentOFId]["fade"])
                lightOFStatus(getOFFadeStatus(_startTime, _OFJson[currentOFId], _OFJson[currentOFId + 1]));
        }
        // Calculate startTime
        _startTime = localStartTime + (getsystime() - sysStartTime);
    }
    cout << "end playing\n";
}

// Testing and reference
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