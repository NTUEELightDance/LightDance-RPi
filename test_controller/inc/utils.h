#ifndef UTILS_H
#define UTILS_H

#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "definition.h"

using namespace std;

vector<string> splitStr(string &str);
bool Str2Unsint(const string &str, unsigned &unsint);
bool Str2LongInt(const string &str, long &);
long getsystime();  // Uint: ms
int rgbHexInterpolate(int hex1, int hex2, const float &rate);
void colorCode2RGB(int colorCode, char &R, char &G, char &B);

#endif