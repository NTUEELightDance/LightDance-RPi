#include "OFPlayer.h"

// ==================== OFStatus ============================

template <class Archive>
void OFStatus::serialize(Archive &archive, const unsigned int version) {
    archive &r;
    archive &g;
    archive &b;
    archive &a;
}

OFStatus::OFStatus() : r(0), g(0), b(0), a(0) {}

OFStatus::OFStatus(const int &_r, const int &_g, const int &_b, const int &_a)
    : r(_r), g(_g), b(_b), a(_a) {}

// ==================== OFFrame ============================

template <class Archive>
void OFFrame::serialize(Archive &archive, const unsigned int version) {
    archive &start;
    archive &fade;
    archive &statusList;
}

OFFrame::OFFrame() : start(0), fade(false) {}

OFFrame::OFFrame(const int &_start, const bool &_fade,
                 const vector<pair<string, OFStatus>> &_statusList) {
    start = _start;
    fade = _fade;
    statusList.clear();
    // append all status
    statusList.assign(_statusList.begin(), _statusList.end());
}

// ====================== OFPlayer ===============================

template <class Archive>
void OFPlayer::serialize(Archive &archive, const unsigned int version) {
    archive &fps;
    archive &frameList;
    archive &statusList;
    archive &channelIds;
}

string OFPlayer::list() const {
    stringstream ostr;
    ostr << "*************** OFPlayer ***************\n";
    ostr << "frameList :\n";
    ostr << "[";
    for (auto part_it : frameList) {
        ostr << "\n\t{\n\tstart: " << part_it.start << ",\n\tfade: " << part_it.fade
             << ",\n\tstatus: [\n";
        for (auto status_it : part_it.statusList) {
            ostr << "\t\t" << status_it.first;
            ostr << " : [ " << status_it.second.r << " , " << status_it.second.g << " , "
                 << status_it.second.b << " , " << status_it.second.a << " ],\n";
        }
        ostr << "\t]\n\t},\n";
    }
    ostr << "]\n";
    ostr << "*****************************************\n";

    return ostr.str();
}

ostream &operator<<(ostream &os, const OFPlayer &player) {
    os << player.list();
    return os;
}

void saveOFPlayer(OFPlayer &player, const char *filename) {
    // make an archive
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "File Not Found! ( " << filename << " )" << endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa << player;
}

bool restoreOFPlayer(OFPlayer &player, const char *filename) {
    // open the archive
    ifstream ifs(filename);
    if (!ifs) {
        cerr << "File Not Found! ( " << filename << " )" << endl;
        return false;
    }
    boost::archive::text_iarchive ia(ifs);

    // restore the schedule from the archive
    ia >> player;

    return true;
}

OFPlayer::OFPlayer() : fps(0){};

OFPlayer::OFPlayer(const int &_fps, const vector<OFFrame> &_frameList,
                   const vector<vector<OFStatus>> &_statusList, unordered_map<string, int> &_channeldIds) {
    fps = _fps;
    frameList.assign(_frameList.begin(), _frameList.end());
    statusList.assign(_statusList.begin(), _statusList.end());
    for (unordered_map<string, int>::iterator part_it = _channeldIds.begin(); part_it!=_channeldIds.end();part_it++){
        channelIds[part_it->first] = part_it->second;
    }

    // TODO: assign channel number to part name
    // TODO: assign status to every part
    // statusList[channel_num] = part_status
    // done in save.cpp
}

long OFPlayer::getElapsedTime(const struct timeval &base, const struct timeval &current) {
    // Get time elapsed from start time in microsecond
    return (long)(current.tv_sec - base.tv_sec) * 1000000l + (long)(current.tv_usec - base.tv_usec);
}

vector<OFStatus> OFPlayer::findFrameStatus(const long &time) {
    frameId = findFrameId(time);
    bool fade = frameList[frameId].fade;
    printf("frameId: %d, fade: %d\n", frameId, fade);
    return fade ? findFadeFrameStatus(time) : statusList[frameId];
}

