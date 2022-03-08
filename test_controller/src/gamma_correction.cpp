#include <math.h>
#include <iostream>
#include "../inc/gamma_correction.h"
#define MAX_BRIGHTNESS 200
using namespace std;

Color_regulator::Color_regulator(float g){
  gamma=g;
}
int Color_regulator:: gamma_correction(float c)
{
  float temp_c = (c) / 255;
  float c_out = pow(temp_c, gamma) * 255;
  return int(c_out + 0.5);
}
float* rgba_to_rgb(float r, float g, float b, float a)
{
  float* color=new float[3];
  a=a/15;
  r=r/(r+g+b);
  g=g/(r+g+b);
  b=b/(r+g+b);
  color[0]=r*a*MAX_BRIGHTNESS;
  color[1]=g*a*MAX_BRIGHTNESS;
  color[2]=b*a*MAX_BRIGHTNESS;
  return color;
}
// int main(int argc, char *argv[])
// {
//   return 0;
// }