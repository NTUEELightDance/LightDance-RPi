#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <thread>
#include <vector>

#include "LEDPlayer.h"
#include "OFPlayer.h"
#include "player.h"
#include "utils.h"
#include "FiniteStateMachine.h"

#define MAXLEN 100

timeval baseTime, startTime;
long stopTime, delayTime;
bool stopTimeAssigned = false;
bool paused = false, stopped = true, delaying = false, delayingDisplay = true;

// thread safe
atomic<bool> to_terminate(false);

enum CMD { PLAY, PAUSE, STOP, RESUME };
std::string cmds[10] = {"play", "pause", "stop"};

std::thread led_loop, of_loop;
atomic<bool> led_playing(false), of_playing(false);
atomic<bool> led_finished(false), of_finished(false);

Player player;
LEDPlayer led_player;
OFPlayer of_player;

int dancer_fd;
string path = string(BASE_PATH) + "data/dancer.dat";
const char *rd_fifo = "/tmp/cmd_to_player";
const char *wr_fifo = "/tmp/player_to_cmd";

void write_fifo(bool success) {
    int wr_fd;
    std::string msg;

    wr_fd = open(wr_fifo, O_WRONLY);
    if (success) {
        msg = "0";
    } else {
        msg = "1";
    }
    write(wr_fd, msg.c_str(), msg.length() + 1);
    close(wr_fd);
}

const std::vector<std::string> split(const std::string &str, const std::string &pattern) {
    std::vector<std::string> result;
    std::string::size_type begin, end;

    end = str.find(pattern);
    begin = 0;

    while (end != std::string::npos) {
        if (end - begin != 0) {
            result.push_back(str.substr(begin, end - begin));
        }
        begin = end + pattern.size();
        end = str.find(pattern, begin);
    }

    if (begin != str.length()) {
        result.push_back(str.substr(begin));
    }
    return result;
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
    printf("restart\n");
    // playing = false;
    led_playing = false;
    of_playing = false;

    dancer_fd = tryGetLock(path.c_str());
    if (dancer_fd == -1) {
        cerr << "[Error] Dancer is playing! Please stop it first!\n";
        return 0;
    } else if (dancer_fd == -2) {
        cerr << "[Error] dancer.dat file not found!\n";
        return 0;
    }

    if (!restorePlayer(player, path.c_str())) {
        // fprintf(stderr, "restorePlayer ERROR\n");
        cerr << "[Error] Can't restorePlayer!\n";
        return false;
    }
    led_player = player.myLEDPlayer;
    led_player.init();
    of_player = player.myOFPlayer;
    of_player.init();
    cerr << "Player loaded\n";

    to_terminate = led_finished = of_finished = false;
    led_loop = std::thread(&LEDPlayer::loop, &led_player, &led_playing, &baseTime, &to_terminate,
                           &led_finished);
    of_loop = std::thread(&OFPlayer::loop, &of_player, &of_playing, &baseTime, &to_terminate,
                          &of_finished);
    return true;
}

void stop() {
    fprintf(stderr, "stop\n");
    of_playing = led_playing = paused = stopTimeAssigned = delaying = false;
    stopped = to_terminate = true;
}

