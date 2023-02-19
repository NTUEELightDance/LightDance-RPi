#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLEN 100

int main() {
    int fd;

    const char *fifo = "/tmp/cmd";
    mkfifo(fifo, 0666);

    char cmd[MAXLEN];
    while (1) {
        fd = open(fifo, O_WRONLY);

        fgets(cmd, MAXLEN, stdin);

        write(fd, cmd, strlen(cmd)+1);
        close(fd);
    }
    return 0;
}