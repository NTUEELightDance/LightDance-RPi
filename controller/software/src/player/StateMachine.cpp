# include <StateMachine.h>
# include <FSM_Common.h>
StateMachine::StateMachine(){
    timeval tv;
    tv.tv_sec=tv.tv_usec=0;  
    setData(tv,tv,-1,0,false,false);
    currentState = nextState = S_STOP;
}


int StateMachine::getCurrentState(){
    return currentState;
}   

void StateMachine::transition(int cmd){
    if (TransitionTable[currentState][cmd]==CANNOT_HAPPEN){
        cerr<<"Invalid Transition"<<endl;
        return;
    }
    else if(TransitionTable[currentState][cmd]!=EVENT_IGNORE){
        (this->EX_func[currentState])();
        nextState=TransitionTable[currentState][cmd];
        (this->EN_func[nextState])();
        currentState=nextState;
    }
    return;
}

/*void StateMachine::setState(int nextState){
    currentState=nextState;
    return;
}*/

void StateMachine::ST_Play() {
    std::cout << "In state PLAY\n";
    timeval tv;
    tv = getCalculatedTime(data.baseTime);
    long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
    if (played_us > playingState.data.stopTime && playingState.data.stopTime != -1) {
        (this->EX_func[currentState])();
        currentState=nextState= S_STOP;
        (this->EN_func[currentState])();
    }
}

void StateMachine::ST_Pause() {
    
}

void StateMachine::ST_Stop() {

}

// Exit functions
void StateMachine::EX_Play() {//EXIT S_PLAY: store playedTime
    this->data.playedTime = getCalculatedTime(baseTime);
    return;
}

void StateMachine::EX_Pause() {

}

void StateMachine::EX_Stop() {
     restart(this); 
}

// Entry functions
void StateMachine::EN_Play() {
    timeval tv;    //delay
    while (true)
    {
        tv=this->getPlayedTime();
        long delayed_us = tv.tv_sec * 1000000 + tv.tv_usec;
        of_player.delayDisplay(&delayingDisplay);
        led_player.delayDisplay(&delayingDisplay);
        cerr << "display" << endl;
        if (delayed_us > data.delayTime / 5l) delayingDisplay = false;
        cerr << "display off" << endl;
        if (delayed_us > data.delayTime) {
            cerr << "delay out" << endl;
            fprintf(stderr, "play\n");
            data.baseTime = getCalculatedTime(data.playedTime);
            cerr << "startTime: " << data.playedTime.tv_sec << " " << data.playedTime.tv_usec << endl;
            break;
        }
    }
    cerr << "Delay finished " << endl<< "Start playing" << endl;
}

void StateMachine::EN_Pause() {
    
}

void StateMachine::EN_Stop() {
    fprintf(stderr, "stop\n");
    // of_playing = led_playing = paused = 
    data.stopTimeAssigned = delaying = false;
    data.stopTime = -1;
    //fsm->setState(S_STOP);
}


timeval StateMachine::getPlayedTime() {
    timeval tv = getCalculatedTime(data.baseTime);
    data.playedTime=tv;
    fprintf(stderr, "playedTime: %ld %ld\n", tv.tv_sec, tv.tv_usec);
    return;
}

void StateMachine::setData(timeval _baseTime, timeval _playedTime, long _stopTime, long _delayTime, bool _stopTimeAssigned, bool _isLiveEditting){
    data.baseTime = _baseTime;
    data.playedTime = _playedTime;
    data.stopTime = _stopTime;
    data.delayTime = _delayTime;
    data.stopTimeAssigned = _stopTimeAssigned;
    data.isLiveEditting = _isLiveEditting;
}

