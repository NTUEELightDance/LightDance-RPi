# include <StateMachine.h>
# include <FSM_Common.h>
# include <fstream>

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
    fsm_log.open("state_machine.log", ios::trunc);
    fsm_log << "StateMachine::Constructor\n";
    timeval tv;
    tv.tv_sec=tv.tv_usec=0;  
    setData(tv,tv,-1,0,false,false);
    currentState = nextState = S_STOP;
}


int StateMachine::getCurrentState(){
    fsm_log << "StateMachien::getCurrentState\n";
    return currentState;
}   

void StateMachine::transition(int cmd){
    fsm_log << "StateMachine::transition()\n";
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
    fsm_log << "StateMachine::ST_Play()\n";
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
    }
}

void StateMachine::ST_Pause() {
   //cerr<<"[ST_PAUSE]\n"; 
}

void StateMachine::ST_Stop() {
    fsm_log << "StateMachine::ST_Stop()";
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
    fsm_log << "StateMachine::Loop_Join()\n";
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
    fsm_log << "StateMachine::getPlayedTime()\n";
    timeval tv = getCalculatedTime(data.baseTime);
    data.playedTime=tv;
    fprintf(stderr, "[FSM] playedTime: %ld %ld\n", tv.tv_sec, tv.tv_usec);
    return tv;
}

void StateMachine::setData(timeval _baseTime, timeval _playedTime, long _stopTime, long _delayTime, bool _stopTimeAssigned, bool _isLiveEditting){
    fsm_log << "StateMachine::setData()\n";
    data.baseTime = _baseTime;
    data.playedTime = _playedTime;
    data.stopTime = _stopTime;
    data.delayTime = _delayTime;
    data.delayDisplay=false;
    data.stopTimeAssigned = _stopTimeAssigned;
    data.isLiveEditting = _isLiveEditting;
}

StateMachine::~StateMachine()
{
    fsm_log << "StateMachine: Destructor\n";
}

int parse_command(StateMachine* fsm,std::string str) {
    if (str.length() == 1){
	    write_fifo(false); 
	    return -1;
    }
    std::vector<std::string> cmd = split(str, " ");
    string cmds[3]= {"play", "pause", "stop"};
    int cmd_recv=-1;
    long startusec=0;
    for (int i = 0; i < 3; i++) {
        if (cmd[0] == cmds[i]) {
            if (i == C_PLAY) {	
	            if(fsm->getCurrentState()==S_PLAY) {
		            write_fifo(false);
		            return -1;
	            }
                fsm->data.delayTime=0;
                gettimeofday(&fsm->data.baseTime, NULL);//for delay display
	            if(fsm->getCurrentState()==S_PAUSE && cmd[1]=="0"&& cmd[2] == "-1" && cmd[4] == "0"){
                    write_fifo(true);
                    cmd_recv=C_RESUME;
		            cerr<<"[Common] RESUME\n";
                    return cmd_recv;
                } else if (cmd.size() >= 3 && cmd[cmd.size() - 2] == "-d") {
                    fsm->data.delayTime = std::stoi(cmd[cmd.size() - 1]);//*1000;//saved as us
                    if (cmd.size() > 3) {
                        startusec = std::stoi(cmd[1])*1000;
                    }
                    if (cmd.size() > 4) {
                        fsm->data.stopTime = std::stoi(cmd[2]);
                        fsm->data.stopTimeAssigned = true;
                    }
                } else {
                    if (cmd.size()>1) {
                        startusec = std::stoi(cmd[1])*1000;
		            }
                    if (cmd.size() > 2) {
                        fsm->data.stopTime = std::stoi(cmd[2]);
                        fsm->data.stopTimeAssigned = true;
                    }
                }
	            //fprintf(stderr,"[Common] startusec[%d]\n",startusec);
                fsm->data.playedTime.tv_sec = startusec / 1000000;
                fsm->data.playedTime.tv_usec = startusec % 1000000;
            }
            cmd_recv=i;
	        printf("command parsed\n");
            write_fifo(true);
            return cmd_recv;
        }
    }
    write_fifo(false); 
    return -1;
}