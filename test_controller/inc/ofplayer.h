#ifndef OFPLAYER_H
#define OFPLAYER_H

#include <map>
#include <vector>

#include "nlohmann/json.hpp"
#include "utils.h"

using namespace std;
using json = nlohmann::json;

class OFPlayer {
   public:
    // structures for of usage
    typedef struct of_status {
        of_status() : colorCode(0), alpha(0) {}
        of_status(const int& cc, const int& a);
        of_status& operator=(const of_status&);
        unsigned int colorCode;
        unsigned int alpha;
    } of_status;
    typedef struct frame {
        frame() : start(0), fade(false) {}
        frame(const size_t& _start, const bool& _fade, const json& _status);
        size_t start;
        bool fade;
        unordered_map<string, of_status> status;
    } frame;

    void init(const json& OFparts);
    void load(const json& pl);
    frame getFrame(const size_t& time);
    bool is_finished() const;
    int getFrameNum() const;
    size_t getEndTime() const;
    int getChannelId(const string& part);
    string getParts();

   private:
    size_t getFrameId(const size_t& time);
    frame getFadeFrame(const size_t& time);

    unordered_map<string, int> channel_id;
    size_t frame_id;
    vector<frame> play_list;
};

#endif