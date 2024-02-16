#include "player.h"

#include "const.h"

using namespace boost;
string Player::getDancerName() { return dancerName; };

bool Player::setDancer(const string& _dancerName, property_tree::ptree& dancerData) {
    // name
    dancerName = _dancerName;
    // FPS

    fps = dancerData.get<int>("fps");

    // LED

    LEDPARTS.clear();
    for (property_tree::ptree::const_iterator it = dancerData.get_child("LEDPARTS").begin();
        it != dancerData.get_child("LEDPARTS").end(); ++it) {
        LEDStripeSetting newSetting;
        newSetting.len = it->second.get<int>("len");
        newSetting.id = it->second.get<int>("id");
        std::string led_name = it->first;
        LEDPARTS[led_name] = newSetting;
    }
    // for (property_tree::ptree::const_iterator it = dancerData.get_child("LEDPARTS").data().begin();
    //      it != dancerData.get_child("LEDPARTS").data().end(); ++it) {
    //     LEDStripeSetting newSetting;
    //     newSetting.len = it.value()["len"];
    //     newSetting.id = it.value()["id"];
    //     LEDPARTS[it.key()] = newSetting;
    // }

    // OF

    // OFPARTS.clear();
    // for (json::iterator it = dancerData["OFPARTS"].begin();
    //      it != dancerData["OFPARTS"].end(); ++it)
    //     OFPARTS[it.key()] = it.value();

    // hardware

    return true;
}

string Player::list() const {
    stringstream ostr;
    ostr << "******************************\ndancerName:  " << dancerName
        << "\n";
    ostr << "fps:  " << fps << "\n";
    ostr << "OFPARTS:\n\n";
    for (unordered_map<string, int>::const_iterator it = OFPARTS.begin();
        it != OFPARTS.end(); ++it) {
        string part = "    ";
        part += it->first;
        for (int i = 0; i < max(short(20 - it->first.size()), (short)0); ++i) {
            part += ' ';
        }
        part += ": " + to_string(it->second) + ",\n";
        ostr << part;
    }

    ostr << "\nLEDPARTS:\n\n";
    for (unordered_map<string, LEDStripeSetting>::const_iterator it =
        LEDPARTS.begin();
        it != LEDPARTS.end(); ++it) {
        ostr << it->first + ":{\n    id: " + to_string(it->second.id) +
            ",\n    len: " + to_string(it->second.len) + "\n}\n";
    }
    ostr << "******************************\0";

    return ostr.str();
}

template <class Archive>
void Player::serialize(Archive& archive, const unsigned int version) {
    archive& dancerName;
    archive& fps;
    archive& OFPARTS;
    archive& LEDPARTS;
    archive& myLEDPlayer;
    archive& myOFPlayer;
    archive& LEDloaded;
    archive& OFloaded;
};

ostream& operator<<(ostream& ostream, const Player& player) {
    ostream << player.list();
    return ostream;
}

void savePlayer(const Player& savePlayer, const char* filename) {
    // make an archive
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "File Not Found! ( " << filename << " )" << endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa << savePlayer;
}

bool restorePlayer(Player& savePlayer, const char* filename) {
    // open the archive
    ifstream ifs(filename);
    if (!ifs) {
        cerr << "File Not Found! ( " << filename << " )" << endl;
        return false;
    }
    boost::archive::text_iarchive ia(ifs);

    // restore the schedule from the archive
    ia >> savePlayer;
    // savePlayer.myLEDPlayer.init();
    // savePlayer.myOFPlayer.init();

    return true;
}

