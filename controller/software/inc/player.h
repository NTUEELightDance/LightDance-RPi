#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string.h>

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>

#include "nlohmann/json.hpp"

// serialize
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/tmpdir.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include "LEDPlayer.h"
#include "OFPlayer.h"
#include "const.h"

using namespace std;
using json = nlohmann::json;

struct LEDStripeSetting {
    LEDStripeSetting(){};
    LEDStripeSetting(const int &_id, const int &_len) : id(_id), len(_len){};
    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {
        archive &id;
        archive &len;
    };
    unsigned int id, len;
};

class Player {
   public:
    Player() : LEDloaded(false), OFloaded(false){};
    Player(const string &_dancerName)
        : dancerName(_dancerName), LEDloaded(false), OFloaded(false){};
    bool setDancer(const string &_dancerName, json &dancerData);
    string list() const;
    string getDancerName();
    template <class Archive>
    void serialize(Archive &archive, const unsigned int version);
    // Variables
    string dancerName;
    bool LEDloaded, OFloaded;
    int fps;

    unordered_map<string, int> OFPARTS;
    unordered_map<string, LEDStripeSetting> LEDPARTS;
    LEDPlayer myLEDPlayer;
    OFPlayer myOFPlayer;

   private:
    // Functions

    // Variables

    // serialization
    friend class boost::serialization::access;
    friend ostream &operator<<(ostream &ostream, const Player &player);
    friend void LEDload(Player &Player, json &data_json);
    friend void OFload(Player &Player, json &data_json);
};

void savePlayer(const Player &savePlayer, const char *filename);
bool restorePlayer(Player &savePlayer, const char *filename);

#endif
