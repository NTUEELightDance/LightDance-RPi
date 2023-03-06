#include "LEDController.h"

// ==================== LEDColor =============================

LEDColor::LEDColor() : r(0), g(0), b(0) {}

LEDColor::LEDColor(const int &colorCode) {
    const int R = (colorCode >> 24) & 0xff;
    const int G = (colorCode >> 16) & 0xff;
    const int B = (colorCode >> 8) & 0xff;
    const int A = (colorCode >> 0) & 0xff;

    // TODO: convert rgba to rgb
}

// ==================== LEDController ========================

LEDController::LEDController() {}

int LEDController::init(const vector<int> &shape) {
    printf("Hardware Initialzed\n");
    return 1;
}

int LEDController::sendAll(const vector<vector<int>> &statusLists) {
    for (int i = 0; i < statusLists.size(); i++) {
        const vector<int> statusList = statusLists[i];
        if (statusList.size() > 0) {
            printf("  strip %d:", i);
            for (int j = 0; j < statusList.size() && j < 5; j++) {
                printf(" %d ", statusList[j]);
            }
            printf("\n");
        }
    }
    return 1;
}

void LEDController::fini() {}
