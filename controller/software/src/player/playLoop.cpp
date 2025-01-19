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
#include <sstream>

#include "LEDPlayer.h"
#include "OFPlayer.h"
#include "player.h"
#include "utils.h"
#include "state_machine.h"
#include "const.h"

#define MAXLEN 100

// enum CMD { C_PLAY, C_PAUSE, C_STOP, C_RESUME };
extern const std::string cmds[10];
extern std::thread led_loop, of_loop;
extern Player player;
extern LEDPlayer led_player;
extern OFPlayer of_player;
extern int dancer_fd;
extern string path;

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
    char cmd_buf[MAXLEN];
    // of_playing = false;
    // led_playing = false;
    // timeval playedTime;
    StateMachine* fsm = new StateMachine();

    while (1) {     
        n = read(rd_fd, cmd_buf, MAXLEN);
        if (n > 0) {
            stringstream ss(cmd_buf);
            string cmd;
            ss >> cmd;
            fprintf(stderr,"[playLoop] parsing command\n");
	        EVENT event = parse_event(cmd.c_str());
            fprintf(stderr, "[playLoop] cmd_buf: %s, event: %d\n", cmd_buf, event);
	        if(event == EVENT_NULL) continue;
            fsm->processEvent(event);
        }
        else{
           fsm->execCurrState();
        }
    }
    close(rd_fd);
    return 0;
}
