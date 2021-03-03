/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include <time.h>
#include <sys/time.h>
#include "dbJson.h"
#include "util.h"
//#include "nlohmann/json.hpp"

using namespace std;
//using json = nlohmann::json;

//extern void sigHandler(int sig);

static long getsystime() // ms
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    //return (tv.tv_sec*1000 + tv.tv_usec/1000);
    return tv.tv_usec/1000;
    
}

bool
RPIMgr::setDancer(const string& name) {
    string path = "./data/" + name + ".json";
    ifstream infile(path.c_str());
    if (!infile) {
        cerr << "Error: cannot load file " << path << endl;
        return false;
    }
    json j;
    infile >> j;
    _ELparts.clear();
    _LEDparts.clear();
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        if (it.key() == "ELPARTS")
            _ELparts = it.value();
        if (it.key() == "LEDPARTS")
            _LEDparts = it.value();
    }
    cout << name << " success loaded." << endl;
    return true;
}
bool
RPIMgr::load(const string& path) {
    ifstream infile(path.c_str());
    if (!infile) {
        cerr << "Error: cannot load file " << path << endl;
        return false;
    }
    _loaded = true;
    infile >> _ctrlJSON;
    cout << "success loading file from " << path << endl;
    return true;
}

void
RPIMgr::play(bool givenTime, size_t startTime) {
    //signal(SIGINT, sigHandler);
    if (!_loaded) {
        cerr << "Error: need to load control.json first" << endl;
        return;
    }
    if (givenTime)
        _startTime = startTime;
    _playing = true;
    if (_ctrlJSON.size() == 0) {
        cout << "Warning: there is no frame in control.json" << endl;
        cout << "End of playing" << endl;
        return;
    }
    if (_startTime > _ctrlJSON[_ctrlJSON.size()-1]["start"]) {
        cout << "Warning: startTime exceed total time" << endl;
        cout << "End of playing" << endl;
        return;
    }
    size_t currentFrameId = getFrameId();
    cout << currentFrameId << endl;
    if (_ctrlJSON[currentFrameId]["fade"])
        lightOneStatus(getFadeStatus(_startTime, _ctrlJSON[currentFrameId], _ctrlJSON[currentFrameId+1]));
    else
        lightOneStatus(_ctrlJSON[currentFrameId]["status"]);
    long sysStartTime = getsystime();
    while (_playing) {
        cout << "Time: " << _startTime << " FrameId: " << currentFrameId << endl;
        if (_startTime >= _ctrlJSON[_ctrlJSON.size()-1]["start"]) {
            lightOneStatus(_ctrlJSON[_ctrlJSON.size()-1]["status"]);
            cout << "End of playing" << endl;
            _playing = false;
            break;
        }
        if (_startTime >= _ctrlJSON[currentFrameId+1]["start"]) {
            currentFrameId ++;
            if (_ctrlJSON[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJSON[currentFrameId], _ctrlJSON[currentFrameId+1]));
            else
                lightOneStatus(_ctrlJSON[currentFrameId]["status"]);
        }
        else {
            if (_ctrlJSON[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJSON[currentFrameId], _ctrlJSON[currentFrameId+1]));
        }
        /*
        if (_ctrlJSON[currentFrameId]["fade"]) {
            cout << "fade" << endl;
            lightOneStatus(getFadeStatus(_startTime, _ctrlJSON[currentFrameId], _ctrlJSON[currentFrameId+1]));
        }
        else
            lightOneStatus(_ctrlJSON[currentFrameId]["status"]);
        */
        _startTime += (getsystime() - sysStartTime);
    }
}

void
RPIMgr::testEL() const {
    
}

void
RPIMgr::testLED() const {
    
}

void
RPIMgr::listPart() const {
    cout << "ELPARTS:" << endl;
    for (json::const_iterator it = _ELparts.begin(); it != _ELparts.end(); ++it) {
        cout << '\t' << setw(15) << left << it.key()
        << it.value() << endl;
    }
    cout << "LEDPARTS:" << endl;
    for (json::const_iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++it) {
        cout << '\t' << setw(15) << left << it.key()
        << it.value() << endl;
    }
}

bool
RPIMgr::statusLight() const {
    ifstream infile("./data/status.json");
    if (!infile) {
        cerr << "Error: cannot open ./data/status/json" << endl;
        return false;
    }
    json status;
    infile >> status;
    lightOneStatus(status);
    return true;
}

void
RPIMgr::lightOneStatus(const json& status) const {
    //*
    for (json::const_iterator it = status.begin(); it != status.end(); ++it) {
        json::const_iterator temp = _ELparts.find(it.key());
        if (temp != _ELparts.end()) {   //ELparts
            cout << "ELlightName: " << it.key() << ", "
                 << "alpha: " << it.value() << ", "
                 << "number: " << temp.value() << endl;
        }
        else {
            temp = _LEDparts.find(it.key());
            if (temp != _LEDparts.end()) {  //LEDparts
                cout << "LEDlightName: " << it.key() << ", "
                << "alpha: " << (it.value()["alpha"]) << ", "
                << "number: " << temp.value() << ", "
                << "src: " << (it.value()["src"]) << endl;
            }
            else
                cerr << "Error: lightName " << it.key() << " not found!" << endl;
        }
    }
     //*/
}

size_t
RPIMgr::getFrameId() const {
    size_t totalFrame = _ctrlJSON.size();
    if (totalFrame == 0) {
        cout << "Error: totalFrame is 0" << endl;
        return 0;
    }
    if (_startTime > _ctrlJSON[totalFrame-1]["start"]) {
        cout << "Error: startTime exceed total time" << endl;
        return 0;
    }
    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first < last) {
        size_t mid = (first + last) / 2;
        if (_startTime < _ctrlJSON[mid]["start"])
            last = mid-1;
        else if (_startTime > _ctrlJSON[mid]["start"])
            first = mid+1;
        else
            return mid;
    }
    if (_ctrlJSON[first]["start"] > _startTime)
        first--;
    return first;
}

json
RPIMgr::getFadeStatus(const size_t& currentTime, const json& firstFrame, const json& secondFrame) const {
    size_t firstTime = firstFrame["start"];
    size_t secondTime = secondFrame["start"];
    float rate = (float)(currentTime-firstTime) / (float)(secondTime-firstTime);
    json ret;
    for (json::const_iterator it = firstFrame["status"].begin(); it != firstFrame["status"].end(); ++it) {
        if (_ELparts.find(it.key()) != _ELparts.end()) {
            json::const_iterator it2 = secondFrame["status"].find(it.key());
            ret[it.key()] = (1-rate) * float(it.value()) + rate * float(it2.value());
        }
        else if (_LEDparts.find(it.key()) != _LEDparts.end()) {
            json LEDinfo;
            LEDinfo["src"] = it.value()["src"];
            json::const_iterator it2 = secondFrame["status"].find(it.key());
            LEDinfo["alpha"] = (1-rate) * float(it.value()["alpha"]) + rate * float(it2.value()["alpha"]);
            if (it.value()["src"] != it2.value()["src"]) {
                cout << "Error: the src in two fade frame is different" << endl;
            }
            ret[it.key()] = LEDinfo;
        }
        else
            cerr << "Error: light name " << it.key() << " not found!" << endl;
    }
    return ret;
    
}
/*
void
RPIMgr::sigHandler(int sig) {
    //playing = false;
    cout << "startTime: " << _startTime << endl;
}
*/
