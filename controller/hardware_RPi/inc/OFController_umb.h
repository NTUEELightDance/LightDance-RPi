#ifndef OF_CONTROLLER
#define OF_CONTROLLER

#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

// library for PCA
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

// define
#define NUMPCA 2
const int PCAAddr[] = {0x22, 0x23};

class OFColor {
   public:
    OFColor();
    OFColor(const int &colorCode);
    int getR();
    int getG();
    int getB();

   private:
    int r, g, b;
};

class OFController {
   public:
    OFController();
    int init();
    int sendAll(const vector<int> &statusLists);

   private:
    int I2CInit();
    int fd[NUMPCA];
};

#endif
