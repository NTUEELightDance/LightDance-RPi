// load given json file to a BIN file
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "player.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Error: missing parameters! (dancerName is needed)" << endl;
        exit(1);
    }

    /*
    in
    */
    string dancerName = argv[1];
    string fileName(boost::archive::tmpdir());
    fileName += "/data/" + dancerName + "_DancerData";
    Player new_dancer;
    if (!restorePlayer(new_dancer, fileName.c_str())) {
        exit(1);
    };
    cout << dancerName << "'s data restore as below\n";
    cout << new_dancer << endl;

    return 0;
}