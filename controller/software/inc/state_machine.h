#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <sys/time.h>
#include <sstream>

enum STATE
{
    STATE_STOP, 
    STATE_PLAY, 
    STATE_PAUSE, 
    STATE_DELAY, 
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
    timeval enter_play_time; // time when last enterplay
    timeval start_time; //time to start playing 
    timeval played_time; //time interval of played 
    timeval stop_time; //time to stop playing 
    timeval delay_time; //time interval of delay 
    bool stopTimeAssigned; //if stopTime is assigned
    float delay_display_ratio; //if light R in the fractional time of delay
};

class StateMachine
{
private: 
    STATE m_state;
    playLoop_Data data;
    static const STATE m_transition_table[NUM_OF_STATES][NUM_OF_EVENTS];
    void exitState(STATE state);
    void execState(STATE state);
    void enterState(STATE state);
    void exitSTOP();
    void exitPLAY();
    void exitPAUSE();
    void exitDELAY();
    void execSTOP();
    void execPLAY();
    void execPAUSE();
    void execDELAY();
    void enterSTOP();
    void enterPLAY();
    void enterPAUSE();
    void enterDELAY();

public: 
    StateMachine();
    ~StateMachine();
    bool processEvent(EVENT event);
    void execCurrState();
    STATE getState() const;
    void setStartTime(timeval _baseTime, timeval _delay);
    void setStopTime(timeval _stopTime);
    void setDelayTime(timeval _delayTime);
    timeval getStartTime() const;

friend class LEDPlayer;
friend class OFPlayer;
};

EVENT parse_event(const char *str);
std::string stateToStr(STATE state);
std::string eventToStr(EVENT event);

#endif // _STATE_MACHINE_H_