void LEDload(Player& Player, property_tree::ptree& data_json) {
    vector<vector<LEDFrame>> frameLists;
    vector<int> stripShapes;

    const int partNum = LED_NUM;

    stripShapes.resize(partNum);
    fill(stripShapes.begin(), stripShapes.end(), 0);

    frameLists.clear();
    frameLists.resize(partNum);

    for (unordered_map<string, LEDStripeSetting>::iterator part_it =
        Player.LEDPARTS.begin();
        part_it != Player.LEDPARTS.end(); ++part_it) {
        const string partName = part_it->first;
        const unsigned int id = part_it->second.id;
        const unsigned int len = part_it->second.len;

        stripShapes[id] = len;
        auto check = data_json.get_child_optional(partName);
        if (!check) {
            cerr << "Error in LED data: " << partName << " not found" << endl;
        }
        const auto darkFrame = LEDFrame(0, false, len);
        // If no frames are given, push only dark frame

        if (check->size() == 0) {
            cerr << "warning" << partName << " has no frames" << endl;
            frameLists[id].push_back(darkFrame);
            continue;
        }
        // If not starting at time = 0, use dark frame as first frame
        const int firstStartTime = data_json.get_child(partName).front().second.get<int>("start");
        if (firstStartTime != 0) {
            frameLists[id].push_back(darkFrame);
        }

        vector<LEDStatus> statusList;
        statusList.clear();

        for (property_tree::ptree::value_type& frame_data : data_json.get_child(partName)) {
            property_tree::ptree frame_json = frame_data.second;
            if (!frame_json.get_child_optional("status")) {
                cerr << "Error in LED data: " << partName << " has no status key" << endl;
            }
            if (frame_json.get_child("status").size() != len) {
                cerr << "Error in LED data: " << partName << " has size mismatch" << endl;
            }
            statusList.clear();
            for (property_tree::ptree::value_type& status_data : frame_json.get_child("status")) {
                property_tree::ptree status_json = status_data.second;
                auto it = status_json.begin();
                statusList.push_back(LEDStatus(
                    it->second.get_value<int>(),
                    (++it)->second.get_value<int>(),
                    (++it)->second.get_value<int>(),
                    (++it)->second.get_value<int>()));
            }
            frameLists[id].push_back(LEDFrame(frame_json.get<int>("start"), frame_json.get<bool>("fade"), statusList));
        }


        Player.myLEDPlayer = LEDPlayer(Player.fps, frameLists, stripShapes);
        Player.LEDloaded = true;

        vector<vector<int>>& currentStatus = Player.myLEDPlayer.currentStatus;
        currentStatus.resize(partNum);
        for (int i = 0; i < partNum; i++) {
            currentStatus[i].resize(stripShapes[i]);
            fill(currentStatus[i].begin(), currentStatus[i].end(), 0);
        }
    }
}

void OFload(Player& Player, property_tree::ptree& data_json) {
    vector<OFFrame> frameLists;
    vector<vector<OFStatus>> statusList;

    // TODO: load from .h file, instead of hard coded
    const int partNum = OF_NUM;

    frameLists.clear();
    statusList.clear();

    const OFFrame darkFrame;
    const OFStatus darkStatus;


    for (property_tree::ptree::value_type& frame_data : data_json) {
        property_tree::ptree frame_json = frame_data.second;
        vector<OFStatus> status;
        status.clear();
        status.resize(partNum);
        fill(status.begin(), status.end(), darkStatus);  // fill 0 to every body part
        vector<pair<string, OFStatus>> frameStatus;
        frameStatus.clear();
        if (frame_json.size() == 0) {
            // If no frames are given, push only dark frame
            // frameLists.push_back(darkFrame);
            // statusList.push_back(status);
            cerr << "warning: OF has no frames" << endl;
            continue;
        }
        const property_tree::ptree& status_json = frame_json.get_child("status");
        for (unordered_map<string, int>::iterator part_it = Player.OFPARTS.begin(); part_it != Player.OFPARTS.end(); ++part_it) {
            const string partName = part_it->first;
            const int id = part_it->second;
            auto check = status_json.get_child_optional(partName);
            if (!check) {
                cerr << "Error in OF data: " << partName << " not found" << endl;
            }
            auto it = status_json.get_child(partName).begin();
            const OFStatus currentStatus(
                it->second.get_value<int>(),
                (++it)->second.get_value<int>(),
                (++it)->second.get_value<int>(),
                (++it)->second.get_value<int>());
            status[id] = currentStatus;
            frameStatus.push_back(make_pair(partName, currentStatus));
        }
        OFFrame currentFrame(frame_json.get<int>("start"), frame_json.get<bool>("fade"), frameStatus);
        frameLists.push_back(currentFrame);
        statusList.push_back(status);
    }

    Player.myOFPlayer =
        OFPlayer(Player.fps, frameLists, statusList, Player.OFPARTS, partNum);
    Player.OFloaded = true;

    vector<int>& currentStatus = Player.myOFPlayer.currentStatus;
    currentStatus.resize(partNum);
    fill(currentStatus.begin(), currentStatus.end(), 0);
}
