#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <thread>

#include "OFPlayer.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

OFPlayer load(const json &data_json, const json &parts_json, const int &fps) {
    vector<OFFrame> frameList;
    vector<vector<OFStatus>> statusList;
    unordered_map<string, int> channelIds;

    frameList.clear();
    statusList.clear();
    channelIds.clear();
    
    // build frames
    for (auto &data : data_json) {

         // init darkFrame
        OFFrame darkFrame;
        vector<OFStatus> darkStatus;
        darkStatus.resize(26);

        darkFrame.start = 0;
        darkFrame.fade = false;
        for (auto &part : parts_json.items()) {
            darkFrame.statusList.push_back(
                pair<string, OFStatus>(part.key(), OFStatus(0, 0, 0, 0)));
            darkStatus.push_back(OFStatus(0, 0, 0, 0));
        }
   
        if (data.size() == 0) {
            frameList.push_back(darkFrame);
            statusList.push_back(darkStatus);
            continue;
        }
        
        int _start = data["start"];
        bool _fade = data["fade"];

        // set size of one frame
        // twenty-six channel
        vector<OFStatus> newStatus;
        newStatus.resize(26);

        vector<pair<string, OFStatus>> _statusList;

        // init channelId
        for (auto &part : parts_json.items()) {
            string partName = part.key();
            channelIds[partName] = part.value();
            // map channelId to every part's status and frame
            int channelId = part.value();
            newStatus[channelId] = OFStatus(
                data["status"][partName][0], data["status"][partName][1],
                data["status"][partName][2], data["status"][partName][3]);
            
            // append all status
            for (auto &it : data["status"].items()) {
                _statusList.push_back(pair<string, OFStatus>(
                    it.key(), OFStatus(it.value()[0], it.value()[1], it.value()[2],
                                 it.value()[3])));
            }
        }
        statusList.push_back(newStatus);
        frameList.push_back(OFFrame(_start, _fade, _statusList));
    }

    return OFPlayer(fps, frameList, statusList, channelIds);
}

int main() {
    ifstream OFIfs("./OF.json");
    json OFJson = json::parse(OFIfs);

    ifstream dancerIfs("./10_dontstop.json");
    json dancerJson = json::parse(dancerIfs);

    OFPlayer player = load(OFJson, dancerJson["OFPARTS"], dancerJson["fps"]);
    saveOFPlayer(player, "data/10_dontstop.dat");

    return 0;
}
