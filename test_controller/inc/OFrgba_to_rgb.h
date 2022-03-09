#include <math.h>
#define gamma_r 1.6
#define gamma_g 1.8
#define gamma_b 1.66

void OFrgba_to_rgbiref(vector<char>& OF, const char& R, const char& G, const char& B, const float& a) {
    OF[0] = char(pow(a / 3, gamma_r) * 255);
    OF[1] = char(pow(a / 3, gamma_g) * 255);
    OF[2] = char(pow(a / 3, gamma_b) * 255);
    OF[3] = R;
    OF[4] = G;
    OF[5] = B;
};
