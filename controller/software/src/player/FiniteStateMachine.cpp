#include "FiniteStateMachine.h"
StateMachine::StateMachine(){
    baseTime.tv_sec = 0;
    baseTime.tv_usec = 0;
    playedTime.tv_sec = 0;
    playedTime.tv_usec = 0;  
    currentState = S_STOP;
    newState = S_STOP;
}
void StateMachine::ST_Play() {
    std::cout << "In state PLAY\n";
}

void StateMachine::ST_Pause() {
    std::cout << "In state PAUSE\n";
}

void StateMachine::ST_Stop() {
    std::cout << "In state STOP\n";
}

// Exit functions
void StateMachine::EX_Play() {
    std::cout << "Exiting state PLAY\n";
}

void StateMachine::EX_Pause() {
    std::cout << "Exiting state PAUSE\n";
}

void StateMachine::EX_Stop() {
    std::cout << "Exiting state STOP\n";
}

// Entry functions
void StateMachine::EN_Play() {
    std::cout << "Entering state PLAY\n";
}

void StateMachine::EN_Pause() {
    std::cout << "Entering state PAUSE\n";
}

void StateMachine::EN_Stop() {
    std::cout << "Entering state STOP\n";
}

int StateMachine::transistion(int cmd){
    if(TransitionTable[currentState][cmd]!=EVENT_IGNORE){
        newState=TransitionTable[currentState][cmd];
    }
    else return -1;
}

void StateMachine::stating(int state){
    (this->*ST_func[state])();
}

void StateMachine::entering(int state){
    (this->*EN_func[state])();
}

void StateMachine::exiting(int state){
    (this->*EX_func[state])();
}



