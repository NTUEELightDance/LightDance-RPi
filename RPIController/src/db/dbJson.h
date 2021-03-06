/****************************************************************************
  FileName     [ dbJson.h ]
  PackageName  [ db ]
  Synopsis     [ Define database JSON classes ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef DB_JSON_H
#define DB_JSON_H

#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

class RPIMgr {
    
public:
    RPIMgr() {
        _startTime = 0;
        _loaded = false;
        _playing = false;
        /*
        if (!loadBoardConfig()) {
            cerr << "Error: cannot load board_config.json... exiting" << endl;
            exit(1);
        }
        if (!loadMapping()) {
            cerr << "Error: cannot load dancer.json... exiting" << endl;
            exit(1);
        }
         */
    }
    ~RPIMgr() {}
    bool setDancer(const string& name);
    bool load(const string& path);
    void play(bool givenTime, size_t startTime);
    void pause() {_playing = false;}
    void stop() {_startTime = 0;}
    void testEL() const;
    void testLED() const;
    void listPart() const;
    //void setStartTime(size_t startTime) {_startTime = startTime;}
    size_t getStartTime() const {return _startTime;}
    //void sigHandler(int sig);
    bool statusLight() const;
    //bool get_playing() const {return _playing;}
private:
    size_t _startTime;
    bool _loaded;
    json _ctrlJSON;
    json _ELparts;
    json _LEDparts;
    string _name;
    bool _playing;
    
    //function
    void lightOneStatus(const json& status) const;
    size_t getFrameId() const;
    json getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& secondStatus) const;
    
};


#endif // DB_JSON_H
