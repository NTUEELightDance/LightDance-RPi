#ifndef LEDPLAYER_H
#define LEDPLAYER_H

#include <iostream>
#include <vector>

#include "nlohmann/json.hpp"
#include "utils.h"

using namespace std;
using json = nlohmann::json;

class LEDPlayer {
   public:
    // structures for led usage
    typedef struct LEDStatus {
        LEDStatus(const int& cc, const int& a);
        LEDStatus& operator=(const LEDStatus&);
        unsigned int colorCode;
        unsigned int alpha;
    } LEDStatus;
    typedef struct Frame {
        Frame() : start(0), fade(false) {}
        Frame(const size_t& _start, const bool& _fade, const json& _status);
        Frame& operator=(const Frame&);
        size_t start;
        bool fade;
        vector<LEDStatus> status;
    } Frame;

    LEDPlayer(const string& _name, const int& _len, const int& _id) : name(_name), len(_len), channelId(_id), frameId(0) {}

    void load(const json& pl);
    Frame getFrame(const size_t& time);
    bool isFinished() const;
    int getFrameNum() const;
    size_t getEndTime() const;

    const string name;
    const int len;
    const int channelId;

   private:
    size_t getFrameId(const size_t& time) const;
    Frame getFadeFrame(const size_t& time) const;

    size_t frameId;
    vector<Frame> playList;
};

#endif