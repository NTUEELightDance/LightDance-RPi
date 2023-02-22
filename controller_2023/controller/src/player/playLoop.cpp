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
long stopTime;
bool stopTimeAssigned = false, success = false;
bool playing = false, paused = false, stopped = true;
bool to_terminate = false;
enum CMD { PLAY, PAUSE, STOP };
// const char *cmds[10] = {"play", "pause", "stop"};
std::string cmds[10] = {"play", "pause", "stop"};

std::thread led_loop, of_loop;

Player player;
LEDPlayer led_player;
OFPlayer of_player;

void write_fifo(bool success) {
    int wr_fd;
    const char *wr_fifo = "/tmp/player_to_cmd";
    std::string msg;
    mkfifo(wr_fifo, 0666);

    wr_fd = open(wr_fifo, O_WRONLY);
    if (success) {
        msg = "0";
    } else {
        msg = "1";
    }
    write(wr_fd, msg.c_str(), msg.length() + 1);
    close(wr_fd);
}

// int parse_command(char str[]) {
//     if (strlen(str) == 1) return -1;
//     str[strlen(str) - 1] = '\0';
//     char *cmd[MAXLEN];
//     const char *sep = " ";
//     int len = 0, i;
//     cmd[len] = strtok(str, sep);
//     while (cmd[len] != NULL) {
//         len++;
//         cmd[len] = strtok(NULL, sep);
//     }
//     for (i = 0; i < 3; i++) {
//         if (!strcmp(cmd[0], cmds[i])) {
//             if (i == PLAY && !(len == 1 && paused)) {
//                 long start_usec = 0;
//                 stopTimeAssigned = false;
//                 if (len > 1) {
//                     start_usec = atoi(cmd[1]);
//                 }
//                 if (len > 2) {
//                     stopTimeAssigned = true;
//                     stopTime = atoi(cmd[2]);
//                 }
//                 startTime.tv_sec = start_usec / 1000000;
//                 startTime.tv_usec = start_usec % 1000000;
//             }
//             success = true;
//             write_fifo(true);
//             fprintf(stderr, "cmd: %d\n", i);
//             return i;
//         }
//     }
//     // write_fifo(false);
//     return -1;
// }

const std::vector<std::string> split(const std::string &str,
                                     const std::string &pattern) {
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
            if (i == PLAY && !(cmd.size() == 1 && paused)) {
                long start_usec = 0;
                stopTimeAssigned = false;
                if (cmd.size() > 1) {
                    start_usec = std::stoi(cmd[1]);
                }
                if (cmd.size() > 2) {
                    stopTimeAssigned = true;
                    stopTime = std::stoi(cmd[2]);
                }
                startTime.tv_sec = start_usec / 1000000;
                startTime.tv_usec = start_usec % 1000000;
            }
            success = true;
            write_fifo(true);
            // fprintf(stderr, "cmd: %d\n", i);
            return i;
        }
    }
    return -1;
}

timeval getCalculatedTime(timeval subtrahend) {
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    timeval time;
    time.tv_sec = currentTime.tv_sec - subtrahend.tv_sec;
    time.tv_usec = currentTime.tv_usec - subtrahend.tv_usec;
    return time;
}

void restart() {
    printf("restart\n");
    playing = false;

    restorePlayer(player, "dancer.dat");
    led_player = player.myLEDPlayer;
    of_player = player.myOFPlayer;
    fprintf(stderr, "Player loaded\n");

    // led_loop = std::thread(&LEDPlayer::loop, &led_player, &playing, &baseTime,
    //                        &to_terminate);
    of_loop = std::thread(&OFPlayer::loop, &of_player, &playing, &baseTime,
                          &to_terminate);
}

void stop() {
    printf("stop\n");
    playing = paused = false;
    stopped = to_terminate = true;
    led_loop.join();
    of_loop.join();
}

int main(int argc, char *argv[]) {
    int rd_fd, n;
    const char *rd_fifo = "/tmp/cmd_to_player";
    mkfifo(rd_fifo, 0666);
    rd_fd = open(rd_fifo, O_RDONLY | O_NONBLOCK);
    if (rd_fd == -1) perror("open");

    // int flags;
    // flags = fcntl(rd_fd, F_GETFL);
    // flags |= O_NONBLOCK;
    // fcntl(rd_fd, F_SETFL, flags);

    char cmd_buf[MAXLEN];
    playing = false;
    timeval playedTime;

    while (1) {
        if (stopTimeAssigned && !paused) {
            timeval tv;
            tv = getCalculatedTime(baseTime);
            long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
            if (played_us > stopTime) {
                stopTimeAssigned = false;
                stop();
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
                        restart();
                        printf("Restart\n");
                    }
                    if (paused) {
                        printf("resume\n");
                        baseTime = getCalculatedTime(playedTime);
                    } else {
                        printf("play\n");
                        baseTime = getCalculatedTime(startTime);
                    }
                    playing = true;
                    paused = stopped = false;
                    break;
                case PAUSE:
                    fprintf(stderr, "ACTION: pause\n");
                    if (paused || stopped) break;
                    printf("pause\n");
                    playing = false;
                    paused = true;
                    playedTime = getCalculatedTime(baseTime);
                    break;
                case STOP:
                    fprintf(stderr, "ACTION: stop\n");
                    if (stopped) break;
                    stop();
                    break;
                default:
                    break;
            }
        }
    }
    close(rd_fd);
    return 0;
}
