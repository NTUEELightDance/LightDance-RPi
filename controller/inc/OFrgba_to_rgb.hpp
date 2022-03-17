#ifndef OFRGBA_TO_RGB_H
#define OFRGBA_TO_RGB_H

#include <math.h>

#include <vector>

#define gamma_r 1.6
#define gamma_g 1.8
#define gamma_b 1.66

void OFrgba2rgbiref(std::vector<char>& OF, const char& R, const char& G, const char& B, const float& alpha) {
    float a = alpha > 12 ? 1 : alpha / 12;
    OF[0] = char(pow(a / 3, gamma_r) * 255);
    OF[1] = char(pow(a / 3, gamma_g) * 255);
    OF[2] = char(pow(a / 3, gamma_b) * 255);
    OF[3] = B;
    OF[4] = G;
    OF[5] = R;
};

#endif