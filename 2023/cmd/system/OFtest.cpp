#include "command.h"

class OFtest: public Command{
    public:
    OFtest(): Command(){
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
                cout<<"Error: OFtest expect 3 arguments (Channel ColorCode Alpha), got " << options.size() << ".\n";
                return 0;
            }
        }
        return 0;
    }
    private:
    int Test(const int& channel, int colorCode, int alpha){
        //Todo : Convert colorCode and alpha to status object

        //Todo : Use sendAll to send information to hardware

        cout<<"Doing OF test:\n";
        cout<<"Channel: "<<channel<<"\n";
        cout<<"ColorCode: "<<colorCode<<"\n";
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

