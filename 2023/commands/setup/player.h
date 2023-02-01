#ifndef __PROTOTYPES_H__
#define __PROTOTYPES_H__

// #include <math.h>
// #include <sys/time.h>
// #include <time.h>
#include <string.h>

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "nlohmann/json.hpp"

// serialize
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
using namespace std;
using json = nlohmann::json;

struct OFStatus {
    OFStatus(){};
    OFStatus(const int &_r, const int &_g, const int &_b, const int &_a)
        : r(_r), g(_g), b(_b), a(_a){};
    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive &r;
        archive &g;
        archive &b;
        archive &a;
    };

    int r;
    int g;
    int b;
    int a;
};
struct LEDStatus {
    LEDStatus(){};
    LEDStatus(const int &_r, const int &_g, const int &_b, const int &_a)
        : r(_r), g(_g), b(_b), a(_a){};
    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive &r;
        archive &g;
        archive &b;
        archive &a;
    };
    int r;
    int g;
    int b;
    int a;
};
struct LEDStripeSetting {
    LEDStripeSetting(){};
    LEDStripeSetting(const int &_id, const int &_len) : id(_id), len(_len){};
    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive &id;
        archive &len;
    };
    int id;
    int len;
};

class Player {
   public:
    Player(){};
    Player(const string &_dancerName) : dancerName(_dancerName){};
    bool setDancer(const string &_dancerName, json &dancerData);
    string list() const;

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);

   private:
    // Functions

    // For threading

    // Variables
    string dancerName;
    int fps;

    map<string, int> OFPARTS;
    map<string, LEDStripeSetting> LEDPARTS;

    // serialization
    friend class boost::serialization::access;
    friend ostream &operator<<(ostream &ostream, const Player &player);
};
void savePlayer(const Player &savePlayer, const char *filename);
bool restorePlayer(Player &savePlayer, const char *filename);

#endif