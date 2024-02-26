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
#include "StateMachine.h"
#include "FSM_Common.h"

#define MAXLEN 100

//enum CMD { C_PLAY, C_PAUSE, C_STOP, C_RESUME };
extern const std::string cmds[10];
extern std::thread led_loop, of_loop;
extern Player player;
extern LEDPlayer led_player;
extern OFPlayer of_player;
extern int dancer_fd;
extern string path;
extern const char *rd_fifo;
extern const char *wr_fifo;


int main(int argc, char *argv[]){
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
    char cmd_buf[MAXLEN];
    // of_playing = false;
    // led_playing = false;
    // timeval playedTime;
    StateMachine* playingState=new StateMachine();

    while (1) {
        /*timeval tv;
        tv = getCalculatedTime(baseTime);
        // This may be packed into ST_PLAY
        if (playingState.getCurrentState() == S_PLAY && !delaying) {
            long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
            if (played_us > playingState.data.stopTime && playingState.data.stopTime != -1) {
                stop(&playingState);
            }
        }*/
        // This means Entering S_PLAY ???
        //pack into EN_PLAY
        // printf("entering while loop\n");        
        n = read(rd_fd, cmd_buf, MAXLEN);
        std::string cmd_str = cmd_buf;
        if (n > 0) {
            fprintf(stderr,"[playLoop] parsing command\n");
	    int cmd = parse_command(playingState,cmd_buf);
            fprintf(stderr, "[playLoop] cmd_buf: %s, cmd: %d\n", cmd_buf, cmd);
	    if(cmd==-1)continue;
            playingState->transition(cmd);//trans?
        }
        else{
           playingState->stating(playingState->getCurrentState());
        }
    }
    close(rd_fd);
    return 0;
}
