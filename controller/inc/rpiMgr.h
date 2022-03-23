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
    void statuslight();                                          // TODO
    void LEDtest(const int& channel, int colorCode, int alpha);  // Will change led buf
    void OFtest(const int& channel, int colorCode, int alpha);   // Will not change of buf
    void list();
    void quit();
    void darkAll();
    void lightAll(int colorCode, int alpha);  // Will change led buf, but not changing of buf

   private:
    // Functions
    void lightLEDStatus(const LEDPlayer::Frame& frame, const int& channelId);
    void lightOFStatus(const OFPlayer::Frame& frame);
    void ledDark();
    void ofDark();
    // For threading
    void playLoop(const long startTime);

    // Variables
    vector<LEDPlayer> ledPlayers;
    OFPlayer ofPlayer;

    vector<vector<char>> LEDBuf;
    vector<vector<char>> OFBuf;

    string _dancerName;
    atomic<bool> _playing;
    bool _loaded;
    atomic<size_t> _startTime;
    atomic<int> fps;
    LED_Strip* led_strip;
    PCA* of;
};

#endif