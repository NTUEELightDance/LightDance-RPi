#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "player.h"

class BoardInfo {
   public:
    BoardInfo();
    void getIp();
    void getRpiName();  // Hostname
    void getDancerName();

   private:
    char* RpiName;
    char* Ip;
    std::string dancerName;
};

BoardInfo::BoardInfo() {
    getIp();
    getRpiName();
    getDancerName();
};

void BoardInfo::getIp() {
    FILE* fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("hostname -I", "r");
    if (fp == NULL) {
        printf("Failed to get ip");
        return;
    } else {
        if (fgets(path, sizeof(path), fp) !=
            NULL) {  // Read the output a line at a time - output it.
            Ip = path;
            printf("ip: %s", Ip);
            pclose(fp);  // close

            return;
        }
    }

    return;
};

void BoardInfo::getRpiName() {
    FILE* fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("hostname", "r");
    if (fp == NULL) {
        printf("Failed to get hostname");
        return;
    } else {
        if (fgets(path, sizeof(path), fp) !=
            NULL) {  // Read the output a line at a time - output it.
            RpiName = path;
            printf("Hostname: %s", RpiName);
            pclose(fp);  // close
            return;
        }
    }
    return;
};

void BoardInfo::getDancerName() {
    Player new_dancer;
    if (!restorePlayer(new_dancer, "dancer.dat")) {
        printf("Failed to get dancer name\n");
        exit(1);
    };
    dancerName = new_dancer.getDancerName();
    printf("Dancer name: %s\n", dancerName.c_str());
    return;
};

int main(int argc, char* argv[]) {
    BoardInfo boardInfo;
    return 0;
}