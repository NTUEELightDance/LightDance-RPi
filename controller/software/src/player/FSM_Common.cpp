


#include <sys/time.h>
#include <iostream>
#include <thread> 
#include <const.h>
#include <StateMachine.h>
#include <LEDPlayer.h>
#include <OFPlayer.h>
#include <player.h>
#include <string>
#include <FSM_Common.h>

//enum CMD { C_PLAY, C_PAUSE, C_STOP, C_RESUME };
const std::string cmds[10] = {"play", "pause", "stop"};
//std::thread led_loop, of_loop;
std::thread led_loop, of_loop;
Player player;
LEDPlayer led_player;
OFPlayer of_player;
int dancer_fd;
string path = string(BASE_PATH) + "data/dancer.dat";
const char *rd_fifo = "/tmp/cmd_to_player";
const char *wr_fifo = "/tmp/player_to_cmd";

/*inline void write_fifo(bool success) {
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
inline bool restart(StateMachine* fsm) {
    printf("restart\n");
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
    led_loop = std::thread(&LEDPlayer::loop, &led_player, fsm);
    of_loop = std::thread(&OFPlayer::loop, &of_player, fsm);
    return true;
}
inline int parse_command(StateMachine* fsm,std::string str) {
    if (str.length() == 1) return -1;
    std::vector<std::string> cmd = split(str, " ");
    string cmds[3]= {"play", "pause", "stop"};
    int cmd_recv=-1;
    long startusec=0;
    for (int i = 0; i < 3; i++) {
        if (cmd[0] == cmds[i]) {
            if (i == C_PLAY) {
            fsm->data.delayTime=0;
            gettimeofday(&fsm->data.baseTime, NULL);//for delay display
            if(fsm->getCurrentState()==S_PAUSE && cmd[1]=="0"&& cmd[2] == "-1" && cmd[4] == "0"){
                write_fifo(true);
                cmd_recv=C_RESUME;
                return cmd_recv;
            }
            else if (cmd.size() >= 3 && cmd[cmd.size() - 2] == "-d") {
                fsm->data.delayTime = std::stoi(cmd[cmd.size() - 1]);
                if (cmd.size() > 3) {
                    startusec = std::stoi(cmd[1]);
                }
                if (cmd.size() > 4) {
                    fsm->data.stopTime = std::stoi(cmd[2]);
                    fsm->data.stopTimeAssigned = true;
                }
            } else {
                if (cmd.size()>1) {
                    startusec = std::stoi(cmd[1]);
                }
                if (cmd.size() > 2) {
                    fsm->data.stopTime = std::stoi(cmd[2]);
                    fsm->data.stopTimeAssigned = true;
                }
            }
            fsm->data.playedTime.tv_sec = startusec / 1000000;
            fsm->data.playedTime.tv_usec = startusec % 1000000;
            }
            cmd_recv=i;
            write_fifo(true);
            return cmd_recv;
        }
    }
    return -1;
}*/

/*inline void stop(StateMachine* fsm) {
    fprintf(stderr, "stop\n");
    // of_playing = led_playing = paused = 
    fsm->data.stopTimeAssigned = delaying = false;
    fsm->data.stopTime = -1;
    fsm->setState(S_STOP);
}*/

