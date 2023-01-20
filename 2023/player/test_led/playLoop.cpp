#include "LEDPlayer.h"

#include <fstream>
#include <pthread.h>
#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#include "nlohmann/json.hpp"

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

    pthread_t id;
    pthread_create(&id, NULL, LEDPlayer::loop, NULL);

    gettimeofday(&baseTime, NULL);
    playing = true;

    while (true) {
    }

    return 0;
}
