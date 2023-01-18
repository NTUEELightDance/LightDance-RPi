#include "LEDPlayer.h"
#include "OFPlayer.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int playing;
long baseTime;

int main() {
    baseTime = 0;

    pthread_t id;
    pthread_create(&id, NULL, LEDPlayer::loop, NULL);

    while (true) {
        sleep(1);
        printf("change time\n");
        baseTime += 1;
    }

    return 0;
}
