#ifndef OFRGBA_TO_RGB_H
#define OFRGBA_TO_RGB_H

#include <math.h>

#include <vector>

#define gamma_r 1.6
#define gamma_g 1.8
#define gamma_b 1.66

float sigmoid(float gamma, float a) {
    const float exp = 2.71828;
    const int max_scale = 140;
    return 1 / (pow(exp, -5 * gamma * (a - 0.5)) + 1) * max_scale;
}

void OFrgba2rgbiref(std::vector<char>& OF, const char& R, const char& G, const char& B, const float& alpha) {
    float a = alpha > 12 ? 1 : alpha / 12;  // a <= 1
    // Sigmoid function for fade in/out
    OF[0] = char(int8_t(sigmoid(gamma_r, a) - sigmoid(gamma_r, 0)));
    OF[1] = char(int8_t(sigmoid(gamma_g, a) - sigmoid(gamma_g, 0)));
    OF[2] = char(int8_t(sigmoid(gamma_b, a) - sigmoid(gamma_b, 0)));
    OF[3] = B;
    OF[4] = G;
    OF[5] = R;
};

#endif
