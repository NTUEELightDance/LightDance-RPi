#ifndef OFRGBA_TO_RGB_H
#define OFRGBA_TO_RGB_H

#include <math.h>

#include <vector>

#define gamma_r 1.6
#define gamma_g 1.8
#define gamma_b 1.66

void OFrgba2rgbiref(std::vector<char>& OF, const char& R, const char& G, const char& B, const float& alpha) {
    const float exp = 2.71828;
    float a = alpha > 12 ? 1 : alpha / 12;  // a <= 1
    float theta = 1.5;                      // as theta smaller, OF[0] ~ [3] will bigger (approaching 255)
    // Sigmoid function for fade in/out
    OF[0] = char(1 / (pow(exp, -5 * gamma_r * (a - 0.5)) + 1) * 140);
    OF[1] = char(1 / (pow(exp, -5 * gamma_g * (a - 0.5)) + 1) * 140);
    OF[2] = char(1 / (pow(exp, -5 * gamma_b * (a - 0.5)) + 1) * 140);
    OF[3] = B;
    OF[4] = G;
    OF[5] = R;
};

#endif
