// load given json file to a BIN file
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"
#include "player.h"
// #include <boost/serialization/list.hpp>

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Error: missing parameters! (input type, json file are needed)" << endl;
        exit(1);
    }

    string dataType = argv[1];

    // string dancerName = argv[2];
    // string path = argv[3];

    string path = argv[2];
    if (path.length() < 6) {
        cout << "Invalid file name: " << argv[2];
        cout << "\n.json file is needed!" << endl;
        exit(1);
    }
    if (path.compare(path.length() - 5, 5, ".json") != 0) {
        cout << "Invalid file name: " << argv[2];
        cout << "\n.json file is needed!" << endl;
        exit(1);
    }

    ifstream infile(path.c_str());
    if (!infile) {
        cout << "Cannot open file: " << path << endl;
        exit(1);
    }
    size_t fileNameStart = path.find_last_of("/\\") + 1;
    string dancerName = path.substr(fileNameStart, path.find_last_of(".") - fileNameStart);
    json jsonFile;
    infile >> jsonFile;
    infile.close();

    if (dataType.compare("dancer") == 0) {
        Player dancer;
        dancer.setDancer(dancerName, jsonFile);
        cout << "Data set as below\n";
        cout << dancer.list() << endl;

        string ofileName("./data/");
        ofileName += dancerName + "_DancerData.dat";
        cout << "stored at: " << ofileName << endl;
        savePlayer(dancer, ofileName.c_str());
    }

    return 0;
}