#include "../inc/utils.h"

vector<string> splitStr(string &str){
    vector<string> v;
    istringstream ss(str);
    string word;
    while (ss >> word)
    {
        v.push_back(word);
    }
    return v;
}

bool Str2Unsint(const string &str, unsigned &unsint){
    unsint = 0;
    if (str.size() == 0)
        return false;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (!isdigit(str[i]))
            return false;
        unsint = unsint * 10 + (str[i] - '0');
    }
    return true;
}

bool Str2LongInt(const string &str, long &longInt){
    longInt = 0;
    if (str.size() == 0)
        return false;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (!isdigit(str[i]))
            return false;
        longInt = longInt * 10 + (str[i] - '0');
    }
    return true;
}

bool send_str(zmq::socket_t& socket, const string& msg){
    zmq::message_t response(msg.c_str(), msg.size());
    return socket.send(response);
}