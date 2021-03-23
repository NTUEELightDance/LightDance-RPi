#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "definition.h"
using namespace std;

extern const uint16_t convertEL[11];

// alpha: 0, 0.1, 0.2 ... 1
uint16_t getELBright(double alpha);

vector<string> splitStr(string &str);

bool Str2Unsint(const string &str, unsigned &unsint);
bool Str2LongInt(const string &str, long &);
long getsystime();

#endif
