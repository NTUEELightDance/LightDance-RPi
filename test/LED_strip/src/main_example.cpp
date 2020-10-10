#include <iostream>
#include <time.h>
#include "LED_strip.h"
using namespace std;

double wait = 0.005;
const uint8_t nStrips = 1;
const uint16_t nLeds[1] = {5};
uint8_t **buf = NULL;

LED_Strip strips(nStrips, nLeds);
int main(){
	int num = 0;
	buf = new uint8_t*[nStrips];
	for(int i = 0; i < nStrips; i++){
		buf[i] = new uint8_t[nLeds[i] * 3];
		for(int j = 0; j < nLeds[i] * 3; j++){
			buf[i][j] = 0;
		}
		strips.sendToStrip(i, buf[i]);
	}
	while (1) {


	sleep(1);
	for (int i = 0; i < nStrips; ++i) { for (int j = 0; j < nLeds[i] * 3; ++j) buf[i][j] = 255; strips.sendToStrip(i, buf[i]);}
	sleep(1);
	for (int i = 0; i < nStrips; ++i) { for (int j = 0; j < nLeds[i] * 3; ++j) buf[i][j] = 0; strips.sendToStrip(i, buf[i]);}
	}

}
