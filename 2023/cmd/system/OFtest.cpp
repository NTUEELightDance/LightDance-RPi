#include "command.h"

class OFtest: public Command{
    public:
    OFtest(): Command(){
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

        cout<<"Doing OF test:\n";
        cout<<"Channel: "<<channel<<"\n";
        cout<<"RGB: "<<R<<" "<<G<<" "<<B<<"\n";
        cout<<"Alpha: "<<alpha<<"\n";
        
        return 0;
    }
    
};

int main(int argc, char* argv[]){
    OFtest test;
    return test.execute(argc, argv);
}



// void Command::OFtest(const int& channel, Status status) {

//     vector<Status> OF_buf(OF_SIZE, Status(0, 0, 0, 0));

//     OF_buf[channel] = status;

//     OPT_CTRL.sendAll(OF_buf);
//     return;
// }

// void RPiMgr::OFtest(const int& channel, int colorCode, int alpha) {
//     // Will not change of buf
//     vector<char> buf(6);
//     char R, G, B;
//     colorCode2RGB(colorCode, R, G, B);
//     // ofDark();
//     OFrgba2rgbiref(buf, R, G, B, alpha);
//     // OFrgba2rgbiref(OFBuf[channel], R, G, B, alpha);
//     // of->WriteAll(OFBuf);
//     of->WriteChannel(buf, channel);
//     return;
// }

