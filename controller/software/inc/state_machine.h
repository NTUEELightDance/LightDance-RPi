#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <sys/time.h>

enum STATE
{
    STATE_STOP, 
    STATE_PLAY, 
    STATE_PAUSE, 
    NUM_OF_STATES, 
    STATE_NULL
};

enum EVENT
{
    EVENT_PLAY, 
    EVENT_STOP, 
    EVENT_PAUSE, 
    EVENT_RESUME,
    NUM_OF_EVENTS, 
    EVENT_NULL
};

struct playLoop_Data
{
    timeval baseTime;
    timeval playedTime;
    long stopTime;
    long delayTime;
    bool stopTimeAssigned;
    bool delayDisplay;
    bool isLiveEditting;
};

class StateMachine
{
private: 
    STATE m_state;
    playLoop_Data data;
    static const STATE m_transition_table[NUM_OF_STATES][NUM_OF_EVENTS];
    void exitState(STATE state) const;
    void enterState(STATE state) const;

public: 
    StateMachine();
    void processEvent(EVENT event);
    STATE getState() const;

friend class LEDPlayer;
friend class OFPlayer;
};

#endif // _STATE_MACHINE_H_