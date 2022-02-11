#include "method.h"

// TODO: check whether multiple replies is valid

BaseMethod::BaseMethod(zmq::socket_t& socket):_socket(socket){};
void BaseMethod::exec(const vector<string>& cmd, bool& quit){
    this->method(cmd, quit);
};
void BaseMethod::method(const vector<string>& cmd, bool& quit){
    logger->error("not implementing method()");
};
BaseMethod::~BaseMethod(){};

Load::Load(zmq::socket_t& socket): BaseMethod(socket){};
void Load::method(const vector<string>& cmd, bool& quit){
    if (cmd.size() > 1)
        rpiMgr->load(cmd[1]);
    else
        rpiMgr->load();
    logger->success("load success");
};

Play::Play(zmq::socket_t& socket): BaseMethod(socket){};
void Play::method(const vector<string>& cmd, bool& quit){
    if (cmd.size() == 1)
        rpiMgr->play(false, 0);
    else if (cmd.size() == 2){
        unsigned startTime;
        if (!Str2Unsint(cmd[1], startTime)){
            char buf[128];
            sprintf("illegal option \"%s\"", cmd[1].c_str());
            logger->error(buf);
            return;
        }
        else
            rpiMgr->play(true, startTime);
    }
    else if (cmd.size() == 3){
        unsigned startTime, delayTime;
        const bool option1 = Str2Unsint(cmd[1], startTime), option2 = Str2Unsint(cmd[2], delayTime);
        if (!option1){
            char buf[128];
            sprintf("illegal option \"%s\"", cmd[1].c_str());
            logger->error(buf);
        }
        if (!option2){
            char buf[128];
            sprintf("Illegal option \"%s\"", cmd[2].c_str());
            logger->error(buf);
        }
        if (option1 && option2)
            // TODO: add thread
            rpiMgr->play(true, startTime, delayTime);
        else
            return;
    }
    // logger->success("play success");
};

Stop::Stop(zmq::socket_t& socket): BaseMethod(socket){};
void Stop::method(const vector<string>& cmd, bool& quit){
    rpiMgr->stop();
    logger->success("stop success");
};

StatusLight::StatusLight(zmq::socket_t& socket): BaseMethod(socket){};
void StatusLight::method(const vector<string>& cmd, bool& quit){
    rpiMgr->statuslight();
    logger->success("statuslight success");
};

Eltest::Eltest(zmq::socket_t& socket): BaseMethod(socket){};
void Eltest::method(const vector<string>& cmd, bool& quit){
    // default all light
    if (cmd.size() == 1)
        rpiMgr->eltest(-1, 4095);
    // id/brightness
    else if (cmd.size() >= 3){
        int id = stoi(cmd[1]);
        char buf[128];
        sprintf(buf, "Testing id: %d", id);
        // logger->log(buf);

        unsigned brightness;
        if (!Str2Unsint(cmd[2], brightness)){
            char buf[128];
            sprintf(buf, "Illegal option \"%s\"", cmd[2].c_str());
            logger->error(buf);
            return;
        }
        else
            rpiMgr->eltest((int)id, brightness);
    }
    // error
    else{
        logger->error("Missing options, should give id(0 ~ 31) and brightness(0 ~ 4095)");
        return;
    }
    logger->success("eltest success");
};

Ledtest::Ledtest(zmq::socket_t& socket): BaseMethod(socket){};
void Ledtest::method(const vector<string>& cmd, bool& quit){
    rpiMgr->ledtest();  // TODO: need to get cmd arguments
    logger->success("ledtest success");
};

List::List(zmq::socket_t& socket): BaseMethod(socket){};
void List::method(const vector<string>& cmd, bool& quit){
    rpiMgr->list();
    logger->success("list success");
};

Quit::Quit(zmq::socket_t& socket): BaseMethod(socket){};
void Quit::method(const vector<string>& cmd, bool& quit){
    rpiMgr->quit();
    quit = true;
    logger->success("quit success");
};

Pause::Pause(zmq::socket_t& socket): BaseMethod(socket){};
void Pause::method(const vector<string>& cmd, bool& quit){
    rpiMgr->pause();
    logger->success("pause success");
};