vector<OFStatus> OFPlayer::findFadeFrameStatus(const long &time) {
    const long startTime = (long)frameList[frameId].start;
    const long endTime = (long)frameList[frameId + 1].start;
    const float rate = (float)(time - startTime) / (float)(endTime - startTime);

    vector<OFStatus> fadeFrameStatus;
    fadeFrameStatus.resize(26);

    // Do interpolate
    for (int i = 0;i < frameList[frameId].statusList.size(); i++) {
        if (frameId+1 >= frameList.size()) {
            printf("Fade for the last frame should be false.");
            break;
        }   
        const string &partName = frameList[frameId].statusList[i].first;
        const OFStatus &status = frameList[frameId].statusList[i].second;
        int channelId = findChannelId(partName);
        const OFStatus &statusNext = frameList[frameId + 1].statusList[i].second;
        const int &r = (int)round((1 - rate) * (float)status.r + rate * (float)statusNext.r);
        const int &g = (int)round((1 - rate) * (float)status.g + rate * (float)statusNext.g);
        const int &b = (int)round((1 - rate) * (float)status.b + rate * (float)statusNext.b);
        const int &a = (int)round((1 - rate) * (float)status.a + rate * (float)statusNext.a);
        fadeFrameStatus[channelId] = OFStatus(r, g, b, a);
    }

    return fadeFrameStatus;
}

int OFPlayer::findFrameId(const long &time) {
    const int totalFrame = frameList.size();
    if (totalFrame == 0 || time <= 0) return 0;
    if (time > frameList[frameList.size() - 1].start) return frameList.size() - 1;
    if (time >= frameList[frameId].start) {
        if (frameId < frameList.size() - 1) {
            if (time < frameList[frameId + 1].start) return frameId;
        }
        if (frameId < frameList.size() - 2) {
            if (time >= frameList[frameId + 1].start && time < frameList[frameId + 2].start)
                return frameId + 1;
        }
    }

    int first = 0;
    int last = totalFrame - 1;
    while (first <= last) {
        const int mid = (first + last) >> 1;
        if (time > frameList[mid].start)
            first = mid + 1;
        else if (time == frameList[mid].start)
            return mid;
        else
            last = mid - 1;
    }

    if (time < frameList[first].start) --first;
    return first;
}

// store infromation of every part
int OFPlayer::findChannelId(const string &partName) { return channelIds[partName]; }

void OFPlayer::init() {
    // TODO: enable hardware
    //controller.init();
}

vector<int> OFPlayer::castStatusList(vector<OFStatus> statusList) {
    vector<int> castedList(statusList.size());
    for (int i = 0; i < statusList.size(); i++) {
        const OFStatus &status = statusList[i];
        castedList[i] = ((status.r << 24) + (status.g << 16) + (status.b << 8) + (status.a << 0));
    }

    return castedList;
}

void OFPlayer::loop(const bool *playing, const timeval *baseTime, const bool *toTerminate) {
    timeval currentTime;
    vector<OFStatus> statusList;

    while (true) {
        if (*toTerminate) {
            // TODO: finish darkall
            statusList.resize(channelIds.size());
            for (auto &status : statusList) {
                status = OFStatus(0, 0, 0, 0);
            }
            // TODO: enable hardware
            // controller.sendAll(castStatusList(statusList));

            break;
        }
        if (*playing) {
            gettimeofday(&currentTime, NULL);
            const long elapsedTime = getElapsedTime(*baseTime, currentTime);
            statusList.clear();
            // printf("Time: %f\n", elapsedTime / 1000000.0f);

            // find status
            statusList = findFrameStatus(elapsedTime / 1000l);

            printf("%d strips sent: ", (int)statusList.size());
            for (int i = 0; i < statusList.size(); i++) {
                printf("%d ", statusList[i]);
            }
            printf("\n");

            // TODO: enable hardware
            // controller.sendAll(castStatusList(statusList));
        }
    }
}
