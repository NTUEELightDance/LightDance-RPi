#include "state_machine.h"
#include <machine_tools.h>
#include <timeval_tools.h>

#define DATA_RESET {TIME_NULL, TIME_NULL, TIME_ZERO, TIME_NULL, TIME_ZERO, false, 0}

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
    data.played_time += (curr_time - data.start_time);
}

void StateMachine::exitPAUSE()
{
    fprintf(stderr, "%sexitPAUSE\n", TAG);
    restart();
}

void StateMachine::exitDELAY()
{
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
    if (curr_time-data.start_time > data.stop_time && data.stop_time != TIME_NULL) 
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
    if(curr_time > data.start_time || data.start_time == TIME_NULL)
    {
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
    gettimeofday(&(data.enter_play_time), NULL);
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
    // set start time
    data.start_time = data.start_time + data.delay_time;
    resume(this);
}

StateMachine::StateMachine(): m_state(STATE_STOP)
{
    data = {0, 0, -1, 0, false, false};
}

StateMachine::~StateMachine()
{
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
        fprintf(stderr, "%sProcess invalid event. Current state: %d, event got: %d. \n", TAG, m_state, event);
        return false;
    }
    fprintf(stderr, "%sProcess event: %d. \n", TAG, event);
    // update base time 
    gettimeofday(&(data.base_time), NULL);
    exitState(m_state);
    m_state = next_state;
    enterState(m_state);
    return true;
}

void StateMachine::execCurrState() 
{
    execState(m_state);
}

STATE StateMachine::getState() const 
{
    return m_state;
}

void StateMachine::setStartTime(timeval _baseTime, timeval _delay = TIME_ZERO) 
{
    data.start_time = _baseTime + _delay;
}

void StateMachine::setStopTime(timeval _stopTime) 
{
    data.stop_time = _stopTime;
}

void StateMachine::setDelayTime(timeval _delayTime) 
{
    data.delay_time = _delayTime;
}

timeval StateMachine::getStartTime() const 
{
    return data.start_time;
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