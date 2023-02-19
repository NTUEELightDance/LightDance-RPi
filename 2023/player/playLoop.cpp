#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>

#include <string.h>

// #include "LEDPlayer.h"
// #include "OFPlayer.h"

#define MAXLEN 100

timeval baseTime, startTime;
long stopTime;
bool stopTimeAssigned = false, success = false;
bool playing = false, paused = false, stopped = true;
bool to_terminate = false;
enum CMD {PLAY, PAUSE, STOP};
const char *cmds[10] = {"play","pause","stop"};

// std::thread led_loop, of_loop;

// LEDPlayer led_player;
// OFPlayer of_player;

void write_fifo(bool success) {
    int wr_fd;
    const char *wr_fifo = "/tmp/report";
    std::string msg;
    mkfifo(wr_fifo,0666);

    wr_fd = open(wr_fifo,O_WRONLY);
    if(success) {
        msg = "success";
    }
    else {
        msg = "something went wrong";
    }
    write(wr_fd,msg.c_str(),msg.length()+1);
    close(wr_fd);
}

int parse_command(char str[]) {
    if(strlen(str) == 1) return -1;
    str[strlen(str)-1] = '\0';
    char *cmd[MAXLEN];
    const char *sep = " ";
    int len = 0, i;
    cmd[len] = strtok(str, sep);
    
    while (cmd[len] != NULL) {
        len++;
        cmd[len] = strtok(NULL, sep);
    }
    for(i=0; i<3; i++) {
    	if(!strcmp(cmd[0],cmds[i])) {
            if(i == PLAY && !(len == 1 && paused)) {
                long start_usec = 0;
                stopTimeAssigned = false;
                if(len > 1) {
                    start_usec = atoi(cmd[1]);
                }
                if(len > 2) {
                    stopTimeAssigned = true;
                    stopTime = atoi(cmd[2]);
                }
                startTime.tv_sec = start_usec / 1000000;
                startTime.tv_usec = start_usec % 1000000;
            }
            success = true;
            write_fifo(true);
            return i;
        }
	}
    // write_fifo(false);
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
    // restoreLEDPlayer(led_player,"data/10_dontstop.dat");
    // restoreOFPlayer(of_player,"data/10_dontstop.dat");

    // led_loop = std::thread(LEDPlayer::loop, &led_player, &playing, &baseTime, &to_terminate);
    // of_loop = std::thread(OFPlayer::loop);
}

void stop() {
    printf("stop\n");
    playing = paused = false;
    stopped = to_terminate = true;
    // led_loop.join();
    // of_loop.join();
}

int main(int argc, char* argv[]) {
    int rd_fd, n;
    const char* rd_fifo = "/tmp/cmd";
    mkfifo(rd_fifo, 0666);
    rd_fd = open(rd_fifo, O_RDONLY | O_NONBLOCK);
    if (rd_fd == -1)
        perror("open");

    int flags;
    flags = fcntl(rd_fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(rd_fd, F_SETFL, flags);

    char cmd[MAXLEN];
    playing = false;
    timeval playedTime;

    while (1) {        
        if(stopTimeAssigned && !paused) {
            timeval tv;
            tv = getCalculatedTime(baseTime);
            long played_us = tv.tv_sec*1000000 + tv.tv_usec;
            if(played_us > stopTime) {
                stopTimeAssigned = false;
                stop();
            }
        }
        
        n = read(rd_fd, cmd, MAXLEN);
        if(n >= 0) {
            switch (parse_command(cmd)) {
                case PLAY:
                    playing = false;
                    if(stopped) {
                        restart();
                    }
                    if(paused) {
                        printf("resume\n");
                        baseTime = getCalculatedTime(playedTime);
                    }
                    else {
                        printf("play\n");
                        baseTime = getCalculatedTime(startTime);
                    }
                    playing = true;
                    paused = stopped = false;
                    break;
                case PAUSE:
                    if(paused || stopped) break;
                    printf("pause\n");
                    playing = false;
                    paused = true;
                    playedTime = getCalculatedTime(baseTime);
                    break;
                case STOP:
                    if(stopped) break;
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
