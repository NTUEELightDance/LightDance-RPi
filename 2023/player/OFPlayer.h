#ifndef OF_PLAYER
#define OF_PLAYER

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <map>
#include <vector>

#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

namespace OFPlayer {

typedef struct Status {
    int r, g, b, a;
    Status();
    Status(const int &_r, const int &_g, const int &_b, const int &_a);
} Status;

typedef struct Frame {
    long start;
    bool fade;
    vector<pair<string, Status>> status;
    Frame();
    Frame(const long &_start, const bool &_fade, const json &_status);
} Frame;

extern vector<OFPlayer::Frame> frameList;
extern vector<vector<Status>> statusList;
// status[channelId] = status
extern int frameId;
extern vector<string> channels;
extern unordered_map<string, int> channelIds;
extern int fps;
// status return to main logic
void load(const json &data_json, const json &parts_json, const int &_fps);
// functions for finding frame at specific time

vector<Status> findFrameStatus(const long &time);
vector<Status> findFadeFrameStatus(const long &time);
int findFrameId(const long &time);
int findChannelId(const string &partName);

// function for calculating time
long timeTransformation(const struct timeval &tm);

void loop();
};  // namespace OFPlayer

#endif
