#include "command.h"
#include "player.h"
#include "OFController.h"

class OFtest : public Command {
   public:
    OFtest() : Command() {
        addFlag("-h", "help");
        addFlag("--rgb", "RGB values");
        addFlag("--hex", "colorcode");
        addFlag("-a", "alpha");
    }
    int execute(int argc, char* argv[]) {
        // cout<<argc<<"\n";
        if (argc <= 1 || cmdOptionExists(argv, argv + argc, "-h")) {
            cout<<"./oftest <PartName>\n";
            help();
            return 0;
        }
        string PartName = argv[1];
        int R = 255, G = 255, B = 255, alpha = 255;
        if (cmdOptionExists(argv, argv + argc, "--rgb")) {
            vector<int> rgb = getCmdOptionInt(argv, argv + argc, "--rgb");
            if (rgb.size() >= 3) {
                R = rgb[0];
                G = rgb[1];
                B = rgb[2];

            } else {
                cout << "Wrong RGB length! Using default\n";
                return 0;
            }
        } else if (cmdOptionExists(argv, argv + argc, "--hex")) {
            vector<string> hex = getCmdOptionStr(argv, argv + argc, "--hex");
            if (hex.size()){
                colorCode2RGB(hex[0], R, G, B);
            } else{
                cout << "Hex value not specified! Using default\n";
            }
        }
        if (cmdOptionExists(argv, argv + argc, "-a")) {
            vector<int> _alpha = getCmdOptionInt(argv, argv + argc, "-a");
            if (_alpha.size()){
                alpha = _alpha[0];
            } else{
                cout<<"Alpha value not specified! Using default\n";
            }        }
        return Test(PartName, R, G, B, alpha);
    }

   private:

    const int partNum = 35;
    map<char, int> hexCode = {
        {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
        {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'A', 10}, {'B', 11},
        {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'a', 10}, {'b', 11},
        {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}};

    void colorCode2RGB(string hex, int& R, int& G, int& B) {
        if (hex.size() == 6) {
            R = hexCode[hex[0]] * 16 + hexCode[hex[1]];
            G = hexCode[hex[2]] * 16 + hexCode[hex[3]];
            B = hexCode[hex[4]] * 16 + hexCode[hex[5]];
        } else {
            cout << "Colorcode length error!\n";
        }
    }
    int Test(const string &part, int R, int G, int B, int alpha) {
        
        Player player;
        string path = "./dancer.dat";
        if (!restorePlayer(player, path.c_str())) {
            cout<<"Need to load first!\n";
            return 0;
        }
        vector<int> OFbuf (partNum);
        if (player.OFPARTS.find(part) == player.OFPARTS.end()){
            cout<<"Can't find part "<<part<<"!\n";

        } else{
            int id = player.OFPARTS[part];
            int color = (R<<24) + (G<<16) + (B<<8) + alpha;

            OFbuf[id] = color;

            OFController OF_CTRL;
            OF_CTRL.init();
            OF_CTRL.sendAll(OFbuf);
        }
        return 0;
    }
};

int main(int argc, char* argv[]) {
    OFtest test;
    return test.execute(argc, argv);
}

