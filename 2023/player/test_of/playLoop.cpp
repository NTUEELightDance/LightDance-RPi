#include "OFPlayer.h"

#include <fstream>
#include <stdio.h>
#include <sys/select.h>
#include <thread>
#include <time.h>
#include <unistd.h>

#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

bool playing;
struct timeval baseTime;

int main() {
    ifstream OFIfs("./OF.json");
    json OFJson = json::parse(OFIfs);

    ifstream dancerIfs("./10_dontstop.json");
    json dancerJson = json::parse(dancerIfs);

    OFPlayer::load(OFJson, dancerJson["OFPARTS"], dancerJson["fps"]);

    // playing = false;
    // thread led_loop(OFPlayer::loop);
    //
    // gettimeofday(&baseTime, NULL);
    // playing = true;
    //
    // while (true) {
    // }

    return 0;
}
