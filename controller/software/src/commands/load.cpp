// load given json file to a BIN file
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "LEDPlayer.h"
#include "const.h"
#include "player.h"

/*
"load" help file

---- load <Data Type> <Data Path> ----

load given JSON file to a BIN file ( dancer.dat )

Data Type ( optional ) :
    If not assigned, it will load all three data by their default path
    Supported Types:
    - control: dancer's pin mapping
    - LED: LED light frame
    - OF: OF light frame
Data Path ( optional ) :
    Must be assigned to a specific Data Type to load from a specific path
    If not assigned, it will load from the below default path.
    - control: data/control.json
    - LED: data/LED.json
    - OF: data/control.json

i.e. All support commands are listed below:
    $ load
    $ load control
    $ load LED
    $ load OF
    $ load control <path.json>
    $ load LED <path.json>
    $ load OF <path.json>

some ERROR messages might occur:

1. Unsupported datatype :
    Please enter the correct types, like control, LED, and OF.

2. Invalid file name    :
    Please enter .json file only.

3. Cannot open file     :
    The folder "data"(or the folder you provide) might not be made.

4. Please Load the dancer's control data first :
    When loading LED or OF, you Load control data first.

5."terminate called after throwing an instance of
'nlohmann::detail::type_error'..." : The data type and file you provided might
have a mismatch. (e.g. load LED OF.json).

*/
using namespace std;

int main(int argc, char *argv[]) {
    setuid(1000);
    bool loadControl = false, loadLED = false, loadOF = false;

    string basePath = BASE_PATH;
    string controlPath = basePath + "data/control.json";
    string LEDpath = basePath + "data/LED.json";
    string OFpath = basePath + "data/OF.json";

    string dataType;
    if (argc >= 2) {
        dataType = argv[1];
        if (dataType.compare("control") == 0)
            loadControl = true;
        else if (dataType.compare("LED") == 0)
            loadLED = true;
        else if (dataType.compare("OF") == 0)
            loadOF = true;
        else {
            cerr << "Unsupported datatype " << argv[1];
            exit(1);
        }
    } else {
        dataType = "all";
        loadControl = true;
        loadLED = true;
        loadOF = true;
    }

    string path = "NONE";
    if (argc == 3) {
        path = argv[2];
        if (path.length() < 6) {
            cerr << "Invalid file name: " << argv[2];
            cerr << "\n.json file is needed!" << endl;
            exit(1);
        }
        if (path.compare(path.length() - 5, 5, ".json") != 0) {
            cerr << "Invalid file name: " << argv[2];
            cerr << "\n.json file is needed!" << endl;
            exit(1);
        }
    }
    if (path.compare("NONE") != 0) {
        if (loadControl) controlPath = path;
        if (loadLED) LEDpath = path;
        if (loadOF) OFpath = path;
    }

    // size_t fileNameStart = path.find_last_of("/\\") + 1;
    // string dancerName = path.substr(fileNameStart, path.find_last_of(".") -
    // fileNameStart);
    string dancerName = "DANCER";
    boost::property_tree::ptree controlJsonFile;
    if (loadControl) {
        ifstream infile(controlPath.c_str());
        if (!infile) {
            cerr << "Cannot open file: " << controlPath << endl;
            exit(1);
        }
        boost::property_tree::read_json(infile, controlJsonFile);
        infile.close();
    }
    boost::property_tree::ptree LEDJsonFile;
    if (loadLED) {
        ifstream infile(LEDpath.c_str());
        if (!infile) {
            cerr << "Cannot open file: " << LEDpath << endl;
            exit(1);
        }
        boost::property_tree::read_json(infile, LEDJsonFile);
        infile.close();
    }
    boost::property_tree::ptree OFJsonFile;
    if (loadOF) {
        ifstream infile(OFpath.c_str());
        if (!infile) {
            cerr << "Cannot open file: " << OFpath << endl;
            exit(1);
        }
        boost::property_tree::read_json(infile, OFJsonFile);
        infile.close();
    }

    string outputFileName = basePath + "data/dancer.dat";
    if (loadControl) {
        
        boost::property_tree::write_json(std::cout, controlJsonFile);

        Player dancer;
        dancer.setDancer(dancerName, controlJsonFile);
        cout << "Dancer control data loaded successfully\n";
        cout << "Stored at: " << outputFileName << endl;
        savePlayer(dancer, outputFileName.c_str());

        // cout << "dancer Control data set as below\n";
        // cout << dancer.list() << endl;
        // return 0;
    }

    Player dancerData;
    // if (!restorePlayer(dancerData, outputFileName.c_str())) {
    //     cerr << "Please load dancer's control data first !" << endl;
    //     exit(1);
    // };
    if (loadLED) {
        boost::property_tree::write_json(std::cout, LEDJsonFile);
        LEDload(dancerData, LEDJsonFile);
        cout << "LED data loaded successfully\n";
        cout << "Stored at: " << outputFileName << endl;
        savePlayer(dancerData, outputFileName.c_str());

        // Player clarifyPlayer;
        // restorePlayer(clarifyPlayer, outputFileName.c_str());
        // cout << clarifyPlayer.myLEDPlayer;
        // return 0;
    }
    if (loadOF) {
        boost::property_tree::write_json(std::cout, OFJsonFile);
        OFload(dancerData, OFJsonFile);
        cout << "OF data loaded successfully\n";
        cout << "Stored at: " << outputFileName << endl;
        savePlayer(dancerData, outputFileName.c_str());

        // Player clarifyPlayer;
        // restorePlayer(clarifyPlayer, outputFileName.c_str());
        // cout << clarifyPlayer.myOFPlayer;
        // return 0;
    }
    return 0;
}
