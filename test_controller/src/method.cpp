#include "method.h"

BaseMethod::BaseMethod(zmq::socket_t& socket):_socket(socket){};
void BaseMethod::exec(const vector<string>& cmd, bool& quit){
    thread t(&BaseMethod::method, this, ref(cmd), ref(quit));
    // Seperate the execution
    t.detach();
};
void BaseMethod::method(const vector<string>& cmd, bool& quit){
    string err = "Error: not implementing method()\n";
    send_str(_socket, err);
};
BaseMethod::~BaseMethod(){};

Load::Load(zmq::socket_t& socket): BaseMethod(socket){};
void Load::method(const vector<string>& cmd, bool& quit){
    if (cmd.size() > 1)
        rpiMgr->load(cmd[1]);
    else
        rpiMgr->load();
};

Play::Play(zmq::socket_t& socket): BaseMethod(socket){};
void Play::method(const vector<string>& cmd, bool& quit){
    if (cmd.size() == 1)
        rpiMgr->play(false, 0);
    else if (cmd.size() == 2){
        unsigned startTime;
        if (!Str2Unsint(cmd[1], startTime)){
            string err = "Error: illegal option \"" + cmd[1] +"\"\n";
            _socket.send(err.c_str(), err.size());
        }
        else
            rpiMgr->play(true, startTime);
    }
    else if (cmd.size() == 3){
        unsigned startTime, delayTime;
        const bool option1 = Str2Unsint(cmd[1], startTime), option2 = Str2Unsint(cmd[2], delayTime);
        if (!option1){
            string err = "Error: illegal option \"" + cmd[1] + "\"\n";
            send_str(_socket, err);
        }
        if (!option2){
            string err = "Error: illegal option \"" + cmd[2] + "\"\n";
            send_str(_socket, err);
        }
        if (option1 && option2)
            rpiMgr->play(true, startTime, delayTime);
    }
};

Stop::Stop(zmq::socket_t& socket): BaseMethod(socket){};
void Stop::method(const vector<string>& cmd, bool& quit){
    rpiMgr->stop();
};

StatusLight::StatusLight(zmq::socket_t& socket): BaseMethod(socket){};
void StatusLight::method(const vector<string>& cmd, bool& quit){
    rpiMgr->statuslight();
};

Eltest::Eltest(zmq::socket_t& socket): BaseMethod(socket){};
void Eltest::method(const vector<string>& cmd, bool& quit){
    // default all light
    if (cmd.size() == 1)
        rpiMgr->eltest(-1, 4095);
    // id/brightness
    else if (cmd.size() >= 3){
        int id = stoi(cmd[1]);
        string msg = "testing id: " + id;
        send_str(_socket, msg);

        unsigned brightness;
        if (!Str2Unsint(cmd[2], brightness)){
            string err = "Error: illegal option \"" + cmd[2] + "\"\n";
            send_str(_socket, msg);
        }
        else
            rpiMgr->eltest((int)id, brightness);
    }
    // error
    else{
        string err = "Error: missing options, should give id(0 ~ 31) and brightness(0 ~ 4095)\n";
        send_str(_socket, err);
    }
};

Ledtest::Ledtest(zmq::socket_t& socket): BaseMethod(socket){};
void Ledtest::method(const vector<string>& cmd, bool& quit){
    rpiMgr->ledtest();  // TODO: need to get cmd arguments
};

List::List(zmq::socket_t& socket): BaseMethod(socket){};
void List::method(const vector<string>& cmd, bool& quit){
    rpiMgr->list();
};

Quit::Quit(zmq::socket_t& socket): BaseMethod(socket){};
void Quit::method(const vector<string>& cmd, bool& quit){
    rpiMgr->quit();
    quit = true;
};

Pause::Pause(zmq::socket_t& socket): BaseMethod(socket){};
void Pause::method(const vector<string>& cmd, bool& quit){
    cout << "pause success" << endl;
    rpiMgr->pause();
};