#include "command.h"

class LEDtest: public Command{
    public:
    LEDtest(): Command(){
        addFlag("-h", "help");
        addFlag("-o", "options");
    }
    int execute(int argc, char* argv[]){
        if (cmdOptionExists(argv, argv + argc, "-h")) {
            help();
            return 0;
        }
        if(cmdOptionExists(argv, argv + argc, "-o")) {
            vector<int> options = getCmdOptionInt(argv, argv + argc, "-o");
            if (options.size() == 3){
                return Test(options[0], options[1], options[2]);
            } else{
                cout<<"Expected 3 arguments (Channel ColorCode Alpha), got " << options.size() << ".\n";
                return 0;
            }
        }
        return 0;
    }
    private:
    int Test(const int& channel, int colorCode, int alpha){
        //Todo : Convert colorCode and alpha to status object

        //Todo : Use sendAll to send information to hardware

        // Do we use LED_buf? Don't think it's necessary...
        // They should use setDancer before this command. Should we check that?

        cout<<"Doing LED test:\n";
        cout<<"Channel: "<<channel<<"\n";
        cout<<"ColorCode: "<<colorCode<<"\n";
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