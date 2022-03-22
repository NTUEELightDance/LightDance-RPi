#include "pca995X.h"

#include <asm/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <iostream>

#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "pcaDefinition.h"

#define I2C_SMBUS_BLOCK_DATA 5  // SMBus-level access
#define I2C_SMBUS 0x0720

using namespace std;

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
PCA995X::PCA995X(int Address, int pca_type, int iref0Reg, int pwm0Reg, int ledChannelNum) : PCA995X_Address(Address), type(pca_type), iref0Reg(iref0Reg), pwm0Reg(pwm0Reg), ledChannelNum(ledChannelNum) {
    fd = wiringPiI2CSetup(PCA995X_Address);
};
int PCA995X::SetPWMAI(int channel, int *PWM, int size) {
    if (!CheckChannelLegal(channel)) return false;

    union i2c_smbus_data regData;

    regData.block[0] = size - 1;
    for (int i = 1; i < size; i++) {
        regData.block[i] = PWM[i];
    }

    struct i2c_smbus_ioctl_data args;
    args.rw = 0;
    args.cmd = channel + pwm0Reg + AUTO_INCREMENT;
    args.size = I2C_SMBUS_BLOCK_DATA;
    args.data = &regData;

    return !(!ioctl(fd, I2C_SMBUS, &args) && !SetPWM(channel, PWM[0]));
};
int PCA995X::SetIREFAI(int channel, int *IREF, int size) {
    if (!CheckChannelLegal(channel)) return false;

    union i2c_smbus_data regData;

    regData.block[0] = size - 1;
    for (int i = 1; i < size; i++) {
        regData.block[i] = IREF[i];
    }

    struct i2c_smbus_ioctl_data args;
    args.rw = 0;
    args.cmd = channel + iref0Reg + AUTO_INCREMENT;
    args.size = I2C_SMBUS_BLOCK_DATA;
    args.data = &regData;

    return !(!ioctl(fd, I2C_SMBUS, &args) && !SetIREF(channel, IREF[0]));
};
int PCA995X::SetPWMIREFAI(int *data) {
    SetPWMAI(0, data, GetLedChannelNum() * NUM_AN_OF_NEED_PWM);
    SetIREFAI(0, &data[GetLedChannelNum() * NUM_AN_OF_NEED_PWM], GetLedChannelNum() * NUM_AN_OF_NEED_IREF);

    return 0;
};
int PCA995X::SetRGB(int led_address, int Rduty, int Gduty, int Bduty, int Riref, int Giref, int Biref) {
    int *PWM, *IREF;
    PWM = new int[3];
    IREF = new int[3];
    PWM[0] = Rduty;
    PWM[1] = Gduty;
    PWM[2] = Bduty;
    IREF[0] = Riref;
    IREF[1] = Giref;
    IREF[2] = Biref;
    int temp01 = SetPWMAI(led_address * NUM_AN_OF_NEED_PWM, PWM, NUM_AN_OF_NEED_PWM);
    int temp02 = SetIREFAI(led_address * NUM_AN_OF_NEED_IREF, IREF, NUM_AN_OF_NEED_IREF);
    return !(!temp01 && !temp02);
};
void PCA995X::GetAll() {
    for (int i = 0; i < GetLedChannelNum() * 3; i++) {
        cout << "addr : " << i << ", IREF : " << GetIREF(i) << ", PWM : " << GetPWM(i) << endl;
    }
};
int PCA995X::SetPWM(int channel, int PWM) {
    if (!CheckChannelLegal(channel)) return false;
    PWM = PWM < PWM_MIN ? PWM_MIN : PWM > PWM_MAX ? PWM_MAX
                                                  : PWM;
    return I2CWriteReg(channel + pwm0Reg, PWM);
};
int PCA995X::GetPWM(int channel) {
    if (!CheckChannelLegal(channel)) return false;
    return I2CReadReg(channel + pwm0Reg);
};
int PCA995X::SetIREF(int channel, int IREF) {
    if (!CheckChannelLegal(channel)) return false;
    IREF = IREF < IREF_MIN ? IREF_MIN : IREF > IREF_MAX ? IREF_MAX
                                                        : IREF;
    return I2CWriteReg(channel + iref0Reg, IREF);
};
int PCA995X::GetIREF(int channel) {
    if (!CheckChannelLegal(channel)) return false;
    return I2CReadReg(channel + iref0Reg);
};
int PCA995X::I2CWriteReg(int reg, int value) {
    return wiringPiI2CWriteReg8(fd, reg, value);
};
int PCA995X::I2CReadReg(int reg) {
    return wiringPiI2CReadReg8(fd, reg);
};
bool PCA995X::CheckChannelLegal(int channel) {
    return (channel >= 0 && channel < ledChannelNum * 3);
};
int PCA995X::GetLedChannelNum() {
    return ledChannelNum;
};
