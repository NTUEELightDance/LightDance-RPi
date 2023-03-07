#include "OFController.h"

// ==================== OFColor ============================

OFColor::OFColor() : r(0), g(0), b(0) {}

OFColor::OFColor(const int &colorCode) {
    const int R = (colorCode >> 24) & 0xff;
    const int G = (colorCode >> 16) & 0xff;
    const int B = (colorCode >> 8) & 0xff;
    const int A = (colorCode >> 0) & 0xff;

/*    // TODO: convert rgba to rgb
    const float gamma = 2.2;
    // convert rgba to rgb

    // GAMMA CORRECTION
    r = (int)(pow(R * A, (1 / gamma)));
    g = (int)(pow(G * A, (1 / gamma)));
    b = (int)(pow(B * A, (1 / gamma)));
*/
    r = R;
    g = G;
    b = B;
}

int OFColor::getR() { return r; }
int OFColor::getG() { return g; }
int OFColor::getB() { return b; }

// ==================== OFController ========================

OFController::OFController() {}

int OFController::init() {
    unsigned char buffer[10];
    printf("Hardware Initialzed\n");

    // open I2C bus
    for (int i = 0; i < NUMPCA; i++) {
        if ((fd[i] = I2CInit()) < 0) {
            printf("I2C of %d init fail.\n", i);
            return 1;
        }
        printf("File descriptor of %d opened at %d.\n", i, fd[i]);

        if (ioctl(fd[i], I2C_SLAVE, PCAAddr[i]) < 0) {
            printf("Failed to acquire bus access and/or talk to slave %d", i);
            return 2;
        }
    }
    printf("======================\n\n");

    // write to PCA
    buffer[0] = 0x45;   // 0x45
    buffer[1] = 0xFF;  // 0xFF

    for (int i = 0; i < NUMPCA; i++) {
        if (write(fd[i], buffer, 2) != 2) {
            printf("Failed to write to I2C bus %d.\n", i);
	}
	printf("Now sending: ");
	for (int j = 0; j < 2; j++)
		printf("%d, ", buffer[j]);
	printf("\n");
        
    }

    return 1;
}

int OFController::sendAll(const vector<int> &statusLists) {
    unsigned char buffer[20];
    printf("%d strips sent\n", (int)statusLists.size());
    // OFController RGBData[(int)statusLists.size()];
    buffer[0] = 136;  // 0x88
    int counter;
    for (int i = 0; i < NUMPCA; i++) {
        counter = 0;
        for (int j = i * 5; j < 5 * (i + 1); j++) {
            //          printf("Counter: %d, i: %d, j: %d\n", counter, i, j);
            OFColor Status(statusLists[j]);
            buffer[counter * 3 + 1] = Status.getR();
            buffer[counter * 3 + 2] = Status.getG();
            buffer[counter * 3 + 3] = Status.getB();
            counter++;
        }
        if (write(fd[i], buffer, 16) != 16) {
            printf("Failed to write to the I2C bus %d.\n", i);
            //return 4;
        }
    }

    return 1;
}

int OFController::I2CInit() {
    int fileI2C;
    char *filename = (char *)"/dev/i2c-1";
    if ((fileI2C = open(filename, O_RDWR)) < 0) {
        printf("Failed to open i2c bus. Exit.\n");
        return -1;
    }
    return fileI2C;
}
