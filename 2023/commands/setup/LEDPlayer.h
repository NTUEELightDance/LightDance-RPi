#ifndef LED_PLAYER
#define LED_PLAYER

#include <stdio.h>
#include <string.h>
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
#include <vector>

// serialize
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

using namespace std;

struct LEDStatus {
    int r, g, b, a;
    LEDStatus();
    LEDStatus(const uint &_r, const uint &_g, const uint &_b, const uint &_a);

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);
};

struct LEDFrame {
    int start;
    bool fade;
    vector<LEDStatus> statusList;
    LEDFrame();
    LEDFrame(const int &_start, const bool &_fade, const int &len);
    LEDFrame(const int &_start, const bool &_fade,
             const vector<LEDStatus> &_statusList);

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);
};

class LEDPlayer {
   public:
    LEDPlayer();
    LEDPlayer(const int &_fps, const vector<vector<LEDFrame>> &_frameLists,
              const vector<int> &_frameIds, const vector<int> &_stripShapes);
    // threading function
    void loop(const bool *playing, const timeval *baseTime);

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);

    string list() const;
    void save(const char *filename);
    bool restore(const char *filename);

   private:
    int fps;
    vector<vector<LEDFrame>> frameLists;
    vector<int> frameIds;
    vector<int> stripShapes;

    // time calculation
    long getElapsedTime(const struct timeval &base,
                        const struct timeval &current);
    int getTimeId(const long &elapsedTime);
    // frame calculation
    void calculateFrameIds(const int &timeId);
    vector<LEDStatus> interpolateFadeFrame(const LEDFrame &origin,
                                           const LEDFrame &target,
                                           const float &rate);

    // serialization
    friend class boost::serialization::access;
    friend ostream &operator<<(ostream &os, const LEDPlayer &player);
};

#endif  // LED_PLAYER
