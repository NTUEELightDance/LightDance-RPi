#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdio.h>

#include <vector>

using namespace std;

struct LEDColor {
    int r, g, b;
    LEDColor();
    LEDColor(const int &colorCode);
    // TODO: add other utils
};

class LEDController {
   public:
    LEDController();
    int init(const vector<int> &shape);
    int sendAll(const vector<vector<int>> &statusLists);
    void fini();

   private:
    // TODO: add other utils
};

#endif
