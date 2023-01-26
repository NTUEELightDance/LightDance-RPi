#include "player.h"

bool Player::setDancer(const string& _dancerName,json& dancerData){
    //name
    dancerName = _dancerName;
    // FPS

    fps = int(dancerData["fps"]);

    // LED
    LEDPARTS.clear();
    for (json::iterator it = dancerData["LEDPARTS"].begin(); it != dancerData["LEDPARTS"].end(); ++it){
        LEDStripeSetting newSetting;
        newSetting.len=it.value()["len"];
        newSetting.id=it.value()["id"];
        LEDPARTS[it.key()] = newSetting;
    
    }

    // OF
    
    OFPARTS.clear();
    for (json::iterator it = dancerData["OFPARTS"].begin(); it != dancerData["OFPARTS"].end(); ++it)
        OFPARTS[it.key()] = it.value();

    // hardware

    return true;
}

string Player::list()const{
    stringstream ostr;
    ostr<< "******************************\ndancerName:  "<<dancerName<<"\n";
    ostr<<"fps:  "<<fps<<"\n";
    ostr<<"OFPARTS:\n\n";
    for (map<string, int >::const_iterator it = OFPARTS.begin();it != OFPARTS.end(); ++it){
        string part = "    ";
        part +=it->first;
        for (int i = 0; i < max(short(20 - it->first.size()),(short) 0); ++i){
            part +=' ';
        }
        part +=": "+to_string(it->second)+",\n";
        ostr<<part;
    }

    ostr<<"\nLEDPARTS:\n\n";
    for (map<string, LEDStripeSetting >::const_iterator it = LEDPARTS.begin(); it != LEDPARTS.end(); ++it){
        ostr<<it->first+":{\n    id: "+to_string(it->second.id)+",\n    len: "+to_string(it->second.len)+"\n}\n";
    }
    ostr<<"******************************\0";
    
    return ostr.str();
}

template<class Archive>
void Player::serialize(Archive & archive, const unsigned int version){
    archive & dancerName;
    archive & fps;
    archive & OFPARTS;
    archive & LEDPARTS;
};


ostream & operator<<(ostream &os, const Player &player)
{
    os<<player.list();
    return os;
}

void savePlayer(const Player &savePlayer, const char * filename){
    // make an archive
    ofstream ofs(filename);
    if(!ofs){
        cerr<<"File Not Found! ( "<<filename<<" )"<<endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa << savePlayer;
}

bool restorePlayer(Player &savePlayer, const char * filename)
{
    // open the archive
    ifstream ifs(filename);
    if(!ifs){
        cerr<<"File Not Found! ( "<<filename<<" )"<<endl;
        return false;
    }
    boost::archive::text_iarchive ia(ifs);

    // restore the schedule from the archive
    ia >> savePlayer;

    return true;

}