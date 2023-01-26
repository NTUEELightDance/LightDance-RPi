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
    if (argc != 3) {
        cerr << "Error: missing parameters! (input type, json file are needed)" << endl;
        exit(1);
    }

    string dataType = argv[1];

    // string dancerName = argv[2];
    // string path = argv[3];

    string path = argv[2];
    if( path.length() < 6){
        cout << "Invalid file name: " << argv[2];
        cout << "\n.json file is needed!"<< endl;
        exit(1);
    }
    if( path.compare(path.length()-5,5,".json")!=0){
        cout << "Invalid file name: " << argv[2];
        cout << "\n.json file is needed!"<< endl;
        exit(1);
    }

    ifstream infile(path.c_str());
    if (!infile) {
        cout << "Cannot open file: " << path << endl;
        exit(1);
    }
    size_t fileNameStart = path.find_last_of("/\\")+1;
    string dancerName=path.substr(fileNameStart,path.find_last_of(".")-fileNameStart);
    // cout<< dancerName<<' '<<fileNameStart<<" "<<path.find_last_of(".")<<endl;
    json jsonFile;
    infile >> jsonFile;
    infile.close();

    // cout<<jsonFile["OFPARTS"]["Arm_L"]<<endl;
    if(dataType.compare("dancer")==0){
        Player dancer;
        dancer.setDancer(dancerName, jsonFile);
        cout<<"Data set as below\n";
        cout<<dancer.list()<<endl;

        string ofileName(boost::archive::tmpdir());
        ofileName += "/data/"+dancerName+"_DancerData";
        cout << ofileName<<endl;
        savePlayer(dancer,ofileName.c_str());
    }


    // string fileName(boost::archive::tmpdir());
    // fileName += "/data/"+dancerName+"_DancerData";
    // Player new_dancer;
    // if(!restorePlayer(new_dancer,fileName.c_str())){
    //     exit(0);
    // };

    // cout<<"Data restore as below\n";
    // cout<<new_dancer<<endl;

    return 0;
}