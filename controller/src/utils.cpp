#include "utils.h"

vector<string> splitStr(string &str) {
    vector<string> v;
    istringstream ss(str);
    string word;
    while (ss >> word) {
        v.push_back(word);
    }
    return v;
}

bool Str2Unsint(const string &str, unsigned &unsint) {
    unsint = 0;
    if (str.size() == 0)
        return false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isdigit(str[i]))
            return false;
        unsint = unsint * 10 + (str[i] - '0');
    }
    return true;
}

bool Str2LongInt(const string &str, long &longInt) {
    longInt = 0;
    if (str.size() == 0)
        return false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isdigit(str[i]))
            return false;
        longInt = longInt * 10 + (str[i] - '0');
    }
    return true;
}

// Unit: ms
long getsystime() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int rgbHexInterpolate(int hex1, int hex2, const float &rate) {
    const float B1 = hex1 % 256;
    hex1 >>= 8;
    const float G1 = hex1 % 256;
    hex1 >>= 8;
    const float R1 = hex1 % 256;

    const float B2 = hex2 % 256;
    hex2 >>= 8;
    const float G2 = hex2 % 256;
    hex2 >>= 8;
    const float R2 = hex2 % 256;

    const int Ri = (1 - rate) * R1 + rate * R2;
    const int Gi = (1 - rate) * G1 + rate * G2;
    const int Bi = (1 - rate) * B1 + rate * B2;

    return (Ri << 16) + (Gi << 8) + Gi;
}

void colorCode2RGB(int colorCode, char &R, char &G, char &B) {
    B = colorCode % 256;
    colorCode >>= 8;
    G = colorCode % 256;
    colorCode >>= 8;
    R = colorCode % 256;
}