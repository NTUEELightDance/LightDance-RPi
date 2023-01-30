#include <fcntl.h>  // O_WRONLY
#include <stdio.h>
#include <stdlib.h>  // exit
#include <string.h>
#include <sys/stat.h>
#include <time.h>  // time
#include <unistd.h>

#define ENDFRAME 3600
#define ENDTIME 3600.0
int main(int argc, char *argv[]) {
    int fd;
    int n, i;
    char buf[1024];

    printf("I am %d process.\n", getpid());  // 说明进程ID
    float startTime, endTime;
    int startFrame, endFrame;
    char flag;
    if (argc == 1) {
        startTime = 0.0;
        endTime = ENDTIME;  // 60 min
        flag = 't';
    }
    if (argc == 2) {
        if (strcmp(argv[1], "-f") == 0) {
            printf("please enter your start frame(int):");
            scanf(" %d", &startFrame);
            printf("please enter your end frame(int):");
            scanf(" %d", &endFrame);
        }
        if (strcmp(argv[1], "-t") == 0) {
            printf("please enter your start frame(int):");
            scanf(" %f", &startTime);
            printf("please enter your end frame(int):");
            scanf(" %f", &endTime);
        }
        flag = argv[1][1];
    }
    if (argc == 3) {
        printf("%s", argv[2]);
        if (strcmp(argv[1], "-f") == 0) {
            startFrame = atoi(argv[2]);
            endFrame = ENDFRAME;
        }
        if (strcmp(argv[1], "-t") == 0) {
            startTime = strtof(argv[2], NULL);
            endTime = ENDTIME;
        }
        flag = argv[1][1];
    }
    if (argc == 4) {
        if (strcmp(argv[1], "-f") == 0) {
            startFrame = atoi(argv[2]);
            endFrame = atoi(argv[3]);
        }
        if (strcmp(argv[1], "-t") == 0) {
            startTime = strtof(argv[2], NULL);
            endTime = strtof(argv[3], NULL);
        }
        flag = argv[1][1];
    }

    if ((fd = open("fifo1", O_WRONLY)) < 0)  // 以写打开一个FIFO
    {
        perror("Open FIFO Failed");
        exit(1);
    }

    if (flag == 'f') {
        n = sprintf(buf, "Process %d, -%c %d %d", getpid(), flag, startFrame, endFrame);
    }
    if (flag == 't') {
        n = sprintf(buf, "Process %d, -%c %4.2f %4.2f", getpid(), flag, startTime, endTime);
    }

    printf("Send message: %s", buf);
    if (write(fd, buf, n + 1) < 0) {
        perror("Write FIFO Failed");
        close(fd);
        exit(1);
    }
    // sleep(1);  // 休眠1秒

    close(fd);  // should be comment in formal version
    return 0;
}