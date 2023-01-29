#include "command.h"

void Command::LEDtest(const int& channel, Status status) {
    //應該有更方便取得shape的方法...
    vector<vector<Status> > LED_buf(LED_SIZE, vector<Status> ());
    for (auto i:LEDPARTS){
        LED_buf[i.second.first].resize(i.second.second);
    }
    int len = LED_buf[channel].size();
    for (int i=0;i<len;i++){
        LED_buf[channel][i] = status;
    }
    if (LED_CTRL.checkReady() == 1){
        LED_CTRL.sendAll(LED_buf);
    } else{
        printf("LED not ready...\n");
    }
    return;
}