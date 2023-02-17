#include "OFController.h"

// ==================== OFStatus ============================

OFColor::OFColor() : r(0), g(0), b(0) {}

OFColor::OFColor(const int &colorCode) {
    const int R = (colorCode >> 24) & 0xff;
    const int G = (colorCode >> 16) & 0xff;
    const int B = (colorCode >> 8) & 0xff;
    const int A = (colorCode >> 0) & 0xff;

    // TODO: convert rgba to rgb
}

// ==================== OFController ========================

OFController::OFController() {}

int OFController::init() {
    printf("Hardware Initialzed\n");
    return 1;
}

int OFController::sendAll(const vector<int> &statusLists) {
    printf("%d strips sent\n", (int)statusLists.size());
    return 1;
}
