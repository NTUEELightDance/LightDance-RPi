#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include <string>

// library for WS2812
#include "clk.h"
#include "dma.h"
#include "gpio.h"
#include "pwm.h"
#include "ws2811.h"

// parameters for WS2812
#define TARGET_FREQ WS2811_TARGET_FREQ
#define GPIO_PIN_1 18
#define GPIO_PIN_2 12
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GRB

class LEDColor {
   public:
    LEDColor();
    LEDColor(const int &colorCode);
    uint32_t getRGB();

   private:
    uint32_t rgb;
    int r;
    int g;
    int b;
};

class LEDController {
   public:
    LEDController();

    int init(const std::vector<int> &shape);

    int sendAll(const std::vector<std::vector<int>> &statusLists);
    // send all data to RPI and push them to LED strips.
    // return value:
    // 0: OK
    // -1: The data size of statausLists is not consist with number of strips
    // initialized

    void fini();

   private:
    ws2811_t ledString[2] = {
        {
            .freq = TARGET_FREQ,
            .dmanum = DMA,
            .channel =
                {
                    [0] =
                        {
			    .gpionum = GPIO_PIN_1,
			    .invert = 0,
			    .count = 100,
			    .strip_type = STRIP_TYPE,
			    .brightness = 255,
			},               
                },
        },
        {
            .freq = TARGET_FREQ,
            .dmanum = DMA,
            .channel =
                {
                    [0] =
                        {
                            .gpionum = GPIO_PIN_2,
                            .invert = 0,
                            .count = 100,  // just for init
                            .strip_type = STRIP_TYPE,
                            .brightness = 255,
                        },
                },
        }
    };
    std::vector<int> stripShape;
    int play(const std::vector<std::vector<int>> &statusLists);
};

#endif
