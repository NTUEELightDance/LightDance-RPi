#include "../inc/rpiMgr.h"

RPiMgr::RPiMgr(const string &dancerName, zmq::socket_t& socket):_dancerName(dancerName),_socket(socket){
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
    // TODO
    return true;
}

void RPiMgr::pause(){
    _playing = false;
}

void RPiMgr::load(const string& path){
    string msg = "Loading " + path + "\n";
    send_str(_socket, msg);
    cout << msg;
    ifstream infile(path.c_str());
    if (!infile){
        msg = "Error: failed cannot open file " + path + "\n";
        send_str(_socket, msg);
        cout << msg;
        return;
    }
    infile >> _ctrlJson;
    _loaded = true;
    msg = "success";
    send_str(_socket, msg);
    cout << msg;
}

void RPiMgr::play(bool givenStartTime, unsigned start, unsigned delay){
    // TODO
    return;
}

void RPiMgr::stop(){
    // TODO
    return;
}

void RPiMgr::statuslight(){
    // TODO
    return;
}

void RPiMgr::eltest(int id, unsigned brightness){
    // TODO
    return;
}

void RPiMgr::ledtest(){
    // TODO
    return;
}

void RPiMgr::list(){
    // TODO
    return;
}

void RPiMgr::quit(){
    // TODO
    return;
}