#ifndef RPIMGR_H
#define RPIMGR_H
#include <string>
#include <iostream>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

class RPiMgr
{
public:
    RPiMgr(){};
    RPiMgr(string dancerName) : _dancerName(dancerName){};
    bool setDancer();
    void load(const string& path="../data/control.json");
    void play(bool givenSTartTime, unsigned start, unsigned delay = 0);
    void stop();
    void statuslight();
    void list();
    void eltest();
    void ledtest();
    void quit();

private:
    string _dancerName;
    json _ctrlJson;
    json _ELparts;
    json _LEDparts;
    size_t _startTime;
    bool _loaded;
    bool _playing;
    //function
    size_t getFrameId() const;
    json getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& secondStatus) const;
    void lightOneStatus(const json& status) const;
};

#endif
