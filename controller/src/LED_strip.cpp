/****************************************************************************
  FileName     [ LED_strip.cpp ]
  PackageName  [ clientApp ]
  Synopsis     [ LED strip control ]
  Author       [  ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/
#include "LED_strip.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <iostream>
#include <typeinfo>
#include <vector>

using namespace std;

/*
  @brief constructor of LED strip, initialize UART communication.
  @param nLEDs length of each LED strips
*/
LED_Strip::LED_Strip() {
    initialized = false;
}
void LED_Strip::initialize(vector<uint16_t>& nLEDs) {
    // deepcopy
    if (nLEDs.size() > MAX_CHANNEL_COUNT) {
        fprintf(stderr, "Size of LED strips exceed maximum channel count: %d\n", nLEDs.size());
        exit(-1);
    }
    _nLEDs.resize(nLEDs.size());
    for (int i = 0; i < nLEDs.size(); ++i) {
        _nLEDs[i] = nLEDs[i];
    }
    _nLEDs.resize(MAX_CHANNEL_COUNT, 0);

    // uart
    if ((_serialPort = serialOpen("/dev/ttyS0", 1000000)) < 0) /* open serial port */
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        exit(-1);
    }
    if (wiringPiSetup() == -1) /* initializes wiringPi setup */
    {
        fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
        exit(-1);
    }

    StmInit();
    initialized = true;
}
void LED_Strip::StmInit() {
    pinMode(0, OUTPUT);  // reset pin, actually GPIO 17
    digitalWrite(0, 0);
    usleep(100 * 1000);  // pulse
    digitalWrite(0, 1);
    sleep(2);           // recover
    pinMode(0, INPUT);  // cheat

    // initializing
    for (int i = 0; i < 3; ++i) {
        serialPutchar(_serialPort, (char)255);
        for (uint8_t j = 0; j < _nLEDs.size(); ++j) {
            serialPutchar(_serialPort, _nLEDs[j]);
            // cout << _nLEDs[j] << " ";
        }
        // cout << endl;
        serialPutchar(_serialPort, (char)255);
        usleep(50 * 1000);
    }
    usleep(100 * 1000);

    // how to know?
    StmAlive = true;
}

/*!
  @brief Send pixel secquence to LED strip via UART.
  @param color pixel secquence to send
*/
void LED_Strip::sendToStrip(std::vector<std::vector<char> >& color) {
    if (!StmAlive)
        return;

    for (int i = 0; i < color.size(); ++i) {
        if (color[i].size() != _nLEDs[i] * 3) {
            fprintf(stdout, "Different input color size and initialized LED strip size: #%d, %d vs. %d\n", i, color[i].size(), _nLEDs[i]);
            continue;
        }
        for (int j = 0; j < color[i].size(); ++j) {
            serialPutchar(_serialPort, color[i][j]);
            // cout << (int)color[i][j] << " ";
        }
    }
    // cout << endl;

    // how to know?
    if (false) {
        StmAlive = false;
        // open new thread?
    }
}
