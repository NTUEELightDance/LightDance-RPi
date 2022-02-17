#include "../inc/rpiMgr.h"

RPiMgr::RPiMgr(const string &dancerName):_dancerName(dancerName){
    ifstream infile("./asset/LED.json", ios::in);
    if (!infile)
        cerr << "Error: cannot open ./asset/LED.json" << endl;
    else {
        infile >> LEDJson;
        for (json::iterator it = LEDJson.begin(); it != LEDJson.end(); ++it){
            for (json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2){
                json j;
                for (int i = 0; i < it2.value().size(); ++i){
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

bool RPiMgr::setDancer(){
    string path = "./data/dancers/" + _dancerName + ".json";
    ifstream infile(path.c_str());
    if (!infile){
        string msg = "file " + path + " not found";
        logger->error(msg.c_str());
        return false; 
    }
    json j;
    infile >> j;
    _ELparts.clear();
    _LEDparts.clear();
    for (json::iterator it = j.begin(); it != j.end(); ++ it){
        if (it.key() == "ELPARTS")
            _ELparts = it.value();
        if (it.key() == "LEDPARTS")
            _LEDparts = it.value();
    }

    uint8_t strips = _LEDparts.size();
    uint16_t *nLEDs = new uint16_t[strips];
    for (json::iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++ it)
        nLEDs[(int)(it.value()["id"])] = uint16_t(it.value()["len"]);

    // led_strip = new LED_Strip(strips, nLEDs);
    cout << "successfully set dancer\n";
    return true;
}

void RPiMgr::pause(){
    _playing = false;
}

void RPiMgr::load(const string& path){
    if (_playing){
        logger->error("Load", "Cannot load while playing");
        return;
    }
    string msg = "Loading " + path;
    ifstream infile(path.c_str());
    if (!infile){
        msg += "\nFailed cannot open file: " + path;
        logger->error("Load", msg);
        return;
    }
    infile >> _ctrlJson;
    _loaded = true;
    logger->success("Load", msg);
}

void RPiMgr::play(const bool& givenStartTime, const unsigned& start, const unsigned& delay){
    long timeIntoFunc = getsystime();
    if (!_loaded){
        logger->error("Play", "Play failed, need to load first");
        return;
    }
    if (_ctrlJson.size() == 0){
        logger->log("Play", "Warning: control.json is empty\nend of play\n");
        return;
    }
    if (givenStartTime)
        _startTime = start;
    if (_startTime > (unsigned long)_ctrlJson[_ctrlJson.size() - 1]["start"]){
        logger->log("Play", "Warning: startTime excess totalTime\nend of playing");
        return;
    }

    size_t currentFrameId = getFrameId();
    long hadDelay = getsystime() - timeIntoFunc;
    if (hadDelay < delay)
        this_thread::sleep_for(chrono::microseconds(delay - hadDelay));
    logger->success("Play", "Start play loop thread");

    if (_ctrlJson[currentFrameId]["fade"])
        lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
    else
        lightOneStatus(_ctrlJson[currentFrameId]["status"]);

    long sysStartTime = getsystime();
    _playing = true;
    long startTime = (long)_startTime;

    thread loop(&RPiMgr::play_loop, this, startTime, currentFrameId);
    loop.detach();
    
    return;
}

void RPiMgr::stop(){
    _playing = false;
    _startTime = 0;
    eltest(-1, 0);
    return;
}

void RPiMgr::statuslight(){
    ifstream infile("./data/status.json");
    if (!infile){
        logger->error("Statuslight", "Cannot open ./data/status.json");
        return;
    }
    json status;
    infile >> status;
    lightOneStatus(status);
    logger->success("Statuslight");
    return;
}

void RPiMgr::eltest(int id, unsigned brightness){
    if (brightness > 4095){
        // logger->log("Warning: brightness is bigger than 4095, light brightness as 4095");
        brightness = 4095;
    }
    if (id < 0){
        for (int i = 0; i < 32; ++ i){
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

void RPiMgr::ledtest(){
    // TODO
    return;
}

void RPiMgr::list(){
    string mes = "ELPARTS:\n";
    for (json::const_iterator it = _ELparts.begin(); it != _ELparts.end(); ++ it){
        string part = "\t";
        part += it.key();
        for (int i = 0; i < 15 - it.key().size(); ++ i)
            part += " ";
        part += it.value();
        mes += part + "\n";
    }
    mes += "LEDPARTS:\n";
    for (json::const_iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++ it){
        string part = "\t";
        part += it.key();
        for (int i = 0; i < 15 - it.key().size(); ++ i)
            part += " ";
        part += it.value();
        mes += part + "\n";
    }
    logger->success("List", mes);
    return;
}

void RPiMgr::quit(){
    return;
}

// private function
size_t RPiMgr::getFrameId() const{
    size_t totalFrame = _ctrlJson.size();
    if (totalFrame == 0){
        // logger->log("Warning: totalFrame is 0\n");
        return 0;
    }
    if (_startTime > (unsigned long)_ctrlJson[totalFrame - 1]["start"]){
        // logger->log("Warning: startTime exceed total time\n");
        return 0;
    }
    if (_startTime == 0)
        return 0;
    
    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first <= last){
        size_t mid = (first + last) / 2;
        if (_startTime > (unsigned long)_ctrlJson[mid]["start"])
            first = mid + 1;
        else if (_startTime == (unsigned long)_ctrlJson[mid]["start"])
            return mid;
        else
            last = mid - 1;
    }
    if (_ctrlJson[first]["start"] > (unsigned long)_startTime)
        -- first;
    return first;
}

json RPiMgr::getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& secondStatus) const {
    size_t firstTime = firstStatus["start"];
    size_t secondTime = secondStatus["start"];
    float rate = (float)(currentTime - firstTime) / (float)(secondTime - firstTime);
    json ret;
    for (json::const_iterator it = firstStatus["status"].begin(); it != firstStatus["status"].end(); ++it){
        if (_ELparts.find(it.key()) != _ELparts.end()) {
            json::const_iterator it2 = secondStatus["status"].find(it.key());
            float temp = (1 - rate) * float(it.value()) + rate * float(it2.value());
            ret[it.key()] = roundf(temp * 10) / 10.0;
        }
        else if (_LEDparts.find(it.key()) != _LEDparts.end()) {
            json LEDinfo;
            LEDinfo["src"] = it.value()["src"];
            json::const_iterator it2 = secondStatus["status"].find(it.key());
            float temp = (1 - rate) * float(it.value()["alpha"]) + rate * float(it2.value()["alpha"]);
            LEDinfo["alpha"] = roundf(temp * 10) / 10.0;
            ret[it.key()] = LEDinfo;
        }
        else
            cerr << "Error: light name " << it.key() << " not found!" << endl;
    }
    return ret;
}

void RPiMgr::lightOneStatus(const json &status) const {
    for (json::const_iterator it = status.begin(); it != status.end(); ++it) {
        json::const_iterator temp = _ELparts.find(it.key());
        if (temp != _ELparts.end()) {
            //ELparts
            uint8_t id = temp.value();
            uint16_t dt = getELBright(it.value());
            // if (id < 16)
            //     el1.setEL(id, dt);
            // else
            //     el2.setEL(id % 16, dt);
	    }
        else {
            temp = _LEDparts.find(it.key());
            if (temp != _LEDparts.end()) { 
                //LEDparts
                uint8_t id = temp.value()["id"];
                size_t len = temp.value()["len"];
                uint8_t* color = new uint8_t[len];
                for (int i = 0; i < len; ++i) {
                    color[i] = uint8_t(LEDJson[it.key()][string(it.value()["src"])][i]);
                }
                // led_strip->sendToStrip(id, color);
                delete[] color;
	        }
            else
                cerr << "Error: lightName " << it.key() << " not found!" << endl;
        }
    }
}

// For threading
void RPiMgr::play_loop(const long startTime, size_t currentFrameId){
    const long sysStartTime = getsystime();
    int localStartTime = startTime;
    while (_playing){
        if (_startTime >= (unsigned long)_ctrlJson[_ctrlJson.size() - 1]["start"]){
            lightOneStatus(_ctrlJson[_ctrlJson.size() - 1]["status"]);
            _playing = false;
            break;
        }
        if (_startTime >= (unsigned long)_ctrlJson[currentFrameId + 1]["start"]){
            ++ currentFrameId;
            if (_ctrlJson[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
            else
                lightOneStatus(_ctrlJson[currentFrameId]["status"]);
        }
        else{
            if (_ctrlJson[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
        }
        
        _startTime = localStartTime + (getsystime() - sysStartTime);
    }
    cout << "end playing\n";
}