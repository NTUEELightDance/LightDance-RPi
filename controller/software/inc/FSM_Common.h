#ifndef FSM_COMMON_H
#define FSM_COMMON_H

#include <sys/time.h>
#include <iostream>
#include <thread> 
#include <const.h>
#include <StateMachine.h>
#include <LEDPlayer.h>
#include <OFPlayer.h>
#include <player.h>
#include <string>


enum CMD { C_PLAY, C_PAUSE, C_STOP, C_RESUME };
extern const std::string cmds[10];
extern std::thread led_loop, of_loop;
extern Player player;
extern LEDPlayer led_player;
extern OFPlayer of_player;
extern int dancer_fd;
extern string path ;
extern const char *rd_fifo;
extern const char *wr_fifo;
inline void write_fifo(bool success) {
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

inline const std::vector<std::string> split(const std::string &str, const std::string &pattern) {
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

inline timeval getCalculatedTime(timeval subtrahend) {
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
inline bool restart() {
    printf("restart\n");
    dancer_fd = tryGetLock(path.c_str());
 if (dancer_fd == -1) {
        cerr << "[Common] Dancer is playing! Please stop it first!\n";
        return 0;
    } else if (dancer_fd == -2) {
        cerr << "[Common] dancer.dat file not found!\n";
        return 0;
    }

    if (!restorePlayer(player, path.c_str())) {
        // fprintf(stderr, "restorePlayer ERROR\n");
        cerr << "[Common] Can't restorePlayer!\n";
        return false;
    }
    led_player = player.myLEDPlayer;
    led_player.init();
    of_player = player.myOFPlayer;
    of_player.init();
    cerr << "[Common] Player loaded\n";
    return true;
   }
inline void resume( StateMachine* fsm ){
	led_loop = std::thread(&LEDPlayer::loop, &led_player, fsm);
	of_loop = std::thread(&OFPlayer::loop, &of_player, fsm);
	//led_loop.detach();
	//of_loop.detach();
	// pthread_create(&led_loop, NULL,&LEDPlayer::loop_helper, &led_player, fsm);
	// pthread_create(&of_loop, NULL,&OFPlayer::loop_helper, &of_player , fsm);
	 cerr << "[Common] thread running\n";
	 return;
   }

inline int parse_command(StateMachine* fsm,std::string str) {
    if (str.length() == 1){
	    write_fifo(false); 
	    return -1;
    }
    std::vector<std::string> cmd = split(str, " ");
    string cmds[3]= {"play", "pause", "stop"};
    int cmd_recv=-1;
    long startusec=0;
    for (int i = 0; i < 3; i++) {
        if (cmd[0] == cmds[i]) {
            if (i == C_PLAY) {	
	            if(fsm->getCurrentState()==S_PLAY) {
		            write_fifo(false);
		            return -1;
	            }
                fsm->data.delayTime=0;
                gettimeofday(&fsm->data.baseTime, NULL);//for delay display
	            if(fsm->getCurrentState()==S_PAUSE && cmd[1]=="0"&& cmd[2] == "-1" && cmd[4] == "0"){
                    write_fifo(true);
                    cmd_recv=C_RESUME;
		            cerr<<"[Common] RESUME\n";
                    return cmd_recv;
                } else if (cmd.size() >= 3 && cmd[cmd.size() - 2] == "-d") {
                    fsm->data.delayTime = std::stoi(cmd[cmd.size() - 1]);//*1000;//saved as us
                    if (cmd.size() > 3) {
                        startusec = std::stoi(cmd[1])*1000;
                    }
                    if (cmd.size() > 4) {
                        fsm->data.stopTime = std::stoi(cmd[2]);
                        fsm->data.stopTimeAssigned = true;
                    }
                } else {
                    if (cmd.size()>1) {
                        startusec = std::stoi(cmd[1])*1000;
		            }
                    if (cmd.size() > 2) {
                        fsm->data.stopTime = std::stoi(cmd[2]);
                        fsm->data.stopTimeAssigned = true;
                    }
                }
	            //fprintf(stderr,"[Common] startusec[%d]\n",startusec);
                fsm->data.playedTime.tv_sec = startusec / 1000000;
                fsm->data.playedTime.tv_usec = startusec % 1000000;
            }
            cmd_recv=i;
	        printf("command parsed\n");
            write_fifo(true);
            return cmd_recv;
        }
    }
    write_fifo(false); 
    return -1;
}

#endif
