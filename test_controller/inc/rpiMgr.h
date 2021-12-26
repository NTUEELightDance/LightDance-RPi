#ifndef RPIMGR_H
#define RPIMGR_H

#include <string>
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include "nlohmann/json.hpp"
#include "utils.h"

using namespace std;
using json = nlohmann::json;

class RPiMgr {
public:
    RPiMgr();
    RPiMgr(const string& dancerName, zmq::socket_t& socket);
    bool setDancer();  // TODO
    void pause();
    void load(const string& path = "./data/control.json");  // TODO
    void play(bool givenStartTime, unsigned start, unsigned delay = 0);  // TODO
    void stop();  // TODO
    void statuslight();  // TODO
    void eltest(int id, unsigned brightness);  // TODO
    void ledtest();  // TODO
    void list();  // TODO
    void quit();  // TODO

private:
    string _dancerName;
    json LEDJson;
    json _ctrlJson;
    bool _playing;
    bool _loaded;
    zmq::socket_t& _socket;
};

#endif