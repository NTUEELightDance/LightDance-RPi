#ifndef RPIMGR_H
#define RPIMGR_H

#include <string>
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <thread>
#include <chrono>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <atomic>

#include "nlohmann/json.hpp"
#include "utils.h"
#include "logger.h"
// #include "LED_strip.h"
// #include "EL.h"

using namespace std;
using json = nlohmann::json;

// EL el1(16, 0x40), el2(8, 0x60);

extern Logger* logger;

class RPiMgr {
public:
    RPiMgr();
    RPiMgr(const string& dancerName);
    bool setDancer();  // TODO: LED_Strip
    void pause();
    void load(const string& path = "./data/control.json");
    void play(const bool& givenStartTime, const unsigned& start, const unsigned& delay = 0);
    void stop();
    void statuslight();
    void eltest(int id, unsigned brightness);  // TODO: EL
    void ledtest();  // TODO
    void list();
    void quit();

private:
    // functions
    size_t getFrameId() const;
    json getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& second) const;
    void lightOneStatus(const json& status) const;  // TODO: EL
    void play_loop(const long& startTime, size_t currentFrameId);  // For threading

    string _dancerName;
    json LEDJson;
    json _ctrlJson;
    json _ELparts;
    json _LEDparts;
    atomic<bool> _playing;
    bool _loaded;
    atomic<size_t> _startTime;
    // LED_Strip* led_strip;
};

#endif