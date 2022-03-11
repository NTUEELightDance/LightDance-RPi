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
#include "gamma_correction.h"
#include "ledplayer.h"
#include "logger.h"
#include "nlohmann/json.hpp"
#include "ofplayer.h"
#include "pca.h"
#include "utils.h"

using namespace std;
using json = nlohmann::json;

#define TOTAL_LED_PARTS 16
#define ALPHA_RANGE 15
#define NUM_OF 26
#define NUM_OF_PARAMS 6

extern Logger* logger;

class RPiMgr {
   public:
    RPiMgr();
    RPiMgr(const string& dancerName);
    bool setDancer();
    void pause();
    void load(const string& path = "../asset/");
    void play(const bool& givenStartTime, const unsigned& start, const unsigned& delay = 0);
    void stop();
    void statuslight();  // TODO
    void LEDtest();      // TODO
    void OFtest();       // TODO
    void list();
    void quit();

   private:
    // Functions
    void lightLEDStatus(const LEDPlayer::frame& frame, const int& channel_id);
    void lightOFStatus(const OFPlayer::frame& frame);
    // For threading
    void play_loop(const long startTime);

    // Variables
    vector<LEDPlayer> led_players;
    OFPlayer of_player;

    vector<vector<char>> LED_buf;
    vector<vector<char>> OF_buf;

    string _dancerName;
    atomic<bool> _playing;
    bool _loaded;
    atomic<size_t> _startTime;
    LED_Strip* led_strip;
    PCA* of;
};

#endif