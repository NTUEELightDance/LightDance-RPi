/****************************************************************************
  FileName     [ LED_strip.h ]
  PackageName  [ clientApp ]
  Synopsis     [ LED strip control ]
  Author       [  ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/

#ifndef _LED_STRIP_H_
#define _LED_STRIP_H_
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <errno.h>

#define DATA_OFFSET   0     // real data after start bytes
#define START_BYTE    0xFF
#define STOP_BYTE_0   0x55  // stop signal 0
#define STOP_BYTE_1   0xFF  // stop signal 1

#define SPI_CLOCK_DIV BCM2835_SPI_CLOCK_DIVIDER_256 // div by 256 = ~1MHz 

#define MAX_CHANNEL_COUNT 16

class LED_Strip
{
  public:
    LED_Strip();
    void initialize(std::vector<uint16_t>&);
    ~LED_Strip(){};
    void StmInit();
    void sendToStrip(std::vector< std::vector<char> >&);
  private:
    bool initialized;
    bool StmAlive;
    int _serialPort;
    std::vector<uint16_t> _nLEDs;
};

#endif // _LED_STRIP_H_
