#include "gamma_correction.h"

#include <math.h>

#include <iostream>
#define MAX_BRIGHTNESS 200
using namespace std;

Color_regulator::Color_regulator(float g) {
    gamma = g;
}
int Color_regulator::gamma_correction(float c) {
    float temp_c = (c) / 255;
    float c_out = pow(temp_c, gamma) * 255;
    return int(c_out + 0.5);
}
float* rgba_to_rgb(float r, float g, float b, float a) {
    float* color = new float[3];
    a = a / 15;
    r = r / (r + g + b);
    g = g / (r + g + b);
    b = b / (r + g + b);
    color[0] = r * a * MAX_BRIGHTNESS;
    color[1] = g * a * MAX_BRIGHTNESS;
    color[2] = b * a * MAX_BRIGHTNESS;
    return color;
}
void LEDrgba_to_rgb(vector<char>& LED, const int& index, const char& R, const char& G, const char& B, const float& a) {
    float r = float(R) / float(R + G + B);
    float g = float(G) / float(R + G + B);
    float b = float(B) / float(R + G + B);
    // LED format, not error
    LED[3 * index] = char(g * a * MAX_BRIGHTNESS);
    LED[3 * index + 1] = char(r * a * MAX_BRIGHTNESS);
    LED[3 * index + 2] = char(b * a * MAX_BRIGHTNESS);
}
// int main(int argc, char *argv[])
// {
//   return 0;
// }