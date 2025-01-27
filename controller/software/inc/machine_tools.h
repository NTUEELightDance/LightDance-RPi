#ifndef _MACINE_TOOLS_H_
#define _MACINE_TOOLS_H_

#include <thread>

#include <LEDPlayer.h>
#include <OFPlayer.h>
#include <player.h>
#include <const.h>
#include <utils.h>
#include <timeval_tools.h>

extern const char *TAG;

std::thread led_loop, of_loop;
Player player;
LEDPlayer led_player;
OFPlayer of_player;
int dancer_fd;
const string path_to_dat = string(BASE_PATH) + "data/dancer.dat";

void delayDisplay(bool display)
{
    led_player.delayDisplay(display);
    of_player.delayDisplay(&display);
    return;
}

timeval getCalculatedTime(timeval subtrahend) {
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    timeval time;
    time.tv_sec = currentTime.tv_sec - subtrahend.tv_sec;
    time.tv_usec = currentTime.tv_usec - subtrahend.tv_usec;
    if (time.tv_usec < 0) {
        time.tv_sec--;
        time.tv_usec += 1000000;
    }
    return time;
}

bool restart() {
    fprintf(stderr, "%srestart\n", TAG);
    dancer_fd = tryGetLock(path_to_dat.c_str());
    if (dancer_fd == -1) {
        fprintf(stderr, "%sDancer is playing! Please stop it first!\n", TAG);
        return 0;
    } else if (dancer_fd == -2) {
        fprintf(stderr, "%sdancer.dat cannot found\n", TAG);
        return 0;
    }

    if (!restorePlayer(player, path_to_dat.c_str())) {
        fprintf(stderr, "%scannot restore player\n", TAG);
        return false;
    }
    led_player = player.myLEDPlayer;
    led_player.init();
    of_player = player.myOFPlayer;
    of_player.init();
    fprintf(stderr, "%sPlayer loaded\n", TAG);
    return true;
   }

void resume( StateMachine* fsm ){
	led_loop = std::thread(&LEDPlayer::loop, &led_player, fsm);
	of_loop = std::thread(&OFPlayer::loop, &of_player, fsm);
	//led_loop.detach();
	//of_loop.detach();
	// pthread_create(&led_loop, NULL,&LEDPlayer::loop_helper, &led_player, fsm);
	// pthread_create(&of_loop, NULL,&OFPlayer::loop_helper, &of_player , fsm);
	cerr << "[Common] thread running\n";
	return;
}

void Loop_Join()
{
    if(led_loop.joinable()){
        led_loop.join();
        fprintf(stderr, "%sled_loop joined\n", TAG);
    }
    else{
        fprintf(stderr, "%sled_loop cannot join\n", TAG);
    }
    if(of_loop.joinable()){
        of_loop.join();
        fprintf(stderr, "%sof_loop joined\n", TAG);
    }
    else{
        fprintf(stderr, "%sof_loop cannot join\n", TAG);
    }
    return;
}

#endif // _MACINE_TOOLS_H_