#include "state_machine.h"
#include <machine_tools.h>
#include <timeval_tools.h>

#define DATA_RESET playLoop_Data({TIME_ZERO, TIME_ZERO, TIME_ZERO, TIME_NULL, TIME_ZERO, 0.0})

const char *TAG = "[StateMachine]: ";

const STATE StateMachine::m_transition_table[NUM_OF_STATES][NUM_OF_EVENTS] = 
{                  //EVENT_PLAY,    EVENT_STOP,     EVENT_PAUSE,    EVENT_RESUME
/*STATE_STOP*/      {STATE_DELAY,   STATE_NULL,     STATE_NULL,     STATE_NULL}, 
/*STATE_PLAY*/      {STATE_NULL,    STATE_STOP,     STATE_PAUSE,    STATE_NULL}, 
/*STATE_PAUSE*/     {STATE_NULL,    STATE_STOP,     STATE_NULL,     STATE_DELAY}, 
/*STATE_DELAY*/     {STATE_DELAY,   STATE_STOP,     STATE_PAUSE,    STATE_NULL}
};

void StateMachine::exitState(STATE state) 
{
    switch (state)
    {
    case STATE_STOP:
        exitSTOP();
        break;
    case STATE_PLAY:
        exitPLAY();
        break;
    case STATE_PAUSE:
        exitPAUSE();
        break;
    case STATE_DELAY:
        exitDELAY();
    default:
        break;
    }
}

void StateMachine::execState(STATE state) 
{
    switch (state)
    {
    case STATE_STOP:
        execSTOP();
        break;
    case STATE_PLAY:
        execPLAY();
        break;
    case STATE_PAUSE:
        execPAUSE();
        break;
    case STATE_DELAY:
        execDELAY();
    default:
        break;
    }
}

void StateMachine::enterState(STATE state) 
{
    switch (state)
    {
    case STATE_STOP:
        enterSTOP();
        break;
    case STATE_PLAY:
        enterPLAY();
        break;
    case STATE_PAUSE:
        enterPAUSE();
        break;
    case STATE_DELAY:
        enterDELAY();
    default:
        break;
    }
}

void StateMachine::exitSTOP() 
{
    fprintf(stderr, "%sexitSTOP\n", TAG);
    restart();
}

void StateMachine::exitPLAY() 
{
    fprintf(stderr, "%sexitPLAY\n", TAG);
    // record the time interval since start_time to maintain played_time
    timeval curr_time;
    gettimeofday(&curr_time, NULL);
    data.curr_time_stamp += curr_time - data.time_enter_play;
}

void StateMachine::exitPAUSE()
{
    fprintf(stderr, "%sexitPAUSE\n", TAG);
    restart();
}

void StateMachine::exitDELAY()
{
    fprintf(stderr, "%sexitDELAY\n", TAG);
    data.delay_display_ratio = 0;
    data.delay_time = TIME_ZERO;
}

void StateMachine::execSTOP()
{
}

void StateMachine::execPLAY() 
{
    timeval curr_time;
    gettimeofday(&curr_time, NULL);
    // check if stop time is reached
    if (curr_time > data.stop_time_stamp && data.stop_time_stamp != TIME_NULL) 
    {
        exitState(m_state);
        m_state = STATE_STOP;
        enterState(m_state);
    }
}

void StateMachine::execPAUSE() 
{
}

void StateMachine::execDELAY()
{
    // check if time to play, if start time is NULL, play immediately
    timeval curr_time;
    gettimeofday(&curr_time, NULL);
    if(curr_time > data.time_enter_play + data.delay_time)
    {
        fprint_timeval(stderr, curr_time);
        fprintf(stderr, " > ");
        fprint_timeval(stderr, data.time_enter_play);
        fprintf(stderr, " + ");
        fprint_timeval(stderr, data.delay_time);
        fprintf(stderr, "\nshould play\n");
        exitState(m_state);
        m_state = STATE_PLAY;
        enterState(m_state);
    }
}

