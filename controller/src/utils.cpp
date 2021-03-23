
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/time.h>

#include "definition.h"
using namespace std;

extern const uint16_t convertEL[11];

// alpha: 0, 0.1, 0.2 ... 1
uint16_t getELBright(double alpha)
{
    return convertEL[int(alpha * 10)];
}

vector<string> splitStr(string &str)
{
    vector<string> v;
    istringstream ss(str);
    string word;
    while (ss >> word)
    {
        v.push_back(word);
    }
    return v;
}

bool Str2Unsint(const string &str, unsigned &unsint)
{
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

bool Str2LongInt(const string &str, long &longInt)
{
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
// TODO
long getsystime() // ms
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    //return tv.tv_usec/1000;
}
