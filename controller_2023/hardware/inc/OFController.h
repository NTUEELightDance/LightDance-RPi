#ifndef OF_CONTROLLER_H
#define OF_CONTROLLER_H

#include <stdio.h>

#include <vector>

using namespace std;

struct OFColor {
    int r, g, b;
    OFColor();
    OFColor(const int &colorCode);
};

class OFController {
   public:
    OFController();
    int init();
    int sendAll(const vector<int> &statusLists);

   private:
};

#endif
