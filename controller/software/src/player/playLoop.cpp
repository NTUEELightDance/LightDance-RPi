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

#define MAXLEN 100

timeval baseTime, startTime;
long stopTime, delayTime;
bool stopTimeAssigned = false;
bool playing = false, paused = false, stopped = true, delaying = false;
bool to_terminate = false;
enum CMD { PLAY, PAUSE, STOP };
std::string cmds[10] = {"play", "pause", "stop"};

std::thread led_loop, of_loop;

Player player;
LEDPlayer led_player;
OFPlayer of_player;

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
    return time;
}

bool restart() {
    printf("restart\n");
    playing = false;

    if (!restorePlayer(player, "/home/pi/LightDance-RPi/dancer.dat")) {
        fprintf(stderr, "restorePlayer ERROR\n");
        return false;
    }
    led_player = player.myLEDPlayer;
    led_player.init();
    of_player = player.myOFPlayer;
    of_player.init();
    fprintf(stderr, "Player loaded\n");

    to_terminate = false;
    led_loop = std::thread(&LEDPlayer::loop, &led_player, &playing, &baseTime, &to_terminate);
    of_loop = std::thread(&OFPlayer::loop, &of_player, &playing, &baseTime, &to_terminate);
    return true;
}

void stop() {
    printf("stop\n");
    playing = paused = stopTimeAssigned = delaying = false;
    stopped = to_terminate = true;
    if (led_loop.joinable()) {
        led_loop.join();
    }
    if (of_loop.joinable()) {
        of_loop.join();
    }
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
                delayTime = 0;
                delaying = false;
                if (paused && !(cmd.size() == 1 || (cmd.size() == 3 && cmd[1] == "-d"))) {
                    paused = false;
                    stop();
                    if (!restart()) {
                        return -1;
                    }
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
    playing = false;
    timeval playedTime;
    long s = -1;
    while (1) {
        timeval tv;
        tv = getCalculatedTime(baseTime);
        if (stopTimeAssigned && !paused && !stopped && !delaying) {
            long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
            // if (tv.tv_sec != s) {
            //     printf("cnt %ld\n",tv.tv_sec);
            //     s = tv.tv_sec;
            // }
            if (played_us > stopTime && stopTime != -1) {
                stopTimeAssigned = false;
                stop();
                s = -1;
            }
        }
        if (delaying) {
            long delayed_us = tv.tv_sec * 1000000 + tv.tv_usec;
            // if (tv.tv_sec != s) {
            //     printf("delay_cnt %ld\n",tv.tv_sec);
            //     s = tv.tv_sec;
            // }
            if (delayed_us > delayTime) {
                delaying = false;
                if (paused) {
                    printf("resume\n");
                    baseTime = getCalculatedTime(playedTime);
                } else {
                    printf("play\n");
                    baseTime = getCalculatedTime(startTime);
                }
                playing = true;
                paused = false;
                s = -1;
            }
        }

        n = read(rd_fd, cmd_buf, MAXLEN);
        std::string cmd_str = cmd_buf;
        if (n > 0) {
            // fprintf(stderr, "n: %d\n", n);
            int cmd = parse_command(cmd_buf);
            fprintf(stderr, "cmd_buf: %s, cmd: %d\n", cmd_buf, cmd);
            switch (cmd) {
                case PLAY:
                    fprintf(stderr, "ACTION: play\n");
                    playing = false;
                    if (stopped) {
                        if (!restart()) {
                            break;
                        }
                    }
                    gettimeofday(&baseTime, NULL);
                    printf("start delay\n");
                    delaying = true;
                    stopped = false;
                    break;
                case PAUSE:
                    fprintf(stderr, "ACTION: pause\n");
                    if (paused || stopped || delaying) break;
                    printf("pause\n");
                    playing = false;
                    paused = true;
                    playedTime = getCalculatedTime(baseTime);
                    break;
                case STOP:
                    fprintf(stderr, "ACTION: stop\n");
                    if (!stopped) stop();
                    break;
                default:
                    break;
            }
        }
    }
    close(rd_fd);
    return 0;
}
