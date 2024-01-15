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
#include "FSM_Common.h"

#define MAXLEN 100

delaying = false;
delayingDisplay = true;
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
    StateMachine playingState();

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
        /*if (!delaying && playingState.getCurrentState()!=S_PLAY && led_loop.joinable() && of_loop.joinable()) {
            cerr << "[Loop] join" << endl;
            led_loop.join();
            of_loop.join();
            // led_playing = of_playing = stopTimeAssigned = delaying = false;
            // stopped = to_terminate = true;
            stop(&playingState);
            cerr << "[Loop] finished" << endl;
            releaseLock(dancer_fd, path.c_str());
        }*/
        
        n = read(rd_fd, cmd_buf, MAXLEN);
        std::string cmd_str = cmd_buf;
        if (n > 0) {
            int cmd = parse_command(cmd_buf);
            fprintf(stderr, "[Loop] cmd_buf: %s, cmd: %d\n", cmd_buf, cmd);
            int trans = playingState.transition(cmd);
           
        }
        else{
           playingState.ST_func[playingState.getCurrentState()](); 
        }
    }
    close(rd_fd);
    return 0;
}
