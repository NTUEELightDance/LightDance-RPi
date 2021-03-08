/****************************************************************************
  FileName     [ EL.h ]
  PackageName  [ clientApp ]
  Synopsis     [ Electroluminescent Wire control ]
  Author       [  ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/

#ifndef _EL_H_
#define _EL_H_

#include <cstdlib>
#include <bcm2835.h>
#include "pca9685.h"

class EL
{
  public:
    EL(const uint8_t nEL, const uint8_t addr = 0x40) : _nEL(nEL) {

      if (bcm2835_init() != 1) {
        fprintf(stderr, "bcm2835_init() failed\n");
        exit(-1);
      }

      pca = new PCA9685(addr);
      
      _dutyCycle = new uint16_t[nEL];
    };

    ~EL(){
      delete pca;
      delete _dutyCycle;
    };

    // set an EL with given ID and duty cycle
    void setEL(const uint8_t &id, const uint16_t &dt) {
        _dutyCycle[id] = dt;
        pca->Write(CHANNEL(id), VALUE(dt));
    };

    uint16_t getDutyCycle(const uint8_t &id) { 
      uint16_t pOn, pOff;
      pca->Read(CHANNEL(id), &pOn, &pOff);
      
      return (pOff - pOn) / 4096 * 100;
    };

    // turn off all the ELs
    void offAll() {
        for (uint8_t i = 0; i < _nEL; ++i)
            pca->Write(CHANNEL(i), VALUE(0));
    };

    // turn on all the ELs
    void onAll() { 
        for (uint8_t i = 0; i < _nEL; ++i)
            pca->Write(CHANNEL(i), VALUE(4095));
    };

  private:
    uint8_t _nEL; // number of ELs
    uint16_t *_dutyCycle; // range 0~4095
    PCA9685 *pca;
};

#endif // _EL_H_
