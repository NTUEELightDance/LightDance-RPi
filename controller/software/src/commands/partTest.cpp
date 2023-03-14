#include "command.h"
#include "const.h"
#include "player.h"

class PartTest : public Command {
   public:
    PartTest() : Command() {
        addFlag("-h", "help");
        addFlag("--rgb", "RGB values");
        addFlag("--hex", "color hex");
        addFlag("-a", "alpha");
    }
    int execute(int argc, char* argv[]) {
        // cout<<argc<<"\n";
        if (cmdOptionExists(argv, argv + argc, "-h")) {
            cout << "parttest <PartName>\n";
            help();
            return 0;
        }

        // determine if the part name is specified
        string PartName = "All";
        if (argc >= 2 && argv[1][0] != '-') {  // not a flag
            PartName = argv[1];
        }

        int R = 255, G = 255, B = 255, alpha = 255;
        if (cmdOptionExists(argv, argv + argc, "--rgb")) {
            vector<int> rgb = getCmdOptionInt(argv, argv + argc, "--rgb");
            if (rgb.size() >= 3) {
                R = rgb[0];
                G = rgb[1];
                B = rgb[2];
            } else {
                cout << "Wrong RGB length! Using default all white.\n";
            }
        } else if (cmdOptionExists(argv, argv + argc, "--hex")) {
            vector<string> hex = getCmdOptionStr(argv, argv + argc, "--hex");
            if (hex.size()) {
                colorCode2RGB(hex[0], R, G, B);
            } else {
                cout << "Hex value not specified! Using default all white.\n";
            }
        }
        if (cmdOptionExists(argv, argv + argc, "-a")) {
            vector<int> _alpha = getCmdOptionInt(argv, argv + argc, "-a");
            if (_alpha.size()) {
                alpha = _alpha[0];
            } else {
                cout << "Alpha value not specified! Using default\n";
            }
        }
        return Test(PartName, R, G, B, alpha);
    }

   private:
    map<char, int> hexCode = {{'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
                              {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'A', 10}, {'B', 11},
                              {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, {'a', 10}, {'b', 11},
                              {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}};

    void colorCode2RGB(string hex, int& R, int& G, int& B) {
        if (hex.size() == 6) {
            R = hexCode[hex[0]] * 16 + hexCode[hex[1]];
            G = hexCode[hex[2]] * 16 + hexCode[hex[3]];
            B = hexCode[hex[4]] * 16 + hexCode[hex[5]];
        } else {
            cout << "Color hex length error!\n";
        }
    }
    int Test(const string& part, int R, int G, int B, int alpha) {
        Player player;
        string base_path = BASE_PATH;
        string path = base_path + "data/dancer.dat";

        if (!restorePlayer(player, path.c_str())) {
            cout << "Need to load first!\n";
            return 0;
        }

        vector<int>& OFbuf = player.myOFPlayer.currentStatus;
        vector<vector<int> >& LEDbuf = player.myLEDPlayer.currentStatus;
        vector<int> Shape(LED_NUM, 0);
        for (auto parts : player.LEDPARTS) {
            int id = parts.second.id;
            int len = parts.second.len;
            Shape[id] = len;
        }
        int color = (R << 24) + (G << 16) + (B << 8) + alpha;

        if (part == "All" || part == "ALL" || part == "all") {
            for (auto part : player.OFPARTS) {
                OFbuf[part.second] = color;
            }
            for (int i = 0; i < LED_NUM; i++) {
                for (int j = 0; j < Shape[i]; j++) {
                    LEDbuf[i][j] = color;
                }
            }
        } else if (player.OFPARTS.find(part) != player.OFPARTS.end()) {
            int id = player.OFPARTS[part];
            OFbuf[id] = color;
        } else if (player.LEDPARTS.find(part) != player.LEDPARTS.end()) {
            int i = player.LEDPARTS[part].id;
            for (int j = 0; j < Shape[i]; j++) {
                LEDbuf[i][j] = color;
            }
        } else {
            cout << "Error: Can't find part " << part << "!\n";
            return 0;
        }

        OFController& OF_CTRL = player.myOFPlayer.controller;
        LEDController& LED_CTRL = player.myLEDPlayer.controller;
        OF_CTRL.init();
        LED_CTRL.init(Shape);

        cout << "sending OF" << endl;
        OF_CTRL.sendAll(OFbuf);

        cout << "sending LED" << endl;
        LED_CTRL.sendAll(LEDbuf);
        LED_CTRL.finish();

        savePlayer(player, path.c_str());
        cout << "done" << endl;

        return 0;
    }
};

int main(int argc, char* argv[]) {
    PartTest test;
    return test.execute(argc, argv);
}
