#include "command.h"
#include<stdio.h>
#include<stdlib.h>   // exit
#include<fcntl.h>    // O_WRONLY
#include<sys/stat.h>
#include<time.h>     // time
#include<unistd.h>
#include<string>
#include <set>

#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

class Play : public Command {
   public:
    Play() : Command() {
        addFlag("-h", "help");
        addFlag("-t", "ms");
    }
    int execute(int argc, char* argv[]) {
        if (cmdOptionExists(argv, argv + argc, "-h")) {
            help();
            return 0;
        }
        if (cmdOptionExists(argv, argv + argc, "-t")) {
            vector<int> time = getCmdOptionInt(argv, argv + argc, "-t");
            if (time.size() == 1) {
                return play(time[0], -1);
            } else if (time.size() == 2) {
                return play(time[0], time[1]);
            } else {
                cout << "Error in -t option" << endl;
                return 1;
            }
        }
        if(argc>=2 ){
            string cmd = argv[1];
            transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return tolower(c); });
            if (set<string>{"pause", "quit", "restart", "stop"}.count(cmd) > 0){
                sendToPlayLoop(cmd);
            }
        }
        return 0;
    }

   private:
    int play(int start, int end) {
        cout << "start: " << start << " end: " << end << endl;
        return 0;
    }
    int sendToPlayLoop(string msg){

        int fd;
        int n;
        char buf[1024];

        if((fd = open("fifo1", O_WRONLY)) < 0) // 以写打开一个FIFO 
        {
            perror("Open FIFO Failed");
            return 1;
        }
        
        n=sprintf(buf,"Process %d, %s",getpid(),msg.c_str());
        printf("Send message: %s", buf);
        if(write(fd, buf, n+1) < 0)
        {
            perror("Write FIFO Failed");
            close(fd);
            return 1;
        }
        // close(fd);  // should be comment in formal version
        return 0;
    }
};

int main(int argc, char* argv[]) {
    Play cmd;
    return cmd.execute(argc, argv);
}