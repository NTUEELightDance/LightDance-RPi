#include "method.h"

// TODO: check whether multiple replies is valid

BaseMethod::BaseMethod(zmq::socket_t& socket) : _socket(socket){};
void BaseMethod::exec(const vector<string>& cmd, bool& quit) {
    this->method(cmd, quit);
};
void BaseMethod::method(const vector<string>& cmd, bool& quit) {
    logger->error("Method", "not implementing method()");
};
BaseMethod::~BaseMethod(){};

Load::Load(zmq::socket_t& socket) : BaseMethod(socket){};
void Load::method(const vector<string>& cmd, bool& quit) {
    if (cmd.size() > 1)
        rpiMgr->load(cmd[1]);
    else
        rpiMgr->load();
};

Play::Play(zmq::socket_t& socket) : BaseMethod(socket){};
void Play::method(const vector<string>& cmd, bool& quit) {
    if (cmd.size() == 1)
        rpiMgr->play(false, 0);
    else if (cmd.size() == 2) {
        unsigned startTime;
        if (!Str2Unsint(cmd[1], startTime)) {
            logger->error("Play", "Illegal option \"" + cmd[1] + "\"");
            return;
        } else
            rpiMgr->play(true, startTime);
    } else if (cmd.size() == 3) {
        unsigned startTime, delayTime;
        const bool option1 = Str2Unsint(cmd[1], startTime), option2 = Str2Unsint(cmd[2], delayTime);
        if (!option1)
            logger->error("Play", "Illegal option \"" + cmd[1] + "\"");
        if (!option2)
            logger->error("Play", "Illegal option \"" + cmd[2] + "\"");
        if (option1 && option2)
            rpiMgr->play(true, startTime, delayTime);
        else
            return;
    }
};

Stop::Stop(zmq::socket_t& socket) : BaseMethod(socket){};
void Stop::method(const vector<string>& cmd, bool& quit) {
    rpiMgr->stop();
    logger->success("Stop");
};

StatusLight::StatusLight(zmq::socket_t& socket) : BaseMethod(socket){};
void StatusLight::method(const vector<string>& cmd, bool& quit) {
    rpiMgr->statuslight();
};

OFtest::OFtest(zmq::socket_t& socket) : BaseMethod(socket){};
void OFtest::method(const vector<string>& cmd, bool& quit) {
    // rpiMgr->OFtest();
    logger->success("Eltest");
};

LEDtest::LEDtest(zmq::socket_t& socket) : BaseMethod(socket){};
void LEDtest::method(const vector<string>& cmd, bool& quit) {
    // rpiMgr->LEDtest();
    logger->success("Ledtest");
};

List::List(zmq::socket_t& socket) : BaseMethod(socket){};
void List::method(const vector<string>& cmd, bool& quit) {
    rpiMgr->list();
};

Quit::Quit(zmq::socket_t& socket) : BaseMethod(socket){};
void Quit::method(const vector<string>& cmd, bool& quit) {
    rpiMgr->quit();
    quit = true;
    logger->success("Quit");
};

Pause::Pause(zmq::socket_t& socket) : BaseMethod(socket){};
void Pause::method(const vector<string>& cmd, bool& quit) {
    rpiMgr->pause();
    logger->success("Pause");
};