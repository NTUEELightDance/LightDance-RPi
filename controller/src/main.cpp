#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "cmdMgr.h"
#include "rpiMgr.h"
#include "utils.hpp"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "[Error] missing parameters (dancerName)" << endl;
        exit(0);
    }
    string dancerName = argv[1];
    RPiMgr *rpiMgr = new RPiMgr(dancerName);

    // TODO: init and register commands

    bool quit = false;
    while (!quit)
    {
        string inp;
        getline(cin, inp);
        transform(inp.begin(), inp.end(), inp.begin(), ::tolower); // turn input to lowercase
        vector<string> cmd = splitStr(inp);

        if (cmd.size() < 1)
            continue;

        // TODO: Below are too messy, should try a better way

        // load [control_path]
        if (cmd[0] == "load")
        {
            if (cmd.length > 1)
                rpiMgr->load(cmd[1]);
            else
                rpiMgr->load();
        }

        // play [start_time] [system_time]
        else if (cmd[0] == "play")
        {
            rpiMgr->play(0, 0); // TODO: need to get cmd arguments
        }

        // stop
        else if (cmd[0] == "stop")
        {
            rpiMgr->stop();
        }

        // statuslight
        else if (cmd[0] == "statuslight")
        {
            rpiMgr->statuslight();
        }

        // below won't send to server
        else if (cmd[0] == "eltest")
        {
            rpiMgr->eltest(); // TODO: need to get cmd arguments
        }
        else if (cmd[0] == "ledtest")
        {
            rpiMgr->ledtest(); // TODO: need to get cmd arguments
        }
        else if (cmd[0] == "list")
        {
        }
        else if (cmd[0] == "quit")
        {
            quit = true;
        }
    }
    return 0;
}