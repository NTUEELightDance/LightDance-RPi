#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <zmq.hpp>

using namespace std;

vector<string> splitStr(string &str);
bool Str2Unsint(const string &str, unsigned &unsint);
bool Str2LongInt(const string &str, long &);
bool send_str(zmq::socket_t&, const string&, const bool& send_multiple = true);

#endif