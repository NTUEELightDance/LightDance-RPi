#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include <sys/time.h>
#include <sstream>
#include <mutex>
#include <condition_variable>

#define DEFAULT_DELAY_DISPLAY_RATIO 0.2

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
    // specified
    timeval start_time_stamp;
    timeval stop_time_stamp;
    timeval delay_duration;
    float delay_display_ratio;
    // private record
    timeval start_time;
    timeval enter_delay_time;
    timeval enter_pause_time;
    timeval total_pause_time;
    timeval curr_time_stamp;
};

class StateMachine
{
private: 
    STATE m_state;
    playLoop_Data data;
    std::mutex mtx_loop;
    std::condition_variable cv_loop;
    bool LEDReady;
    bool OFReady;
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
    timeval getCurrTimeStamp() const;
    void setStartTime(timeval _start_time_stamp);
    void setStopTime(timeval _stop_time_stamp);
    void setDelayTime(timeval _delay_time, float _delay_display_ratio = DEFAULT_DELAY_DISPLAY_RATIO);
    STATE getCurrState() const;

friend class LEDPlayer;
friend class OFPlayer;
};

EVENT parse_event(const char *str);
std::string stateToStr(STATE state);
std::string eventToStr(EVENT event);

#endif // _STATE_MACHINE_H_