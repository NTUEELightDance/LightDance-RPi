#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <signal.h>
#include <setjmp.h>

#include "cmdMgr.h"
#include "rpiMgr.h"
#include "utils.h"

using namespace std;

jmp_buf env;
RPiMgr *rpiMgr;

void sigHandler(int sig)
{
    cout << endl;
    cout << "pause success" << endl;
    if (rpiMgr) rpiMgr->pause();
    //longjmp(env, 0);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "[Error] missing parameters (dancerName)" << endl;
        exit(0);
    }
    string dancerName = argv[1];
    rpiMgr = new RPiMgr(dancerName);

    // TODO: init and register commands
    if (!rpiMgr->setDancer())
    {
        cerr << "Error: cannot load " << dancerName << ".json" << endl;
        exit(0);
    }
    cout << "start success" << endl;
    signal(SIGINT, sigHandler);
    //setjmp(env);
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
            if (cmd.size() > 1)
                rpiMgr->load(cmd[1]);
            else
                rpiMgr->load();
        }

        // play [start_time] [delay_time]
        else if (cmd[0] == "play")
        {
            // TODO: need to get cmd arguments

            //play
            if (cmd.size() == 1)
                rpiMgr->play(false, 0);

            //play start_time
            else if (cmd.size() == 2)
            {
                unsigned startTime;
                if (!Str2Unsint(cmd[1], startTime))
                    cerr << "Error: illegal option \"" << cmd[1] << "\"" << endl;
                else
                    rpiMgr->play(true, startTime);
            }

            //play start_time delay_time
            else if (cmd.size() == 3)
            {
                unsigned startTime, delayTime;
                if (!Str2Unsint(cmd[1], startTime))
                {
                    cerr << "Error: illegal option \"" << cmd[1] << "\"" << endl;
                    continue;
                }
                if (!Str2Unsint(cmd[2], delayTime))
                {
                    cerr << "Error: illegal option \"" << cmd[2] << "\"" << endl;
                    continue;
                }
                //cout << "play " << startTime << " " << delayTime << endl;
                rpiMgr->play(true, startTime, delayTime);
            }
            //play start_time delay_time system_time
            else
            {
                unsigned startTime;
		long systemTime;
                if (!Str2Unsint(cmd[1], startTime))
                {
                    cerr << "Error: illegal option \"" << cmd[1] << "\"" << endl;
                    continue;
                }
                if (!Str2LongInt(cmd[3], systemTime))
                {
                    cerr << "Error: illegal option \"" << cmd[3] << "\"" << endl;
                    continue;
                }

                long nowTime = getsystime();
                if (systemTime < nowTime)
                {
                    cerr << "Error: the given systemTime(" << systemTime << ") is before systemTime now(" << nowTime << ")" << endl;
                    continue;
                }
                rpiMgr->play(true, startTime, systemTime - nowTime);
            }
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
            // eltest (default all light)
            if (cmd.size() == 1) {
                rpiMgr->eltest(-1, 4095);	    
            }
            // eltest id brightness
            else if (cmd.size() >= 3){
                int id = stoi(cmd[1]);
                cout << "testing id: " << id << endl;
                unsigned brightness;
                /* if (!Str2Unsint(cmd[1], id)) {
                        cerr << "Error: illegal option \"" << cmd[1] << "\"" << endl;
                continue;
            }*/
                if (!Str2Unsint(cmd[2], brightness)) {
                    cerr << "Error: illegal option \"" << cmd[2] << "\"" << endl;
                    continue;
                }
                rpiMgr->eltest((int)id, brightness);

            }
            else {
                cerr << "Error: missing options, should give id(0~31) and brightness(0~4095)" << endl;
                continue;
	        }
	    // TODO: need to get cmd arguments
        }
        else if (cmd[0] == "ledtest")
        {
            rpiMgr->ledtest(); // TODO: need to get cmd arguments
        }
        else if (cmd[0] == "list")
        {
            rpiMgr->list();
        }
        else if (cmd[0] == "quit")
        {
            rpiMgr->quit();
            quit = true;
        }
    }
    return 0;
}
