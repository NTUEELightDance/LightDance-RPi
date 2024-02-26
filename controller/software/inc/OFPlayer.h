#ifndef OF_PLAYER
#define OF_PLAYER

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

// serialize
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include "OFController.h"
#include "StateMachine.h"

using namespace std;

struct OFStatus {
    int r, g, b, a;
    OFStatus();
    OFStatus(const int &_r, const int &_g, const int &_b, const int &_a);

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);
};

struct OFFrame {
    int start;
    bool fade;
    vector<pair<string, OFStatus>> statusList;
    OFFrame();
    OFFrame(const int &_start, const bool &_fade,
            const vector<pair<string, OFStatus>> &_statusList);

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);
};

class OFPlayer {
   public:
    OFController controller;

    vector<int> currentStatus;

    OFPlayer();
    OFPlayer(const int &_fps, const vector<OFFrame> &_frameList,
             const vector<vector<OFStatus>> &_statusList, unordered_map<string, int> &_channelIds,
             const int &_OFnum);

    // threading function
   // void* loop_helper(void* context, StateMachine *fsm);
    void loop(StateMachine *fsm);
    void delayDisplay(const bool *delayingDisplay);
    void init();
    void darkAll();
    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);
    string list() const;

   private:
    int fps;
    int frameId;
    int OFnum;
    vector<OFFrame> frameList;
    vector<vector<OFStatus>> statusList;
    unordered_map<string, int> channelIds;

    // functions for finding frame at specific time
    long getElapsedTime(const struct timeval &base, const struct timeval &current);
    vector<OFStatus> findFrameStatus(const long &time);
    vector<OFStatus> findFadeFrameStatus(const long &time);
    int findFrameId(const long &time);
    int findChannelId(const string &partName);
    void setLightStatus(vector<OFStatus> &statusList, int r, int g, int b, int a);

    vector<int> castStatusList(const vector<OFStatus> statusList);

    // serialization
    friend class boost::serialization::access;
    friend ostream &operator<<(ostream &os, const OFPlayer &player);
};

void saveOFPlayer(OFPlayer &player, const char *filename);
bool restoreOFPlayer(OFPlayer &player, const char *filename);

#endif
