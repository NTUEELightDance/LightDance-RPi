#include "logger.h"

Logger::Logger(zmq::socket_t& _socket) : socket(_socket){};
Logger::~Logger(){};

bool Logger::success(const string& method, const string& message) {
    string buf = "[ " + method + " / Success ]\n" + message;
    zmq::message_t mes(buf.c_str(), buf.size());
    return socket.send(mes);
}

bool Logger::error(const string& method, const string& message) {
    string buf = "[ " + method + " / Error ]\n" + message;
    zmq::message_t mes(buf.c_str(), buf.size());
    return socket.send(mes);
}

bool Logger::log(const string& method, const string& message) {
    string buf = "[ " + method + " / Log ]\n" + message;
    zmq::message_t mes(buf.c_str(), buf.size());
    return socket.send(mes);
}