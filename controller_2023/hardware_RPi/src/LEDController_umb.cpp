#include "LEDController_umb.h"

LEDColor::LEDColor() : r(0), g(0), b(0), rgb(0) {}

LEDColor::LEDColor(const int &colorCode) {
    const int R = (colorCode >> 24) & 0xff;
    const int G = (colorCode >> 16) & 0xff;
    const int B = (colorCode >> 8) & 0xff;
    const int A = (colorCode >> 0) & 0xff;

    const float gamma = 1;
    // convert rgba to rgb

    // GAMMA CORRECTION
    r = (int)(pow(R * A, (1 / gamma)));
    g = (int)(pow(G * A, (1 / gamma)));
    b = (int)(pow(B * A, (1 / gamma)));
    printf("%X, %X, %X", r, g, b);
    rgb = ((r << 16) + (g << 8) + b);
}

uint32_t LEDColor::getRGB() { return rgb; }

LEDController::LEDController() {
    // if (stripShape != NULL) stripShape = NULL;
}

int LEDController::init(const std::vector<int> &shape) {
    // member variables initialization
    stripShape.assign(shape.begin(), shape.end());

    // initialize WS2812B
    ws2811_return_t ret;

    // ledstrips count initialize
    //ledstrip_1

    // initialize GPIO_PIN
    // gpioInit();

    ledString[0].channel[0].count = shape[0];
    if ((ret = ws2811_init(&ledString[0])) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init %d failed: %s\n", 0, ws2811_get_return_t_str(ret));
        return ret;
    }
    ledString[1].channel[0].count = shape[1];
    if ((ret = ws2811_init(&ledString[1])) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init %d failed: %s\n", 1, ws2811_get_return_t_str(ret));
        return ret;
    }

    for (int i = 0; i < ledString[0].channel[0].count ; i++){
        ledString[0].channel[0].leds[i] = 0;
    }
    if ((ret = ws2811_render(&ledString[0])) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_render %d failed: %s\n", 0, ws2811_get_return_t_str(ret));
        return ret;
    }
    usleep(stripShape[0] * 30);

    for (int i = 0; i < ledString[1].channel[0].count ; i++){
        ledString[1].channel[0].leds[i] = 0;
    }
    if ((ret = ws2811_render(&ledString[1])) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_render %d failed: %s\n", 1, ws2811_get_return_t_str(ret));
        return ret;
    }
    usleep(stripShape[1] * 30);
    return WS2811_SUCCESS;
}

int LEDController::sendAll(const std::vector<std::vector<int>> &statusLists) {
    // Check if data size is consistent with stored during initialization

    if (statusLists[0].size() > stripShape[0]) {
        printf("Error: Strip %d is longer then init settings: %d", (int)statusLists[0].size(),
               stripShape[0]);
        return -1;
    }
    if (statusLists[1].size() > stripShape[1]) {
        printf("Error: Strip %d is longer then init settings: %d", (int)statusLists[1].size(),
               stripShape[1]);
        return -1;
    }
    // Push data to LED strips
    play(statusLists);
    return 0;
}

int LEDController::play(const std::vector<std::vector<int>> &statusLists) {
    ws2811_return_t ret;

    for (int i=0 ; i<stripShape[0]; i++){
        LEDColor led(statusLists[0][i]);
        ledString[0].channel[0].leds[i] = led.getRGB();
        if (i == 0) printf("rgb now: %X\n\n", led.getRGB());

    }
    if ((ret = ws2811_render(&ledString[0])) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_render %d failed: %s\n", 0, ws2811_get_return_t_str(ret));
        return ret;
    }
    usleep(stripShape[0] * 30);

    for (int i=0 ; i<stripShape[1]; i++){
        LEDColor led(statusLists[1][i]);
        ledString[1].channel[0].leds[i] = led.getRGB();
        if (i == 0) printf("rgb now: %X\n\n", led.getRGB());

    }
    if ((ret = ws2811_render(&ledString[1])) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_render %d failed: %s\n", 1, ws2811_get_return_t_str(ret));
        return ret;
    }
    usleep(stripShape[1] * 30);
    return 0;
}

void LEDController::fini() {
    stripShape.clear();
    ws2811_fini(&ledString[0]);
    ws2811_fini(&ledString[1]);
}
