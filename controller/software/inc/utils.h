#ifndef __UTILS_H__
#define __UTILS_H__

#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

inline int tryGetLock(char const *fileName) {
    int fd = open(fileName, O_RDWR, 0666);
    if (fd < 0) {
        return -2;
    }
    int rc = flock(fd, LOCK_EX | LOCK_NB);
    if (rc) {
        close(fd);
        return -1;
    }
    return fd;
}

inline void releaseLock(int fd, char const *fileName) {
    flock(fd, LOCK_UN);
    close(fd);
}

inline std::string parseMicroSec(long us) {
    char buf[100];
    int ms = us / 1000;
    int s = ms / 1000;
    int m = s / 60;
    int h = m / 60;
    sprintf(buf, "%02d:%02d:%02d.%03d", h, m % 60, s % 60, ms % 1000);
    return std::string(buf);
}


#endif
