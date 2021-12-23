#ifndef METHOD_H
#define METHOD_H

#include <vector>
#include <zmq.hpp>
#include <thread>
#include "rpiMgr.h"
#include "utils.h"

using namespace std;

extern RPiMgr *rpiMgr;

// BaseMethod for other method to inherit
// You must implement the method() function in child classes
class BaseMethod {
public:
    BaseMethod(zmq::socket_t& socket);
    virtual ~BaseMethod();
    void exec(const vector<string>& cmd, bool& quit);
protected:
    virtual void method(const vector<string>& cmd, bool& quit);

    zmq::socket_t& _socket;
};

class Load: public BaseMethod {
public:
    Load(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class Play: public BaseMethod {
public:
    Play(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class Stop: public BaseMethod {
public:
    Stop(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class StatusLight: public BaseMethod {
public:
    StatusLight(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class Eltest: public BaseMethod {
public:
    Eltest(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class Ledtest: public BaseMethod {
public:
    Ledtest(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class List: public BaseMethod {
public:
    List(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class Quit: public BaseMethod {
public:
    Quit(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

class Pause: public BaseMethod {
public:
    Pause(zmq::socket_t& socket);
protected:
    void method(const vector<string>& cmd, bool& quit);
};

#endif