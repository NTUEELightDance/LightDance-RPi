#ifndef COMMAND_H
#define COMMAND_H

#include<bits/stdc++.h>
using namespace std;

struct Status {
	int r;
	int g;
	int b;
	int a;
    Status(){};
    Status(int r, int g, int b, int a):r(r), g(g), b(b), a(a){};
};

class OpticalFiberController {
	public:
		OpticalFiberController();
		// int init(vector<int> map);
		int sendAll(vector<Status> status); // lightall, darkall
};

class LEDStripController {
	public:
		LEDStripController();
		// int init(vector<int> shape);
		int checkReady();
		int sendAll(vector<vector<Status>> status);
};

#define LED_SIZE 3
#define OF_SIZE 5

class Command {
    public:
        Command();
        // void load(string _dancerName);
        // setDancer??
        void OFtest(const int& channel, Status status);
        void LEDtest(const int& channel, Status status);

        OpticalFiberController OPT_CTRL;
        LEDStripController LED_CTRL;

        map<string, int> OFPARTS; // name -> id
        map<string, pair<int, int> > LEDPARTS; // name -> id, len

};

#endif