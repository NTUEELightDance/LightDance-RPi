#include "../inc/logger.h"

Logger::Logger(zmq::socket_t& _socket): socket(_socket){};
Logger::~Logger(){};

bool Logger::success(const char* message){
    char buf[128];
    sprintf(buf, "Success: %s\n", message);
    zmq::message_t mes(buf, strlen(buf));
    return socket.send(mes);    
}

bool Logger::error(const char* message){
    char buf[128];
    sprintf(buf, "Error: %s\n", message);
    zmq::message_t mes(buf, strlen(buf));
    return socket.send(mes);
}

bool Logger::log(const char* message){
    char buf[128];
    sprintf(buf, "%s\n", message);
    zmq::message_t mes(buf, strlen(buf));
    return socket.send(mes);
}