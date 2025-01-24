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
    timeval time_enter_play;    // time when entering play state
    timeval start_time_stamp;   // specified time stamp to start
    timeval curr_time_stamp;    // current time stamp played, correct when not in play state
    timeval stop_time_stamp;    // specified time stamp to stop, TIME_NULL means not specified
    timeval delay_time;         // specified delay time
    float delay_display_ratio;  // ratio of display red time to delay time
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
    void setStartTime(timeval _start_time_stamp);
    void setStopTime(timeval _stop_time_stamp);
    void setDelayTime(timeval _delay_time);
    STATE getState() const;
    timeval getStartTime() const;

friend class LEDPlayer;
friend class OFPlayer;
};

EVENT parse_event(const char *str);
std::string stateToStr(STATE state);
std::string eventToStr(EVENT event);

#endif // _STATE_MACHINE_H_