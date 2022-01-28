/****************************************************************************
  FileName     [ definition.h ]
  PackageName  [ clientApp ]
  Synopsis     [ definitions ]
  Author       [  ]
  Copyright    [ Copyleft(c) , NTUEE, Taiwan ]
****************************************************************************/

//----------------------------------------------------------------------
//    Global
//----------------------------------------------------------------------
#ifndef _DEF_H_
#define _DEF_H_
#include <string>

#define NUM_OF_EL  22 // number of EL strips
#define NUM_OF_LED 3 // number of LED strips
#define PEOPLE_NUM 8 // number of dancer

// Number of LEDs in strip
#define LED_BRIGHTNESS_SCALE 0.1
#define LEDS_0 300
#define LEDS_1 35
#define LEDS_2 35
#define LEDS {LEDS_0, LEDS_1, LEDS_2}

// refresh interval
#define PERIOD 50 // ms
#define LED_DELAY_1 14 // delay between led_strips
#define LED_DELAY_2 4

// PCA9685 I2C address
#define NUM_EL_1       16
#define NUM_EL_2       8
#define PCA9685_ADDR_1 0x40
#define PCA9685_ADDR_2 0x60
#define EL_BRIGHTNESS_SCALE 4095*0.25
// Json file's path
const std::string DIR = "/home/pi/NTUEE_light_dance/client/clientApp/json/current/"; //"./json/";
const std::string FILENAME = "timeline.json";

// EL parts' name
const std::string ELs[NUM_OF_EL] = {
  "INNER1", "INNER2", "R_COAT2", "R_COAT1", "L_COAT2", "L_COAT1", "R_ARM2", "R_ARM1",
  "L_ARM2", "L_ARM1", "R_HAND", "L_HAND", "FACE2", "FACE1", "HAT2", "HAT1",
  "R_SHOES1", "L_SHOES1", "R_PANTS2", "R_PANTS1", "L_PANTS2", "L_PANTS1"

};

const uint16_t convertEL[11] = {0, 26, 32, 40, 64, 128, 256, 512, 1024, 2048, 4095};

// LED parts' name
const std::string LEDs[NUM_OF_LED] = {
  "LED_CHEST", "LED_R_SHOE", "LED_L_SHOE"
};

const uint16_t numLEDs[NUM_OF_LED] = { LEDS_0, LEDS_1, LEDS_2 };

#endif // _DEF_H_
