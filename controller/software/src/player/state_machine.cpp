#include "state_machine.h"

const STATE StateMachine::m_transition_table[NUM_OF_STATES][NUM_OF_EVENTS] = 
{
{STATE_PLAY, STATE_NULL, STATE_NULL, STATE_NULL}, 
{STATE_NULL, STATE_STOP, STATE_PAUSE, STATE_NULL}, 
{STATE_NULL, STATE_STOP, STATE_NULL, STATE_PLAY}
};

StateMachine::StateMachine(): m_state(STATE_STOP)
{}