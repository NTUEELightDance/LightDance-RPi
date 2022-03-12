#ifndef GAMMA_CORRECTION_H
#define GAMMA_CORRECTION_H

#include <math.h>

#include <iostream>
#include <vector>

using namespace std;

class Color_regulator {
   public:
    Color_regulator(){};
    Color_regulator(float);
    int gamma_correction(float c);
    void set_gamma(float&);

   private:
    float gamma;
};
float* rgba_to_rgb(float r, float g, float b, float a);
void LEDrgba_to_rgb(vector<char>& LED, const int& index, const char& R, const char& G, const char& B, const float& a);

#endif