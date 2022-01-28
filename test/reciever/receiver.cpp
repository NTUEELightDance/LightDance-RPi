#include <cstring>
#include <iostream>
#include <string>
#include <zmq.hpp>

int main() {
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:8000");

    char buf[100] = {0};
    while (std::string(buf).compare("done")) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv(&request);
        std::memcpy(buf, request.data(), 100);
        std::string message = request.to_string();
        std::cout << "Received: " << message << std::endl;

        //  Send reply back to client
        std::string reponse;
        int r = rand();
        if (r % 3 == 0) {
            reponse = "Success:" + message;
        } else if (r % 3 == 1) {
            reponse = "Error:" + message;
        } else {
            reponse = "Warning:" + message;
        }
        zmq::message_t reply(reponse.size());
        std::memcpy(reply.data(), reponse.data(), reponse.size());
        socket.send(reply);
    }
    return 0;
}