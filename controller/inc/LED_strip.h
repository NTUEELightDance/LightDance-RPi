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
#include <bcm2835.h>

#define DATA_OFFSET   1     // real data after start bytes
#define START_BYTE    0xFF
#define STOP_BYTE_0   0x55  // stop signal 0
#define STOP_BYTE_1   0xFF  // stop signal 1

#define SPI_CLOCK_DIV BCM2835_SPI_CLOCK_DIVIDER_256 // div by 256 = ~1MHz 

class LED_Strip
{
  public:
    LED_Strip();
    LED_Strip(const uint8_t &,const uint16_t *);
    ~LED_Strip();

    void sendToStrip(const uint8_t &, uint8_t*);
    void getSeq(const uint8_t &, const uint16_t &, char *, uint8_t *);
  
  private:
    uint8_t _nStrips;
    uint16_t *_nLEDs;
};

#endif // _LED_STRIP_H_
