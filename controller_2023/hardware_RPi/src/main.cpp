#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;

#include "LEDController.h"

int main() {
    LEDController strip;
    vector<int> shape;

    int num_strip = 8;

    shape.clear();
    shape.resize(num_strip);

    for (int i = 0; i < num_strip; i++) shape[i] = 3;
    strip.init(shape);

    vector<vector<int>> status;

    status.resize(num_strip);

    for (int i = 0; i < num_strip; i++) {
        status[i].resize(shape[i]);
        for (int j = 0; j < shape[i]; j++) status[i][j] = 0x00000010;
    }
    strip.sendAll(status);

    for (int it = 0; it < 50; it++) {
        for (int i = 0; i < num_strip; i++) {
            status[i].resize(shape[i]);
            for (int j = 0; j < shape[i]; j++) status[i][j] += 0x00000100;
        }
        strip.sendAll(status);
        printf("Now light: %X\n", status[0][0]);
        usleep(100000);
    }
    return 0;
}
