#include "player.h"

#include "const.h"
string Player::getDancerName() { return dancerName; };

bool Player::setDancer(const string &_dancerName, json &dancerData) {
    // name
    dancerName = _dancerName;
    // FPS

    fps = int(dancerData["fps"]);

    // LED
    LEDPARTS.clear();
    for (json::iterator it = dancerData["LEDPARTS"].begin();
         it != dancerData["LEDPARTS"].end(); ++it) {
        LEDStripeSetting newSetting;
        newSetting.len = it.value()["len"];
        newSetting.id = it.value()["id"];
        LEDPARTS[it.key()] = newSetting;
    }

    // OF

    OFPARTS.clear();
    for (json::iterator it = dancerData["OFPARTS"].begin();
         it != dancerData["OFPARTS"].end(); ++it)
        OFPARTS[it.key()] = it.value();

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
void Player::serialize(Archive &archive, const unsigned int version) {
    archive &dancerName;
    archive &fps;
    archive &OFPARTS;
    archive &LEDPARTS;
    archive &myLEDPlayer;
    archive &myOFPlayer;
    archive &LEDloaded;
    archive &OFloaded;
};

ostream &operator<<(ostream &ostream, const Player &player) {
    ostream << player.list();
    return ostream;
}

void savePlayer(const Player &savePlayer, const char *filename) {
    // make an archive
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "File Not Found! ( " << filename << " )" << endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa << savePlayer;
}

bool restorePlayer(Player &savePlayer, const char *filename) {
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

void LEDload(Player &Player, json &data_json) {
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

        const json &frames_json = data_json[partName];

        stripShapes[id] = len;

        // If no frames are given, push only dark frame
        const LEDFrame darkFrame(0, false, len);
        if (frames_json.size() == 0) {
            frameLists[id].push_back(darkFrame);
            continue;
        }

        // If not starting at time = 0, use dark frame as first frame
        const int firstStartTime = frames_json.begin().value()["start"];
        if (firstStartTime != 0) {
            frameLists[id].push_back(darkFrame);
        }

        vector<LEDStatus> statusList;
        for (const json &frame_json : frames_json) {
            if (frame_json["status"].size() != len) {
                // TODO: print size mismatch warning
                continue;
            }
            statusList.clear();
            for (auto &status_json : frame_json["status"]) {
                statusList.push_back(LEDStatus(status_json[0], status_json[1],
                                               status_json[2],
                                               (int)status_json[3]));
            }

            frameLists[id].push_back(
                LEDFrame(frame_json["start"], frame_json["fade"], statusList));
        }
    }

    Player.myLEDPlayer = LEDPlayer(Player.fps, frameLists, stripShapes);
    Player.LEDloaded = true;

    vector<vector<int>> &currentStatus = Player.myLEDPlayer.currentStatus;
    currentStatus.resize(partNum);
    for (int i = 0; i < partNum; i++) {
        currentStatus[i].resize(stripShapes[i]);
        fill(currentStatus[i].begin(), currentStatus[i].end(), 0);
    }
}

void OFload(Player &Player, json &data_json) {
    vector<OFFrame> frameLists;
    vector<vector<OFStatus>> statusList;

    // TODO: load from .h file, instead of hard coded
    const int partNum = OF_NUM;

    frameLists.clear();
    statusList.clear();

    const OFFrame darkFrame;
    const OFStatus darkStatus;

    for (const json &frame_json : data_json) {
        vector<OFStatus> status;
        status.clear();
        status.resize(partNum);
        fill(status.begin(), status.end(),
             darkStatus);  // fill 0 to every body part
        vector<pair<string, OFStatus>> frameStatus;
        frameStatus.clear();
        if (frame_json.empty()) {
            // If no frames are given, push only dark frame
            // frameLists.push_back(darkFrame);
            // statusList.push_back(status);
            continue;
        }
        const json &status_json = frame_json["status"];
        for (unordered_map<string, int>::iterator part_it =
                 Player.OFPARTS.begin();
             part_it != Player.OFPARTS.end(); ++part_it) {
            const string partName = part_it->first;
            const int id = part_it->second;

            const OFStatus currentStatus(
                status_json[partName][0], status_json[partName][1],
                status_json[partName][2], (int)status_json[partName][3]);
            status[id] = currentStatus;
            frameStatus.push_back(make_pair(partName, currentStatus));
        }
        OFFrame currentFrame(frame_json["start"], frame_json["fade"],
                             frameStatus);
        frameLists.push_back(currentFrame);
        statusList.push_back(status);
    }

    Player.myOFPlayer =
        OFPlayer(Player.fps, frameLists, statusList, Player.OFPARTS, partNum);
    Player.OFloaded = true;

    vector<int> &currentStatus = Player.myOFPlayer.currentStatus;
    currentStatus.resize(partNum);
    fill(currentStatus.begin(), currentStatus.end(), 0);
}
