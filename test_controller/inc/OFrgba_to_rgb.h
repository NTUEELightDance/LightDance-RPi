#include <math.h>
#define gamma_r 1.6
#define gamma_g 1.8
#define gamma_b 1.66

int *OFrgba_to_rgbiref(int R, int G, int B, float a)
{
  int *color = new int[3];

  color[0] = int(pow(a/3, gamma_r) * 255);
  color[1] = int(pow(a/3, gamma_g) * 255);
  color[2] = int(pow(a/3, gamma_b) * 255);
  color[3] = R;
  color[4] = G;
  color[5] = B;

  return color;
};
