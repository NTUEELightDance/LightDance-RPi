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

bool playing;
timeval baseTime;

int main() {
    LEDPlayer player;
    restoreLEDPlayer(player, "data/10_dontstop.dat");

    playing = false;
    thread led_loop(&LEDPlayer::loop, &player, &playing, &baseTime);

    gettimeofday(&baseTime, NULL);
    playing = true;

    sleep(3);
    playing = false;

    sleep(3);
    baseTime.tv_sec += 3;
    playing = true;

    while (true) {
    }

    return 0;
}
