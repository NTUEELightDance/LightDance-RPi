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
        return 0;
    }

   private:
    int play(int start, int end) {
        cout << "start: " << start << " end: " << end << endl;
        return 0;
    }
};

int main(int argc, char* argv[]) {
    Play cmd;
    return cmd.execute(argc, argv);
}