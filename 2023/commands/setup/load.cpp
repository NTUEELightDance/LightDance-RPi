//load given json file to a BIN file
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "nlohmann/json.hpp"
#include "player.h"
// #include <boost/serialization/list.hpp>

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Error: missing parameters! (input type, dancer name, json file are needed)" << endl;
        exit(0); // or 1?
    }

    string dataType = argv[1];
    string dancerName = argv[2];
    string path = argv[3];
    ifstream infile(path.c_str());
    if (!infile) {
        cout << "Cannot open file: " << path << endl;
        exit(0);
    }

    json jsonFile;
    infile >> jsonFile;
    infile.close();

    // cout<<jsonFile["OFPARTS"]["Arm_L"]<<endl;
    if(dataType.compare("dancer")==0){
        Player dancer;
        dancer.setDancer(dancerName, jsonFile);
        cout<<"Date set as below\n";
        cout<<dancer.list()<<endl;

        string ofileName(boost::archive::tmpdir());
        ofileName += "/data/"+dancerName+"_DancerData";
        savePlayer(dancer,ofileName.c_str());
    }


    // string fileName(boost::archive::tmpdir());
    // fileName += "/data/"+dancerName+"_DancerData";
    // Player new_dancer;
    // if(!restorePlayer(new_dancer,fileName.c_str())){
    //     exit(0);
    // };

    // cout<<"Date restore as below\n";
    // cout<<new_dancer<<endl;

    return 0;
}