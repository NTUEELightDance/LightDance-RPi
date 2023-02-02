#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <thread>

#include "LEDPlayer.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

LEDPlayer load(const json &data_json, const json &parts_json, const int &fps) {
    vector<vector<LEDFrame>> frameLists;
    vector<int> stripShapes;

    // TODO: load from .h file, instead of hard coded
    const int partNum = 16;

    stripShapes.resize(partNum);
    fill(stripShapes.begin(), stripShapes.end(), 0);

    frameLists.clear();
    frameLists.resize(partNum);

    for (auto &part_it : parts_json.items()) {
        const string partName = part_it.key();
        const json part = part_it.value();

        const json &frames_json = data_json[partName];

        const int id = part["id"];
        const int len = part["len"];

        stripShapes[id] = len;

        // If no frames are given, push only dark frame
        const LEDFrame darkFrame(0, false, len);
        if (frames_json.size() == 0) {
            frameLists[id].push_back(darkFrame);
            continue;
        }

        // If not starting at time = 0, use dark frame as first frame
        const int firstStartTime = frames_json.begin().value()["start"];
        if (firstStartTime != 0) {
            frameLists[id].push_back(darkFrame);
        }

        vector<LEDStatus> statusList;
        for (const json &frame_json : frames_json) {
            if (frame_json["status"].size() != len) {
                // TODO: print size mismatch warning
                continue;
            }
            for (auto &status_json : frame_json["status"]) {
                statusList.push_back(LEDStatus(status_json[0], status_json[1],
                                               status_json[2], status_json[3]));
            }

            frameLists[id].push_back(
                LEDFrame(frame_json["start"], frame_json["fade"], statusList));
        }
    }

    return LEDPlayer(fps, frameLists, stripShapes);
}

int main() {
    ifstream LEDIfs("./LED.json");
    json LEDJson = json::parse(LEDIfs);

    ifstream dancerIfs("./10_dontstop.json");
    json dancerJson = json::parse(dancerIfs);

    LEDPlayer player = load(LEDJson, dancerJson["LEDPARTS"], dancerJson["fps"]);
    saveLEDPlayer(player, "data");

    return 0;
}
