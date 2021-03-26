#ifndef RPIMGR_H
#define RPIMGR_H
#include <string>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "LED_strip.h"

using namespace std;
using json = nlohmann::json;

class RPiMgr
{
public:
    RPiMgr(){};
    RPiMgr(string dancerName) : _dancerName(dancerName){
    	//cerr << "jizz" << endl;	    
	ifstream infile("./asset/LED.json", ios::in);
	if (!infile)
		cerr << "Error:cannot open LED.json" << endl;
	else {
		infile >> LEDJson;
		for (json::iterator it = LEDJson.begin(); it != LEDJson.end(); ++it) {
			for (json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
				json j;
				//*
				for (int i = 0; i < it2.value().size(); ++i) {
					uint8_t B = ((unsigned long)it2.value()[i]) % 256;
					uint8_t G = (((unsigned long)it2.value()[i]) >> 8) % 256;
					uint8_t R = (((unsigned long)it2.value()[i]) >> 16) % 256;
					j.push_back(R);
					j.push_back(G);
					j.push_back(B);
				}
				it2.value() = j;
				//*/
				//cout << it2.value() << endl;
			}
		}
	}
	infile.close();
    	
    };
    bool setDancer();
    void load(const string& path="./data/control.json");
    void play(bool givenSTartTime, unsigned start, unsigned delay = 0);
    void stop();
    void statuslight();
    void list();
    void eltest(int id, unsigned brightness);
    void ledtest();
    void quit();
    void pause() { _playing = false; }

private:
    string _dancerName;
    json _ctrlJson;
    json _ELparts;
    json _LEDparts;
    size_t _startTime;
    bool _loaded;
    bool _playing;
    json LEDJson;
    LED_Strip* led_strip;

    //function
    size_t getFrameId() const;
    json getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& secondStatus) const;
    void lightOneStatus(const json& status) const;
};

#endif
