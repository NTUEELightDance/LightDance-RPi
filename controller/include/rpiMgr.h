#ifndef RPIMGR_H
#define RPIMGR_H
#include <string>
#include <iostream>
using namespace std;

class RPiMgr
{
public:
    RPiMgr(){};
    RPiMgr(string dancerName) : _dancerName(dancerName){};
    void load();
    void play(int, int);
    void stop();
    void statuslight();
    void list();
    void eltest();
    void ledtest();
    void quit();

private:
    string _dancerName;
};

#endif