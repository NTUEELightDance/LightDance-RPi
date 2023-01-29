#include "command.h"

void Command::OFtest(const int& channel, Status status) {

    vector<Status> OF_buf(OF_SIZE, Status(0, 0, 0, 0));

    OF_buf[channel] = status;

    OPT_CTRL.sendAll(OF_buf);
    return;
}
