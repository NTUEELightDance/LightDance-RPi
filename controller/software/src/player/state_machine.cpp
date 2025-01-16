#include "state_machine.h"

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
}

StateMachine::StateMachine(): m_state(STATE_STOP)
{
    data = {0, 0, -1, 0, false, false};
}