#ifndef LOGGER_H
#define LOGGER_H

#include <string.h>
#include <zmq.hpp>

using namespace std;

// Class for response to CLI/socket
class Logger {
public:
    Logger(zmq::socket_t& _socket);
    ~Logger();
    // log for success executing method
    bool success(const string& method, const string& message = "");
    // log for fail executing method
    bool error(const string& method, const string& message = "");
    // log for normal message doesn't belong to success and error
    bool log(const string& method, const string& message = "");
private:
    zmq::socket_t& socket;
};

#endif