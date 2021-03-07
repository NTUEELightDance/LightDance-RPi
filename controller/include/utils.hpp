#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

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
#endif