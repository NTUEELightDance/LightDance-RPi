#include <fcntl.h>  // O_WRONLY
#include <stdio.h>
#include <stdlib.h>  // exit
#include <sys/stat.h>
#include <time.h>  // time
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <string>

#include "command.h"

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
        if (argc >= 2) {
            string cmd = argv[1];
            transform(cmd.begin(), cmd.end(), cmd.begin(),
                      [](unsigned char c) { return tolower(c); });
            if (set<string>{"pause", "quit", "restart", "stop"}.count(cmd) > 0) {
                sendToPlayLoop(cmd);
            }
            if (cmd.compare("play") == 0) {
                if (cmdOptionExists(argv, argv + argc, "-s")) {
                    vector<float> ftime = getCmdOptionFloat(argv, argv + argc, "-s");
                    for (auto i : ftime) {
                        cout << "hi" << i << endl;
                        if (int(i * 1000) < -1.0) {
                            cout << "It might be overflowed.\nPlease check your input!." << endl;
                            return 1;
                        }
                    }
                    if (ftime.size() == 1) {
                        return play(int(ftime[0] * 1000), -1);
                    } else if (ftime.size() == 2) {
                        return play(int(ftime[0] * 1000), int(ftime[1] * 1000));
                    } else {
                        if (argc == 2) {
                            cout << "Error in -s option.";
                            cout << "Please it should have time after -s flag." << endl;
                            return 1;
                        }
                        cout << "Error in -s option.\nPlease use \"-h\" for help." << endl;
                        return 1;
                    }
                }
                vector<int> time = getCmdOptionInt(argv, argv + argc, "play");
                for (auto i : time) {
                    if (i < -1) {
                        cout << "It might be overflowed.\nPlease check your input!." << endl;
                        return 1;
                    }
                }
                if (time.size() == 1) {
                    return play(time[0], -1);
                } else if (time.size() == 2) {
                    return play(time[0], time[1]);
                } else {
                    if (argc == 2) return sendToPlayLoop("play");
                    cout << "Error in command play.\n";
                    cout << "It should be \"play <start time(int)(option)> <end "
                            "time(int)(option)>\".\n";
                    cout << "Please use \"-h\" for help." << endl;
                    return 1;
                }
            }
        }
        return 0;
    }

   private:
    int play(int start, int end) {
        stringstream cmd;
        cmd << "play " << start << " " << end;
        string mycmd = cmd.str();
        sendToPlayLoop(mycmd);
        cout << "start: " << start << " end: " << end << endl;
        return 0;
    }
    int sendToPlayLoop(string msg) {
        int fd;
        int n;
        char buf[1024];

        if ((fd = open("fifo1", O_WRONLY)) < 0)  // 以写打开一个FIFO
        {
            perror("Open FIFO Failed");
            return 1;
        }

        n = sprintf(buf, "%s", msg.c_str());
        printf("Send message: %s\n", buf);
        if (write(fd, buf, n + 1) < 0) {
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