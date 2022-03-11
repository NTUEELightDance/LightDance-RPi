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
    typedef struct led_status {
        led_status(const int& cc, const int& a);
        led_status& operator=(const led_status&);
        unsigned int colorCode;
        unsigned int alpha;
    } led_status;
    typedef struct frame {
        frame() : start(0), fade(false) {}
        frame(const size_t& _start, const bool& _fade, const json& _status);
        frame& operator=(const frame&);
        size_t start;
        bool fade;
        vector<led_status> status;
    } frame;

    LEDPlayer(const string& _name, const int& _len, const int& _id) : name(_name), len(_len), channel_id(_id), frame_id(0) {}

    void load(const json& pl);
    frame getFrame(const size_t& time);
    bool is_finished() const;
    int getFrameNum() const;
    size_t getEndTime() const;

    const string name;
    const int len;
    const int channel_id;

   private:
    size_t getFrameId(const size_t& time) const;
    frame getFadeFrame(const size_t& time) const;

    size_t frame_id;
    vector<frame> play_list;
};

#endif