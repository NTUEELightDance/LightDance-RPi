#ifndef LED_PLAYER
#define LED_PLAYER

#include <algorithm>
#include <bits/types/struct_timeval.h>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

namespace LEDPlayer {

typedef struct Status {
    int r, g, b, a;
    Status();
    Status(const uint &_r, const uint &_g, const uint &_b, const uint &_a);
    Status(const uint &colorCode, const uint &alpha);
} Status;

typedef struct Frame {
    int start;
    bool fade;
    vector<Status> statusList;
    Frame();
    Frame(const int &_start, const bool &_fade, const int &len);
    Frame(const int &_start, const bool &_fade, const json &_status);
} Frame;

extern int fps;
extern vector<vector<LEDPlayer::Frame>> frameLists;
extern vector<int> frameIds;
extern vector<int> stripShapes;

void load(const json &data, const json &parts, const int &_fps);
// time calculation
long getElapsedTime(const struct timeval &base, const struct timeval &current);
int getTimeId(const long &elapsedTime);
// frame calculation
void calculateFrameIds(const int &timeId);
vector<Status> interpolateFadeFrame(const Frame &origin, const Frame &target, const float &rate);
// threading function
void *loop(void *ptr);

}; // namespace LEDPlayer

#endif // LED_PLAYER
