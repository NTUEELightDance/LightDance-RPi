#include "pca9955.h"

#include <asm/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <iostream>

using namespace std;

enum PCA9955Reg {
    PCA9955_REG_PWMALL = 0x42,
    PCA9955_REF_IREFALL = 0x43,
};

union i2c_smbus_data {
    uint8_t byte;
    uint16_t word;
    uint8_t block[34];
};

struct i2c_smbus_ioctl_data {
    char rw;
    uint8_t cmd;
    int size;
    union i2c_smbus_data *data;
};

PCA9955::PCA9955(int Address) : PCA9955_Address(Address) {
    fd = wiringPiI2CSetup(PCA9955_Address);
};

int PCA9955::SetPWMAI(int channel, int *PWM, int size) {
    union i2c_smbus_data regData;

    // regData.block[0] = size;
    // for(int i=0;i<size;i++){
    // 	regData.block[i]=PWM[i];
    // }

    regData.block[0] = size - 1;
    for (int i = 1; i < size; i++) {
        regData.block[i] = PWM[i];
    }

    struct i2c_smbus_ioctl_data args;
    args.rw = 0;
    args.cmd = channel + PCA9955_PWM0_ADDR + 128;
    args.size = 5;
    args.data = &regData;

    return ioctl(fd, 0x0720, &args) && SetPWM(channel, PWM[0]);
};

int PCA9955::SetIREFAI(int channel, int *IREF, int size) {
    union i2c_smbus_data regData;

    // regData.block[0] = size;
    // for(int i=0;i<size;i++){
    // 	regData.block[i]=PWM[i];
    // }

    regData.block[0] = size - 1;
    for (int i = 1; i < size; i++) {
        regData.block[i] = IREF[i];
    }

    struct i2c_smbus_ioctl_data args;
    args.rw = 0;
    args.cmd = channel + PCA9955_IREF0_ADDR + 128;
    args.size = 5;
    args.data = &regData;

    return ioctl(fd, 0x0720, &args) && SetIREF(channel, IREF[0]);
};

int PCA9955::SetPWMIREFAI(int *data) {
    SetPWMAI(0, data, 15);
    SetIREFAI(0, &data[15], 15);

    return 0;
};

int PCA9955::SetRGB(int led_address, int Rduty, int Gduty, int Bduty, int Riref, int Giref, int Biref) {
    int *PWM, *IREF;
    PWM = new int[3];
    IREF = new int[3];
    PWM[0] = Rduty;
    PWM[1] = Gduty;
    PWM[2] = Bduty;
    IREF[0] = Riref;
    IREF[1] = Giref;
    IREF[2] = Biref;
    SetPWMAI(led_address * 3 + PCA9955_PWM0_ADDR, PWM, 3);
    SetIREFAI(led_address * 3 + PCA9955_IREF0_ADDR, IREF, 3);
    return 0;
};

void PCA9955::GetAll() {
    for (int i = 0; i < 24; i++) {
        cout << "addr : " << i << ", IREF : " << GetIREF(i) << ", PWM : " << GetPWM(i) << endl;
    }
};

int PCA9955::SetPWM(int channel, int PWM) {
    PWM = PWM < PWM_MIN ? PWM_MIN : PWM > PWM_MAX ? PWM_MAX
                                                  : PWM;
    return I2CWriteReg(channel + PCA9955_PWM0_ADDR, PWM);
};
int PCA9955::GetPWM(int channel) {
    return I2CReadReg(channel + PCA9955_PWM0_ADDR);
};
int PCA9955::SetIREF(int channel, int IREF) {
    IREF = IREF < IREF_MIN ? IREF_MIN : IREF > IREF_MAX ? IREF_MAX
                                                        : IREF;
    return I2CWriteReg(channel + PCA9955_IREF0_ADDR, IREF);
};
int PCA9955::GetIREF(int channel) {
    return I2CReadReg(channel + PCA9955_IREF0_ADDR);
};

int PCA9955::I2CWriteReg(int reg, int value) {
    return wiringPiI2CWriteReg8(fd, reg, value);
};

int PCA9955::I2CReadReg(int reg) {
    return wiringPiI2CReadReg8(fd, reg);
}
