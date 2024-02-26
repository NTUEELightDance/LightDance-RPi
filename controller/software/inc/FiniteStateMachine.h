#include <functional>
#include <iostream>
enum Event{
    PLAY,
    PAUSE,
    STOP,
    RESUME
};
class StateMachine{
public:
    StateMachine();
    enum {EVENT_IGNORE=0xef, CANNOT_HAPPEN=0xff};
private:
    enum {S_PLAY, S_PAUSE, S_STOP};
    int currentState, newState;
    timeval baseTime, playedTime;
//state function
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

int transistion(int);
void stating(int);
void entering(int);
void exiting(int);

int TransitionTable[3][4]={
    {S_PAUSE, EVENT_IGNORE, EVENT_IGNORE, S_STOP},
    {EVENT_IGNORE, S_PLAY, EVENT_IGNORE, S_STOP},
    {EVENT_IGNORE, EVENT_IGNORE, S_PLAY, EVENT_IGNORE}
};
};
