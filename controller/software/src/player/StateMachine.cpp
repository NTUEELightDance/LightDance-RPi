# include <StateMachine.h>
# include <FSM_Common.h>

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
}


int StateMachine::getCurrentState(){
    return currentState;
}   

void StateMachine::transition(int cmd){

    if (TransitionTable[currentState][cmd]==CANNOT_HAPPEN){
        cerr<<"[FSM]Invalid Transition"<<endl;
    }
    else if(TransitionTable[currentState][cmd]!=EVENT_IGNORE){
       
        fprintf(stderr,"[FSM]Start Transition\n");
        fprintf(stderr,"[FSM]currentState:%d\n",currentState);
	(this->*EX_func[currentState])();
        currentState=TransitionTable[currentState][cmd];
	fprintf(stderr,"[FSM]nextState: %d\n",currentState);
        (this->*EN_func[currentState])();
	fprintf(stderr,"[FSM]Finish Transition\n");
    }
    else{
    	cerr<<"[FSM]Event Ignored\n";
    }

    return;
}

/*void StateMachine::setState(int nextState){
    currentState=nextState;
    return;
}*/

void StateMachine::ST_Play() {
   // std::cout << "In state PLAY\n";
    timeval tv;
    
    tv = getCalculatedTime(data.baseTime);
    long played_us = tv.tv_sec * 1000000 + tv.tv_usec;
    played_us/=1000;	    
   // fprintf(stderr,"[FSM::PLAY] playedTime[%d],stopTime[%d]\n",played_us,data.stopTime);
    if (played_us > this->data.stopTime && this->data.stopTime != -1) {

        (this->*EX_func[currentState])();
        currentState= S_STOP;
        (this->*EN_func[currentState])();
       /* cerr << "[Loop] join" << endl;
        led_loop.join();
        of_loop.join();
        cerr << "[Loop] finished" << endl;
        releaseLock(dancer_fd, path.c_str());*/
    }
}

void StateMachine::ST_Pause() {
   //cerr<<"[ST_PAUSE]\n"; 
}

void StateMachine::ST_Stop() {

  //  led_player.controller.finish();
}

// Exit functions
void StateMachine::EX_Play() {//EXIT S_PLAY: store playedTime
    this->data.playedTime = getCalculatedTime(data.baseTime);
    return;
}

void StateMachine::EX_Pause() {	
    restart();
}

void StateMachine::EX_Stop() {
    restart();
   // data.playedTime.tv_sec=0;
   // data.playedTime.tv_usec=0;
    data.delayDisplay=true;
}
// Entry functions
void StateMachine::EN_Play() {

    fprintf(stderr, "ENTERING PLAY\n");
    fprintf(stderr,"[DELAY]delayTime[%d]\n",data.delayTime);
    timeval tv;    //delay
    while (data.delayTime>0)
    {
        
        timeval tv = getCalculatedTime(data.baseTime);
       	long delayed_us = tv.tv_sec * 1000000 + tv.tv_usec;
	delayed_us/=1000;
        of_player.delayDisplay(&data.delayDisplay);
        led_player.delayDisplay(&data.delayDisplay);
        cerr << "[DELAY] in loop\n";
	//fprintf(stderr,"delayDisplay[%d],delayed_us[%d],delayTime[%d]\n",data.delayDisplay,delayed_us,data.delayTime);
        if (delayed_us > data.delayTime / 5l&&data.delayDisplay){
	    data.delayDisplay = false;
       	   // cerr << "[DELAY]display off\n" << endl;
	}
        if (delayed_us > data.delayTime) {
            cerr << "[DELAY]delay out\n" << endl;
            fprintf(stderr, "break\n");
            break;
        }
    }
    cerr << "Delay finished " << endl<< "Start playing" << endl;
    data.delayTime=0;
    data.baseTime = getCalculatedTime(data.playedTime);
    cerr << "startTime: " << data.playedTime.tv_sec << " " << data.playedTime.tv_usec << endl;
    resume(this);
}

void StateMachine::EN_Pause() {  
    fprintf(stderr, "ENTERING PAUSE\n");
    Loop_Join();
    releaseLock(dancer_fd, path.c_str());
}

void StateMachine::EN_Stop() {
    fprintf(stderr, "ENTERING STOP\n");
   // pthread_cancel(led_loop);
   // pthread_join(led_loop, NULL);
    Loop_Join();
    led_player.darkAll();
    of_player.darkAll();
    led_player.controller.finish();
    cerr << "[EN_STOP]dark all\n";
    cerr << "[EN_STOP] finished" << endl;
    releaseLock(dancer_fd, path.c_str());
    data.stopTimeAssigned = data.delayDisplay = false;
    data.stopTime = -1;
    //fsm->setState(S_STOP);
}
void StateMachine::Loop_Join(){
    if(led_loop.joinable()) led_loop.join();
    if(of_loop.joinable()) of_loop.join();
}

timeval StateMachine::getPlayedTime() {
    timeval tv = getCalculatedTime(data.baseTime);
    data.playedTime=tv;
    fprintf(stderr, "playedTime: %ld %ld\n", tv.tv_sec, tv.tv_usec);
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