void StateMachine::enterSTOP() 
{
    fprintf(stderr, "%senterSTOP\n", TAG);
    Loop_Join();
    led_player.darkAll();
    of_player.darkAll();
    led_player.controller.finish();
    releaseLock(dancer_fd, path_to_dat.c_str());
    // reset data 
    data = DATA_RESET;
}

void StateMachine::enterPLAY() 
{
    fprintf(stderr, "%senterPLAY\n", TAG);
    gettimeofday(&data.time_enter_play, NULL);
    resume(this);
}

void StateMachine::enterPAUSE() 
{
    fprintf(stderr, "%senterPAUSE\n", TAG);
    Loop_Join();
    releaseLock(dancer_fd, path_to_dat.c_str());
}

void StateMachine::enterDELAY() 
{
    fprintf(stderr, "%senterDELAY\n", TAG);
    fprintf(stderr, "%sDelay time: ", TAG);
    fprint_timeval(stderr, data.delay_time);
    fprintf(stderr, "\n");
    //resume(this);
}

StateMachine::StateMachine(): m_state(STATE_STOP)
{
    fprintf(stderr, "%sConstructor\n", TAG);
    data = DATA_RESET;
}

StateMachine::~StateMachine()
{
    fprintf(stderr, "%sDestructor\n", TAG);
}

bool StateMachine::processEvent(EVENT event) 
{
    if(event < 0 || event >= NUM_OF_EVENTS)
    {
        fprintf(stderr, "%sInvalid event: %d\n", TAG, event);
        return false;
    }
    STATE next_state = m_transition_table[m_state][event];
    if (next_state == STATE_NULL) 
    {
        fprintf(stderr, "%sProcess invalid event. Current state: %d, event got: %s\n", TAG, m_state, eventToStr(event).c_str());
        return false;
    }
    fprintf(stderr, "%sProcess event: %s. \n", TAG, eventToStr(event).c_str());
    exitState(m_state);
    m_state = next_state;
    enterState(m_state);
    return true;
}

void StateMachine::execCurrState() 
{
    execState(m_state);
}

void StateMachine::setStartTime(timeval _start_time_stamp) 
{
    data.start_time_stamp = _start_time_stamp;
}

void StateMachine::setStopTime(timeval _stop_time_stamp) 
{
    data.stop_time_stamp = _stop_time_stamp;
}

void StateMachine::setDelayTime(timeval _delayTime) 
{
    data.delay_time = _delayTime;
}

timeval StateMachine::getStartTime() const 
{
    return data.time_enter_play;
}

STATE StateMachine::getState() const 
{
    return m_state;
}

EVENT parse_event(const char *str)
{
    if(strlen(str) == 0)
    {
        return EVENT_NULL;
    }
    stringstream ss(str);
    string cmd;
    ss >> cmd;
    if(cmd == "play")
    {
        fprintf(stderr, "%sParsed play\n", TAG);
        return EVENT_PLAY;
    }
    else if(cmd == "stop")
    {
        fprintf(stderr, "%sParsed stop\n", TAG);
        return EVENT_STOP;
    }
    else if(cmd == "pause")
    {
        fprintf(stderr, "%sParsed pause\n", TAG);
        return EVENT_PAUSE;
    }
    else if(cmd == "resume")
    {
        fprintf(stderr, "%sParsed resume\n", TAG);
        return EVENT_RESUME;
    }
    else
    {
        return EVENT_NULL;
    }
}

string stateToStr(STATE state)
{
    switch (state)
    {
    case STATE_STOP:
        return "stop";
    case STATE_PLAY:
        return "play";
    case STATE_PAUSE:
        return "pause";
    default:
        return "unknown";
    }
}

string eventToStr(EVENT eve)
{
    switch (eve)
    {
    case EVENT_PLAY:
        return "play";
    case EVENT_STOP:
        return "stop";
    case EVENT_PAUSE:
        return "pause";
    case EVENT_RESUME:
        return "resume";
    default:
        return "unknown";
    }
}