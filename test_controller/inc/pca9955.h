/**
 * @file pca9955.h
 *
 */
/* Copyright (C) 2022 by NTUEE, Taiwan
 *
 */

#ifndef PCA9955_H
#define PCA9955_H

#include <cstdint>

#define PCA9955_I2C_ADDRESS_DEFAULT 0x00
#define PCA9955_IREF_DEFAULT 255
#define PCA9955_PWM_DEFAULT 0

#define ADDRESS(x)   (static_cast<int>(x))
#define VALUE(x)     (static_cast<int>(x))
#define CHANNEL(x)   (static_cast<int>(x))

#define PCA9955_IREF0_ADDR 0x22
#define PCA9955_PWM0_ADDR 0x0a

#define IREF_MAX 255
#define IREF_MIN 0
#define PWM_MAX  255
#define PWM_MIN  0

#define PCA9955_CHANNELS 15

class PCA9955{
    public:

	PCA9955(int Address);
    PCA9955() {};
	~PCA9955() {};

	int SetPWMAI(int channel, int *PWM, int size);
	int SetIREFAI(int channel, int *IREF, int size);
	int SetPWMIREFAI(int *data);

	int SetRGB(int led_address, int Rduty, int Gduty, int Bduty, int Riref, int Giref, int Biref);

    void GetAll();

    int Getfd(){return fd;};

    private:

	int SetPWM(int channel, int PWM);
	int GetPWM(int channel);

    private:

	int SetIREF(int channel, int IREF);
	int GetIREF(int channel);
	
    private:
	
	int I2CWriteReg(int reg, int value);
	int I2CReadReg(int reg);

    private:

    int PCA9955_Address;
	int fd;

};

#endif
