#include "command.h"
#include "const.h"
#include "player.h"

class FrameTest: public Command{
    public:
    FrameTest(): Command(){
        addFlag("-h", "help");
        // addFlag("--ms", "milliseconds");
    }
    int execute(int argc, char* argv[]){
        // cout<<argc<<"\n";
        if (argc <= 1 || cmdOptionExists(argv, argv + argc, "-h")) {
            cout<<"./frametest <time>\n(in milliseconds)\n";
            help();
            return 0;
        }
        int ms = atoi(argv[1]);
        // int ms = 0;
        // if (cmdOptionExists(argv, argv + argc, "--ms")) {
        //     vector<int> _ms = getCmdOptionInt(argv, argv + argc, "--ms");
        //     if (_ms.size() > 0){
        //         ms = _ms[0];
        //     }
        // } 
        return Test(ms);
    }
    private:

    int Test(int ms){
        Player player;
        string path = string(BASE_PATH) + "data/dancer.dat";
        if (!restorePlayer(player, path.c_str())) {
            cout<<"Need to load first!\n";
            return 0;
        }


        if (player.LEDloaded) {
            // for (auto frameList: player.myLEDPlayer.frameLists){
            //     for (auto frame:frameList){
            //         cout << frame.start << " " << frame.fade << "\n";
            //         for (auto i:frame.statusList) {
            //             cout << i.r << " " << i.g << " " << i.b << " " << i.a <<"\n";
            //         }
            //         cout<<"\n";
            //     }
            // }

            // This part directly copied from play loop
            // I didn't test so maybe there will be some bugs idk

            vector<vector<LEDStatus> > statusLists;
            LEDPlayer &LEDP = player.myLEDPlayer;
            LEDP.init();
            const int fps = LEDP.fps;
            const long elapsedTime = (long)ms * 1000;
            const int currentTimeId = LEDP.getTimeId(elapsedTime);

            LEDP.calculateFrameIds(currentTimeId);

            statusLists.clear();
            for (int i = 0; i < LEDP.frameIds.size(); i++) {
                const int &frameId = LEDP.frameIds[i];
                const vector<LEDFrame> &frameList = LEDP.frameLists[i];

                if (frameId < 0) {
                    // use dark frame for invalid time or non-exist part
                    statusLists.push_back(vector<LEDStatus>(LEDP.stripShapes[i]));
                    continue;
                }

                const LEDFrame &frame = frameList[frameId];
                if (frame.fade) {
                    const long startTime = (long)frameList[frameId].start * 1000000l / (long)fps;
                    const long endTime = (long)frameList[frameId + 1].start * 1000000l / (long)fps;
                    const float rate = (float)(elapsedTime - startTime) / (float)(endTime - startTime);
                    statusLists.push_back(LEDP.interpolateFadeFrame(frame, frameList[frameId + 1], rate));
                } else {
                    statusLists.push_back(frameList[frameId].statusList);
                }
            }

            LEDP.controller.sendAll(LEDP.castStatusLists(statusLists));
        
        } else{
            cout<<"LED not loaded yet!\n";
        }
        if (player.OFloaded) {
            // for (auto frame: player.myOFPlayer.frameList){
            //     cout << frame.start << " " << frame.fade << "\n";
            //     for (auto i: frame.statusList) {
            //         cout << i.first << " " << i.second.r << " " << i.second.g << " " << i.second.b << " " << i.second.a <<"\n";
            //     }
            //     cout<<"\n";
            // }
            // for (auto statuslist: player.myOFPlayer.statusList) {
            //     for(auto i: statuslist) {
            //         cout << i.r << " " << i.g << " " << i.b << " " << i.a <<"\n";
            //     }
            //     cout<<"\n";
            // }

            vector<OFStatus > statusList;
            OFPlayer &OFP = player.myOFPlayer;
            OFP.frameId = 0;
            const long elapsedTime = (long)ms * 1000;

            statusList = OFP.findFrameStatus(elapsedTime / 1000l);

            OFP.controller.sendAll(OFP.castStatusList(statusList));

        } else{
            cout<<"OF not loaded yet!\n";
        }

        return 0;
    }
};

int main(int argc, char* argv[]){
    FrameTest test;
    return test.execute(argc, argv);
}

