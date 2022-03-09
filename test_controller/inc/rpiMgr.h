#ifndef RPIMGR_H
#define RPIMGR_H

#include <math.h>
#include <sys/time.h>
#include <time.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <zmq.hpp>

#include "LED_strip.h"
#include "OFrgba_to_rgb.h"
#include "gamma_correction.h"
#include "logger.h"
#include "nlohmann/json.hpp"
#include "pca2022.h"
#include "utils.h"

using namespace std;
using json = nlohmann::json;

#define TOTAL_LED_PARTS 16
#define TOTAL_OF_PARTS 26
#define OF_PARAMS 6
#define ALPHA_RANGE 15

extern Logger* logger;

class RPiMgr {
   public:
    RPiMgr();
    RPiMgr(const string& dancerName);
    bool setDancer();  // TODO: LED_Strip
    void pause();
    void load(const string& path = "./asset/");
    void play(const bool& givenStartTime, const unsigned& start, const unsigned& delay = 0);
    void stop();
    void statuslight();  // TODO
    void LEDtest();      // TODO
    void OFtest();       // TODO
    void list();
    void quit();

   private:
    // Functions
    size_t getLEDId() const;
    size_t getOFId() const;
    json getLEDFadeStatus(const size_t& currentTime, const json& firstFrame, const json& secondFrame) const;
    json getOFFadeStatus(const size_t& currentTime, const json& firstFrame, const json& secondFrame) const;
    void lightLEDStatus(const json& LEDstatus) const;
    void lightOFStatus(const json& OFstatus) const;
    // For threading
    void play_loop(const long startTime, size_t currentLEDId, size_t currentOFId);
    // Testing and reference
    json getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& second) const;
    void lightOneStatus(const json& status) const;

    string _dancerName;
    json _LEDJson;
    json _OFJson;
    json _LEDparts;
    json _OFparts;
    vector<vector<char>> LED_buf;
    vector<vector<char>> OF_buf;
    atomic<bool> _playing;
    bool _loaded;
    atomic<size_t> _startTime;
    LED_Strip* led_strip;
    PCA* of;
};

#endif