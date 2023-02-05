#include "LEDPlayer.h"

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
    ifstream LEDIfs("./LED.json");
    json LEDJson = json::parse(LEDIfs);

    ifstream dancerIfs("./10_dontstop.json");
    json dancerJson = json::parse(dancerIfs);

    LEDPlayer::load(LEDJson, dancerJson["LEDPARTS"], dancerJson["fps"]);

    playing = false;
    thread led_loop(LEDPlayer::loop);

    gettimeofday(&baseTime, NULL);
    playing = true;

    while (true) {
    }

    return 0;
}
