#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

#include "LEDPlayer.h"
#include "OFPlayer.h"
#include "player.h"

#define MAXLEN 20

bool playing;
timeval baseTime;
enum CMD { SET, PLAY, PAUSE, RESUME, STOP };
Player player;
const char* fileName;

int parse_command(char cmd[]) {
    // TBD with the person who wrote the commands
    if (strcmp(cmd, "set\n") == 0) {
        // NOTE: this should be "load"
        return SET;
    } else if (strcmp(cmd, "play\n") == 0) {
        // TODO: parse start time and duration
        return PLAY;
    } else if (strcmp(cmd, "pause\n") == 0) {
        return PAUSE;
    } else if (strcmp(cmd, "resume\n") == 0) {
        return RESUME;
    } else if (strcmp(cmd, "stop\n") == 0) {
        return STOP;
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

int main(int argc, char* argv[]) {
    // std::string fileName = argv[1];
    std::string fileName = "dancer.dat";

    // read command
    int fd;
    // TODO: separate in/out fifo
    const char* fifo = "fifo";
    printf("%s\n", fileName.c_str());

    mkfifo(fifo, 0666);

    char cmd[MAXLEN];
    playing = false;
    timeval playedTime;

    while (1) {
        // TODO: fool proofing, return error with fifo
        fd = open(fifo, O_RDONLY);
        read(fd, cmd, MAXLEN);

        // printf("received: %s", cmd);
        close(fd);

        switch (parse_command(cmd)) {
            case SET:
                playing = false;
                if (!restorePlayer(player, fileName.c_str())) {
                    exit(1);
                };
                break;
            case PLAY:
                playing = false;
                gettimeofday(&baseTime, NULL);
                playing = true;
                break;
            case PAUSE:
                playing = false;
                playedTime = getCalculatedTime(baseTime);
                break;
            case RESUME:
                playing = true;
                timeval currentTime;
                gettimeofday(&currentTime, NULL);
                baseTime = getCalculatedTime(playedTime);
                break;
            case STOP:
                playing = false;
                // darkAll();
                break;
            default:
                break;
        }
    }
    return 0;
}
