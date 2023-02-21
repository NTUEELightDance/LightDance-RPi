#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd;
    const char *report_fifo = "/tmp/report";

    char str1[80];
    while(1) {
        fd = open(report_fifo,O_RDONLY);
        read(fd, str1, 80);

        printf("%s\n", str1);
        close(fd);
    }
    return 0;
}