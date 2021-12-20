#ifndef METHOD_H
#define METHOD_H

#include <vector>
#include <zmq.hpp>
#include "rpiMgr.h"
#include "utils.h"

using namespace std;

extern RPiMgr *rpiMgr;

// BaseMethod for other method to inherit
// You must implement the exec() function in child classes
class BaseMethod {
public:
    BaseMethod(zmq::socket_t& socket):_socket(socket){};
    virtual void exec(const vector<string>& cmd, bool& quit){
        string err = "Error: not implementing exec()\n";
        send_str(_socket, err);
    };
protected:
    zmq::socket_t& _socket;
};

class Load: public BaseMethod {
public:
    Load(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
        if (cmd.size() > 1)
            rpiMgr->load(cmd[1]);
        else
            rpiMgr->load();
    };
};

class Play: public BaseMethod {
public:
    Play(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
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
};

class Stop: public BaseMethod {
public:
    Stop(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
        rpiMgr->stop();
    }
};

class StatusLight: public BaseMethod {
public:
    StatusLight(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
        rpiMgr->statuslight();
    }
};

class Eltest: public BaseMethod {
public:
    Eltest(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
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
    }
};

class Ledtest: public BaseMethod {
public:
    Ledtest(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
        rpiMgr->ledtest();  // TODO: need to get cmd arguments
    }
};

class List: public BaseMethod {
public:
    List(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
        rpiMgr->list();
    }
};

class Quit: public BaseMethod {
public:
    Quit(zmq::socket_t& socket): BaseMethod(socket){};
    void exec(const vector<string>& cmd, bool& quit){
        rpiMgr->quit();
        quit = true;
    }
};

#endif