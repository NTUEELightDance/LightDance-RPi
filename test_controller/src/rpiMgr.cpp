#include "../inc/rpiMgr.h"

RPiMgr::RPiMgr(const string &dancerName):_dancerName(dancerName){
    ifstream infile("./asset/LED.json", ios::in);
    if (!infile)
        cerr << "Error: cannot open ./asset/LED.json" << endl;
    else {
        infile >> LEDJson;
        for (json::iterator it = LEDJson.begin(); it != LEDJson.end(); ++it){
            for (json::iterator it2 = it.value().begin(); it2 != it.value().end(); ++it2){
                json j;
                for (int i = 0; i < it2.value().size(); ++i){
                    uint8_t B = ((unsigned long)it2.value()[i]) % 256;
                    uint8_t G = (((unsigned long)it2.value()[i]) >> 8) % 256;
                    uint8_t R = (((unsigned long)it2.value()[i]) >> 16) % 256;
					j.push_back(R);
					j.push_back(G);
					j.push_back(B);
                }
                it2.value() = j;
            }
        }
        infile.close();
    }
}

bool RPiMgr::setDancer(){
    
}

void RPiMgr::pause(){
    _playing = false;
}