int parse_command(std::string str) {
    // printf("str: %s\n", str.c_str());
    if (str.length() == 1) return -1;
    std::vector<std::string> cmd = split(str, " ");
    // for (int i = 0; i < cmd.size(); i++) {
    //     printf("%d: %s\n", i, cmd[i].c_str());
    // }

    for (int i = 0; i < 3; i++) {
        // printf("cmd[0]: '%s', cmds[i]: '%s'\n", cmd[0].c_str(),
        // cmds[i].c_str());
        if (cmd[0] == cmds[i]) {
            if (i == PLAY) {
                gettimeofday(&baseTime, NULL);
                delayTime = 0;
                delaying = false;
                if (paused && (cmd[1] == "0" && cmd[2] == "-1" && cmd[4] == "0")) {
                    write_fifo(true);
                    return RESUME;
                    // if (!restart()) {
                    //     return -1;
                    // }
                }

                long start_usec = 0;
                stopTimeAssigned = false;
                if (cmd.size() >= 3 && cmd[cmd.size() - 2] == "-d") {
                    delayTime = std::stoi(cmd[cmd.size() - 1]);
                    if (cmd.size() > 3) start_usec = std::stoi(cmd[1]);
                    if (cmd.size() > 4) {
                        stopTimeAssigned = true;
                        stopTime = std::stoi(cmd[2]);
                    }
                } else {
                    if (cmd.size() > 1) start_usec = std::stoi(cmd[1]);
                    if (cmd.size() > 2) {
                        stopTimeAssigned = true;
                        stopTime = std::stoi(cmd[2]);
                    }
                }
                startTime.tv_sec = start_usec / 1000000;
                startTime.tv_usec = start_usec % 1000000;
            }
            write_fifo(true);
            // fprintf(stderr, "cmd: %d\n", i);
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    // create player_to_cmd
    if (mkfifo(wr_fifo, 0666) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Cannot create %s\n", wr_fifo);
        } else {
            fprintf(stderr, "%s already exists\n", wr_fifo);
        }
    } else
        fprintf(stderr, "%s created\n", wr_fifo);
    int rd_fd, n;
    if (mkfifo(rd_fifo, 0666) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Cannot create %s\n", rd_fifo);
        } else {
            fprintf(stderr, "%s already exists\n", rd_fifo);
        }
    } else
        fprintf(stderr, "%s created\n", rd_fifo);
    rd_fd = open(rd_fifo, O_RDONLY | O_NONBLOCK);
    if (rd_fd == -1) perror("open");

    // int flags;
    // flags = fcntl(rd_fd, F_GETFL);
    // flags |= O_NONBLOCK;
    // fcntl(rd_fd, F_SETFL, flags);

    char cmd_buf[MAXLEN];
    // playing = false;
    of_playing = false;
    led_playing = false;
    timeval playedTime;
    StateMachine playingState {};
    // long s = -1;
    while (1) {
        // cerr << ".";
        timeval tv;
        // cerr << "cal" << endl;
        tv = getCalculatedTime(baseTime);
        // cerr << "stop" << endl;
        if (stopTimeAssigned && !paused && !stopped && !delaying) {
            long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
            // if (tv.tv_sec != s) {
            //     printf("cnt %ld\n",tv.tv_sec);
            //     s = tv.tv_sec;
            // }
            if (played_us > stopTime && stopTime != -1) {
                stopTimeAssigned = false;
                stop();
                // s = -1;
            }
        }
        // cerr << "delay" << endl;
        if (delaying) {
            long delayed_us = tv.tv_sec * 1000000 + tv.tv_usec;
            // if (tv.tv_sec != s) {
            //     printf("delay_cnt %ld\n",tv.tv_sec);
            //     s = tv.tv_sec;
            // }
            // OF lightall for 1/5 times of delay time
            of_player.delayDisplay(&delayingDisplay);
            led_player.delayDisplay(&delayingDisplay);
            cerr << "display" << endl;
            if (delayed_us > delayTime / 5l) delayingDisplay = false;
            cerr << "display off" << endl;

            if (delayed_us > delayTime) {
                cerr << "delay out" << endl;
                delaying = false;
                if (paused) {
                    fprintf(stderr, "resume\n");
                    baseTime = getCalculatedTime(playedTime);
                    cerr << "playedTime: " << playedTime.tv_sec << " " << playedTime.tv_usec
                         << endl;

                } else {
                    fprintf(stderr, "play\n");
                    baseTime = getCalculatedTime(startTime);
                    cerr << "startTime: " << startTime.tv_sec << " " << startTime.tv_usec << endl;
                    // cerr << delayTime << " " << delayed_us << endl;
                    // stop();
                }
                // playing = true;
                of_playing = true;
                led_playing = true;
                paused = false;
                // s = -1;
            }
            cerr << "delaying " << delaying << endl;
        }

        // cerr << "join" << endl;
        if (!delaying && !led_playing && !of_playing && led_loop.joinable() && of_loop.joinable()) {
            cerr << "[Loop] join" << endl;
            led_loop.join();
            of_loop.join();
            // if (!led_finished) {
            //     led_loop.join();
            //     cerr << "[Loop] LED joined" << endl;
            // } else {
            //     cerr << "[Loop] LED already finished" << endl;
            // }
            // if (!of_finished) {
            //     of_loop.join();
            //     cerr << "[Loop] OF joined" << endl;
            // } else {
            //     cerr << "[Loop] OF already finished" << endl;
            // }
            led_playing = of_playing = stopTimeAssigned = delaying = false;
            stopped = to_terminate = true;
            // to_terminate = true;
            cerr << "[Loop] finished" << endl;
            releaseLock(dancer_fd, path.c_str());
        }

        n = read(rd_fd, cmd_buf, MAXLEN);
        // cerr << "readed" << endl;
        std::string cmd_str = cmd_buf;
        if (n > 0) {

            // fprintf(stderr, "n: %d\n", n);
            int cmd = parse_command(cmd_buf);
            int trans = PS.transition(cmd);
            if(trans == -1){
                playingState.stating(playingState.currentState);
            }else{
                playingState.exiting(playingState.currentState);
                playingState.entering(playingState.newState);
                playingState.currentState=playingState.newState;
                playingState.stating(playingState.currentState);
            }

            fprintf(stderr, "[Loop] cmd_buf: %s, cmd: %d\n", cmd_buf, cmd);
            /*switch (cmd) {
                case PLAY:
                    cerr << "[Loop] ACTION: play" << endl;
                    // playing = false;
                    of_playing = false;
                    led_playing = false;
                    gettimeofday(&baseTime, NULL);
                    if (stopped) {
                        if (!restart()) {
                            break;
                        }
                    }
                    printf("start delay\n");
                    delaying = delayingDisplay = true;
                    stopped = paused = false;
                    break;
                case PAUSE:
                    if (paused || stopped || delaying) break;
                    // playing = false;
                    if (!of_playing && !led_playing) break;
                    stop();
                    of_playing = false;
                    led_playing = false;
                    stopped = false;
                    paused = true;
                    playedTime = getCalculatedTime(baseTime);
                    fprintf(
                        stderr, "[Loop] ACTION: pause at %s\n",
                        parseMicroSec(playedTime.tv_sec * 1000000 + playedTime.tv_usec).c_str());
                    break;
                case STOP:
                    cerr << "[Loop] ACTION: stop" << endl;
                    if (!stopped || paused) stop();
                    break;
                case RESUME:
                    cerr << "[Loop] ACTION: resume" << endl;
                    // playing = false;
                    of_playing = false;
                    led_playing = false;
                    gettimeofday(&baseTime, NULL);
                    if (stopped) {
                        if (!restart()) {
                            break;
                        }
                    }
                    printf("start delay\n");
                    delaying = true;
                    stopped = false;
                    break;

                default:
                    break;
            }
        }*/
    }
        else{
           playingState.stating(playingState.currentState); 
        }
    close(rd_fd);
    return 0;
}
