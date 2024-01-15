#ifndef PLAY_LOOP_STATE
#define PLAY_LOOP_STATE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>  
#include <utility>
#include <iostream>
#define STATE_NUM 3
using namespace std;
enum Event { PLAY, PAUSE, STOP, RESUME,INVALID_CMD=-1}; // Event type
enum { S_PLAY, S_PAUSE, S_STOP };
// typedef void (*Action)(playLoop_Data &data); // action function during state transition
// typedef bool (*Condition)(playLoop_Data data, Event cmd); // condition function for state transition
// typedef void (*StateFunc)();  // function executed in each state
class StateMachine{
    private:
        int currentState,nextState; 
        int TransitionTable[3][4]={
        {EVENT_IGNORE, S_PAUSE,S_STOP, CANNOT_HAPPEN},
        {CANNOT_HAPPEN, EVENT_IGNORE, S_STOP, S_PLAY},
        {S_PLAY, EVENT_IGNORE, EVENT_IGNORE, CANNOT_HAPPEN}
        };
        //state functions
        void ST_Play();
        void ST_Pause();
        void ST_Stop();
        //exit function
        void EX_Play();
        void EX_Pause();
        void EX_Stop();
        //entry function
        void EN_Play();
        void EN_Pause();
        void EN_Stop();
        public:
        enum { EVENT_IGNORE = 0xef, CANNOT_HAPPEN = 0xff };
        playLoop_Data data; 
        StateMachine(); //init from initial state, maybe need to add more init parameters?
        int getCurrentState();
        void transition(int cmd);
        void (StateMachine::*ST_func[3])() = {
            &StateMachine::ST_Play,
            &StateMachine::ST_Pause,
            &StateMachine::ST_Stop
        };
        void (StateMachine::*EX_func[3])() = {
            &StateMachine::EX_Play,
            &StateMachine::EX_Pause,
            &StateMachine::EX_Stop
        };
        void (StateMachine::*EN_func[3])() = {
            &StateMachine::EN_Play,
            &StateMachine::EN_Pause,
            &StateMachine::EN_Stop
        };
        //void setState(int nextState);
        void setData(timeval _baseTime, timeval _playedTime, long _stopTime, long _delayTime, bool _stopTimeAssigned, bool _isLiveEditting);
        timeval getPlayedTime();
};


typedef struct {
    timeval baseTime;
    timeval playedTime;
    long stopTime;
    long delayTime;
    bool stopTimeAssigned;
    bool delayDisplay;
    bool isLiveEditting;
} playLoop_Data;


int parse_command(string cmd);
#endif