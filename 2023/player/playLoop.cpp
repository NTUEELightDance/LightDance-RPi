#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>

#include "LEDPlayer.h"
#include "OFPlayer.h"
#include "../commands/setup/player.h"

#define MAXLEN 20

bool playing;
timeval baseTime, assignedTime;
enum CMD {LOAD, PAUSE, RESUME, PLAY, STOP, QUIT};
const char *cmds[10] = {"load","pause","resume","play","stop","quit"};
Player player;
const char* fileName;

// commands can be  "load", "pause", "resume", "stop", "quit", "play -t 12 123", "play"

int parse_command(char str[]) {
    printf("cmd: %s\n",str);
    char *cmd[6];
    const char *sep = " ";
    int len = 0, i;
    cmd[len] = strtok(str, sep);
    
    while (cmd[len] != NULL) {
        len++;
        cmd[len] = strtok(NULL, sep);
    }
    for(i=0; i<6; i++) {
    	if(!strcmp(cmd[0],cmds[i])) {
            if(i == PLAY) {
                if(len > 1) {
                    assignedTime.tv_sec = atoi(cmd[2]);
                    assignedTime.tv_usec = atoi(cmd[3]);
                }
                else assignedTime.tv_sec = assignedTime.tv_usec = 0;
            }
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

void chk() {
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    
    printf("\nbase time:    %ld %ld\n",baseTime.tv_sec,baseTime.tv_usec);
    printf("current time: %ld %ld\n",currentTime.tv_sec,currentTime.tv_usec);
}

int main(int argc, char* argv[]) {
    std::string dancerName = argv[1];
    std::string fileName("../commands/setup/data/");
    fileName += dancerName + "_DancerData.dat";
    printf("restorePlayer path: %s\n", fileName.c_str());

    // read command
    int fd;
    // TODO: separate in/out fifo
    const char* fifo = "fifo";

    mkfifo(fifo, 0666);

    char cmd[MAXLEN];
    playing = false;
    timeval playedTime;

    while (1) {
        fd = open(fifo, O_RDONLY);
        read(fd, cmd, MAXLEN);

        close(fd);

        switch (parse_command(cmd)) {
            case LOAD:
                playing = false;
                // there's some error running restorePlayer here
                // if(!restorePlayer(player, fileName.c_str())) {
                //     exit(1);
                // }
                break;
            case PLAY:
                playing = false;
                baseTime = getCalculatedTime(assignedTime);
                playing = true;
                break;
            case PAUSE:
                playing = false;
                playedTime = getCalculatedTime(baseTime);
                break;
            case RESUME:
                playing = true;
                baseTime = getCalculatedTime(playedTime);
                break;
            case STOP:
                playing = false;
                break;
            case QUIT:
                playing = false;
                break;
            default:
                break;
        }
        chk();
        break;
    }
    return 0;
}
