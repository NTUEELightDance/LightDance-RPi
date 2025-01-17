#include "state_machine.h"
#include <machine_tools.h>

const STATE StateMachine::m_transition_table[NUM_OF_STATES][NUM_OF_EVENTS] = 
{
{STATE_PLAY, STATE_NULL, STATE_NULL, STATE_NULL}, 
{STATE_NULL, STATE_STOP, STATE_PAUSE, STATE_NULL}, 
{STATE_NULL, STATE_STOP, STATE_NULL, STATE_PLAY}
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
    default:
        break;
    }
}

void StateMachine::exitSTOP() 
{
    restart();
    data.delayDisplay = true;
}

void StateMachine::exitPLAY() 
{
    data.playedTime = getCalculatedTime(data.baseTime);
}

void StateMachine::exitPAUSE()
{
    restart();
}

void StateMachine::execSTOP() 
{
}

void StateMachine::execPLAY() 
{
    timeval tv;
    tv = getCalculatedTime(data.baseTime);
    long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
    played_us /= 1000;
    if (played_us > this->data.stopTime && this->data.stopTime != -1) 
    {
        exitState(m_state);
        m_state = STATE_STOP;
        enterState(m_state);
    }
}

void StateMachine::execPAUSE() 
{
}

void StateMachine::enterSTOP() 
{
    Loop_Join();
    led_player.darkAll();
    of_player.darkAll();
    led_player.controller.finish();
    releaseLock(dancer_fd, path.c_str());
    data.stopTimeAssigned = data.delayDisplay = false;
    data.stopTime = -1;
}

void StateMachine::enterPLAY() 
{
    while (data.delayTime > 0)
    {
        timeval tv = getCalculatedTime(data.baseTime);
        long delayed_us = tv.tv_sec * 1000000 + tv.tv_usec;
        of_player.delayDisplay(&data.delayDisplay);
        led_player.delayDisplay(&data.delayDisplay);
        if (delayed_us > data.delayTime / 5l && data.delayDisplay) 
        {
            data.delayDisplay = false;
            cerr << "[FSM] display off\n" << endl;
        }
        if (delayed_us > data.delayTime) 
        {
            cerr << "[FSM] delay out\n";
            break;
        }
    }
    cerr << "[FSM] Delay finished\n" << "Start playing\n";
    data.delayTime = 0;
    data.baseTime = getCalculatedTime(data.playedTime);
    cerr << "[FSM] startTime: " << data.playedTime.tv_sec << " " << data.playedTime.tv_usec << "\n";
    resume(this);
}

void StateMachine::enterPAUSE() 
{
    Loop_Join();
    releaseLock(dancer_fd, path.c_str());
}

StateMachine::StateMachine(): m_state(STATE_STOP)
{
    data = {0, 0, -1, 0, false, false};
}

void StateMachine::processEvent(EVENT event) 
{
    STATE next_state = m_transition_table[m_state][event];
    if (next_state != STATE_NULL) 
    {
        exitState(m_state);
        m_state = next_state;
        enterState(m_state);
    }
    else 
    {
        cerr << "Invalid event\n";
    }
}

void StateMachine::execCurrState() 
{
    execState(m_state);
}

STATE StateMachine::getState() const 
{
    return m_state;
}

EVENT parse_event(char *str)
{
    if(strlen(str) == 0)
    {
        write_fifo(false);
        return EVENT_NULL;
    }
    stringstream ss(str);
    string cmd;
    ss >> cmd;
    if(cmd == "play")
    {
        write_fifo(true);
        return EVENT_PLAY;
    }
    else if(cmd == "stop")
    {
        write_fifo(true);
        return EVENT_STOP;
    }
    else if(cmd == "pause")
    {
        write_fifo(true);
        return EVENT_PAUSE;
    }
    else if(cmd == "resume")
    {
        write_fifo(true);
        return EVENT_RESUME;
    }
    else
    {
        write_fifo(false);
        return EVENT_NULL;
    }
}