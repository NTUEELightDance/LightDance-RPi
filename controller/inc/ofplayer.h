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
    typedef struct OFStatus {
        OFStatus() : colorCode(0), alpha(0) {}
        OFStatus(const int& cc, const int& a);
        OFStatus& operator=(const OFStatus&);
        unsigned int colorCode;
        unsigned int alpha;
    } OFStatus;
    typedef struct Frame {
        Frame() : start(0), fade(false) {}
        Frame(const size_t& _start, const bool& _fade, const json& _status);
        size_t start;
        bool fade;
        unordered_map<string, OFStatus> status;
    } Frame;

    void init(const json& OFparts);
    void load(const json& pl);
    Frame getFrame(const size_t& time);
    bool isFinished() const;
    int getFrameNum() const;
    size_t getEndTime() const;
    int getChannelId(const string& part);
    string getParts();

   private:
    size_t getFrameId(const size_t& time);
    Frame getFadeFrame(const size_t& time);

    unordered_map<string, int> channelId;
    size_t frameId;
    vector<Frame> playList;
};

#endif