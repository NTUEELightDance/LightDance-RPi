#include "command.h"

class LEDtest: public Command{
    public:
    LEDtest(): Command(){
        addFlag("-h", "help");
        addFlag("--rgb", "RGB values");
        addFlag("--hex", "colorcode");
        addFlag("-a", "alpha");
    }
    int execute(int argc, char* argv[]){
        // cout<<argc<<"\n";
        if (argc <= 1 || cmdOptionExists(argv, argv + argc, "-h")) {
            help();
            return 0;
        }
        int channel = atoi(argv[1]), R = 0, G = 0, B = 0, alpha = 1;
        if (cmdOptionExists(argv, argv + argc, "--rgb")) {
            vector<int> rgb = getCmdOptionInt(argv, argv + argc, "--rgb");
            if (rgb.size() >= 3){
                R = rgb[0];
                G = rgb[1];
                B = rgb[2];

            } else{
                cout<<"Wrong RGB length\n";
                return 0;
            }
        } else if (cmdOptionExists(argv, argv + argc, "--hex")){
            string hex = getCmdOptionStr(argv, argv + argc, "--hex")[0];
            colorCode2RGB(hex, R, G, B);
        }
        if (cmdOptionExists(argv, argv + argc, "-a")) {
            alpha = getCmdOptionInt(argv, argv + argc, "-a")[0];
        }
        return Test(channel, R, G, B, alpha);
    }
    private:
    map<char, int> hexCode = {
        {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4},
        {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
        {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}, 
        {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}};

    void colorCode2RGB(string hex, int &R, int &G, int &B){
        if (hex.size() == 6){
            R = hexCode[hex[0]] * 16 + hexCode[hex[1]];
            G = hexCode[hex[2]] * 16 + hexCode[hex[3]];
            B = hexCode[hex[4]] * 16 + hexCode[hex[5]];
        } else{
            cout<<"Colorcode length error!\n";
        }
    }
    int Test(const int& channel, int R, int G, int B, int alpha){
        //Todo : Convert colorCode and alpha to status object

        //Todo : Use sendAll to send information to hardware

        // Do we use LED_buf? Don't think it's necessary...
        // They should use setDancer before this command. Should we check that?

        cout<<"Doing LED test:\n";
        cout<<"Channel: "<<channel<<"\n";
        cout<<"RGB: "<<R<<" "<<G<<" "<<B<<"\n";
        cout<<"Alpha: "<<alpha<<"\n";
        
        return 0;
    }
    
};

int main(int argc, char* argv[]){
    LEDtest test;
    return test.execute(argc, argv);
}



// void Command::LEDtest(const int& channel, Status status) {
//     //應該有更方便取得shape的方法...
//     vector<vector<Status> > LED_buf(LED_SIZE, vector<Status> ());
//     for (auto i:LEDPARTS){
//         LED_buf[i.second.first].resize(i.second.second);
//     }
//     int len = LED_buf[channel].size();
//     for (int i=0;i<len;i++){
//         LED_buf[channel][i] = status;
//     }
//     if (LED_CTRL.checkReady() == 1){
//         LED_CTRL.sendAll(LED_buf);
//     } else{
//         printf("LED not ready...\n");
//     }
//     return;
// }

// void RPiMgr::LEDtest(const int& channel, int colorCode, int alpha) {
//     // Will change led buf;
//     const float _alpha = float(alpha) / ALPHA_RANGE;
//     char R, G, B;
//     colorCode2RGB(colorCode, R, G, B);
//     // ledDark();
//     for (int i = 0; i < LEDBuf[channel].size() / 3; ++i)
//         LEDrgba_to_rgb(LEDBuf[channel], i, R, G, B, _alpha);
//     led_strip->sendToStrip(LEDBuf);
//     return;
// }

// sudo apt-get install libboost-all-dev