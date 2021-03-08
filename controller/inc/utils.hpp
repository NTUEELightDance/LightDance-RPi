#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

vector<string> splitStr(string &str) {
    vector<string> v;
    istringstream ss(str);
    string word;
    while (ss >> word)
    {
        v.push_back(word);
    }
    return v;
}

bool Str2Unsint(const string& str, unsigned& unsint) {
    unsint = 0;
    if (str.size() == 0)
        return false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isdigit(str[i]))
            return false;
        unsint = unsint*10 + (str[i]-'0');
    }
    return true;
}
#endif
