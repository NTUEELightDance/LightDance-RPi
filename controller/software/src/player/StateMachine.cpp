# include <StateMachine.h>
# include <FSM_Common.h>
# include <iostream>

//enum CMD { C_PLAY, C_PAUSE, C_STOP, C_RESUME };
extern const std::string cmds[10];
extern std::thread led_loop, of_loop;
extern Player player;
extern LEDPlayer led_player;
extern OFPlayer of_player;
extern int dancer_fd;
extern string path;
extern const char *rd_fifo;
extern const char *wr_fifo;
StateMachine::StateMachine(){
    timeval tv;
    tv.tv_sec=tv.tv_usec=0;  
    setData(tv,tv,-1,0,false,false);
    currentState = nextState = S_STOP;
    fprintf(stderr, "StateMachine::StateMachine()\n");
}


int StateMachine::getCurrentState(){
    //fprintf(stderr, "StateMachine::getCurrentState()\n");
    return currentState;
}   

void StateMachine::transition(int cmd){
    
    if (TransitionTable[currentState][cmd]==CANNOT_HAPPEN){
        cerr<<"[FSM] Invalid Transition"<<endl;
    }
    else if(TransitionTable[currentState][cmd]!=EVENT_IGNORE){
       
        fprintf(stderr,"[FSM] Start Transition\n");
        fprintf(stderr,"[FSM] currentState:%d\n",currentState);
	    (this->*EX_func[currentState])();
        currentState=TransitionTable[currentState][cmd];
	    fprintf(stderr,"[FSM] nextState: %d\n",currentState);
        (this->*EN_func[currentState])();
	    fprintf(stderr,"[FSM] Finish Transition\n");
    }
    else{
    	cerr<<"[FSM] Event Ignored\n";
    }

    return;
}

/*void StateMachine::setState(int nextState){
    currentState=nextState;
    return;
}*/

void StateMachine::ST_Play() {
    timeval tv;
    
    tv = getCalculatedTime(data.baseTime);
    long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
    played_us/=1000;	    
    if (played_us > this->data.stopTime && this->data.stopTime != -1) {

        (this->*EX_func[currentState])();
        currentState= S_STOP;
        (this->*EN_func[currentState])();
    }
}

void StateMachine::ST_Pause() {
}

void StateMachine::ST_Stop() {
}

void StateMachine::EX_Play() {
    fprintf(stderr, "exit STATE_PLAY\n");
    this->data.playedTime = getCalculatedTime(data.baseTime);
    return;
}

void StateMachine::EX_Pause() {	
    fprintf(stderr, "exit STATE_PAUSE\n");
    restart();
}

void StateMachine::EX_Stop() {
    fprintf(stderr, "exit STATE_STOP\n");
    restart();
    data.delayDisplay=true;
}

void StateMachine::EN_Play() {

    fprintf(stderr, "[FSM] ENTERING PLAY\n");
    fprintf(stderr,"[FSM] delayTime[%ld]\n",data.delayTime);
    while (data.delayTime>0)
    {   
        timeval tv = getCalculatedTime(data.baseTime);
       	long delayed_us = tv.tv_sec * 1000000 + tv.tv_usec;
	    // delayed_us/=1000;
        of_player.delayDisplay(&data.delayDisplay);
        led_player.delayDisplay(&data.delayDisplay);
	    //fprintf(stderr,"delayDisplay[%d],delayed_us[%d],delayTime[%d]\n",data.delayDisplay,delayed_us,data.delayTime);
        if (delayed_us > data.delayTime / 5l&&data.delayDisplay){
	        data.delayDisplay = false;
       	    cerr << "[FSM] display off\n" << endl;
	    }
        if (delayed_us > data.delayTime) {
            cerr << "[FSM] delay out\n" ;
            break;
        }
    }
    cerr << "[FSM] Delay finished\n"<< "Start playing\n";
    data.delayTime=0;
    data.baseTime = getCalculatedTime(data.playedTime);
    cerr << "[FSM] startTime: " << data.playedTime.tv_sec << " " << data.playedTime.tv_usec <<"\n";
    resume(this);
}

void StateMachine::EN_Pause() {  
    fprintf(stderr, "[FSM] ENTERING PAUSE\n");
    Loop_Join();
    releaseLock(dancer_fd, path.c_str());
}

void StateMachine::EN_Stop() {
    fprintf(stderr, "[FSM] ENTERING STOP\n");
    Loop_Join();
    led_player.darkAll();
    of_player.darkAll();
    led_player.controller.finish();
    releaseLock(dancer_fd, path.c_str());
    data.stopTimeAssigned = data.delayDisplay = false;
    data.stopTime = -1;
}

void StateMachine::Loop_Join(){
    if(led_loop.joinable()){
        led_loop.join();
        cerr << "[FSM] led_loop joined" << endl;
    }
    else{
        cerr << "[FSM] led_loop not joinable" << endl;
    }
    if(of_loop.joinable()){
        of_loop.join();
        cerr << "[FSM] of_loop joined" << endl;
    }
    else{
        cerr << "[FSM] of_loop not joinable" << endl;
    }
    return;
}

timeval StateMachine::getPlayedTime() {
    timeval tv = getCalculatedTime(data.baseTime);
    data.playedTime=tv;
    fprintf(stderr, "[FSM] playedTime: %ld %ld\n", tv.tv_sec, tv.tv_usec);
    return tv;
}

void StateMachine::setData(timeval _baseTime, timeval _playedTime, long _stopTime, long _delayTime, bool _stopTimeAssigned, bool _isLiveEditting){
    data.baseTime = _baseTime;
    data.playedTime = _playedTime;
    data.stopTime = _stopTime;
    data.delayTime = _delayTime;
    data.delayDisplay=false;
    data.stopTimeAssigned = _stopTimeAssigned;
    data.isLiveEditting = _isLiveEditting;
}

