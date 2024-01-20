#include "OFPlayer.h"

#include <thread>

#include "utils.h"

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
    archive &OFnum;
    archive &currentStatus;
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
                   const vector<vector<OFStatus>> &_statusList,
                   unordered_map<string, int> &_channeldIds, const int &_OFnum) {
    fps = _fps;
    OFnum = _OFnum;
    frameList.assign(_frameList.begin(), _frameList.end());
    statusList.assign(_statusList.begin(), _statusList.end());
    for (unordered_map<string, int>::iterator part_it = _channeldIds.begin();
         part_it != _channeldIds.end(); part_it++) {
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

// vector<OFStatus> OFPlayer::findFrameStatus(const long &time) {
//     frameId = findFrameId(time);
//     bool fade = frameList[frameId].fade;
//     printf("[OF] frameId: %d, fade: %d\n", frameId, fade);
//     return fade ? findFadeFrameStatus(time) : statusList[frameId];
// }

vector<OFStatus> OFPlayer::findFrameStatus(const long &time) {
    bool fade = frameList[frameId].fade;
    // printf("[OF] frameId: %d, fade: %d\n", frameId, fade);
    return fade ? findFadeFrameStatus(time) : statusList[frameId];
}

vector<OFStatus> OFPlayer::findFadeFrameStatus(const long &time) {
    const long startTime = (long)frameList[frameId].start;
    const long endTime = (long)frameList[frameId + 1].start;
    const float rate = (float)(time - startTime) / (float)(endTime - startTime);

    vector<OFStatus> fadeFrameStatus;
    fadeFrameStatus.resize(OFnum);

    // Do interpolate
    for (unsigned int i = 0; i < frameList[frameId].statusList.size(); i++) {
        if ((unsigned int)frameId + 1 >= frameList.size()) {
            return statusList[frameId];
        }
        const string &partName = frameList[frameId].statusList[i].first;
        const OFStatus &status = frameList[frameId].statusList[i].second;
        int channelId = findChannelId(partName);
        const OFStatus &statusNext = frameList[frameId + 1].statusList[i].second;
        const int &r = (int)round((1 - rate) * (float)status.r + rate * (float)statusNext.r);
        const int &g = (int)round((1 - rate) * (float)status.g + rate * (float)statusNext.g);
        const int &b = (int)round((1 - rate) * (float)status.b + rate * (float)statusNext.b);
        const int &a = (int)round(((1 - rate) * (float)status.a + rate * (float)statusNext.a));
        fadeFrameStatus[channelId] = OFStatus(r, g, b, a);
    }

    return fadeFrameStatus;
}

int OFPlayer::findFrameId(const long &time) {
    const int totalFrame = frameList.size();
    // printf("totalFrame :%d",totalFrame);
    if (totalFrame == 0 || time < 0) return -1;
    if (time > frameList[frameList.size() - 1].start) return totalFrame - 1;
    if (time >= frameList[frameId].start) {
        if ((unsigned int)frameId < frameList.size() - 1) {
            if (time < frameList[frameId + 1].start) return frameId;
        }
        if ((unsigned int)frameId < frameList.size() - 2) {
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

// store information of every part
int OFPlayer::findChannelId(const string &partName) { return channelIds[partName]; }

void OFPlayer::init() {
    controller.init();
    frameId = 0;
}

vector<int> OFPlayer::castStatusList(vector<OFStatus> statusList) {
    vector<int> castedList(statusList.size());
    for (unsigned int i = 0; i < statusList.size(); i++) {
        const OFStatus &status = statusList[i];
        castedList[i] = ((status.r << 24) + (status.g << 16) + (status.b << 8) + (status.a << 0));
    }

    return castedList;
}

void OFPlayer::setLightStatus(vector<OFStatus> &statusList, int r, int g, int b, int a) {
    statusList.resize(OFnum);
    for (auto &status : statusList) {
        status = OFStatus(r, g, b, a);
    }
}

void OFPlayer::delayDisplay(const bool *delayingDisplay) {
    vector<OFStatus> statusList;

    // Let OF lightall for 1/5 times of delayTime
    if (*delayingDisplay) {
        setLightStatus(statusList, 100, 0, 0, 100);
        controller.sendAll(castStatusList(statusList));
    } else {
        setLightStatus(statusList, 0, 0, 0, 0);
        controller.sendAll(castStatusList(statusList));
    }
}

void OFPlayer::loop(StateMachine *fsm) {
    timeval currentTime;
    vector<OFStatus> statusList;

#ifdef PLAYER_DEBUG
    ofstream logFile("/tmp/of.log");
#endif
    // *finished = false;

    while (true) {
        timeval lastTime = currentTime;
        gettimeofday(&currentTime, NULL);
        float fps = 1000000.0 / getElapsedTime(lastTime, currentTime);

        if (fsm->getCurrentState() == S_STOP||fsm->getCurrentState() == S_PAUSE) {
            // TODO: finish darkall
            setLightStatus(statusList, 0, 0, 0, 0);
            controller.sendAll(castStatusList(statusList));
            break;
        }
        if (fsm->getCurrentState() == S_PLAY) {
            const long elapsedTime = getElapsedTime(fsm->data.baseTime, currentTime);
            const long elapsedTimeInMs = elapsedTime / 1000l;
            statusList.clear();

            // find status
            frameId = findFrameId(elapsedTimeInMs);
            if (frameId == -1) break;
            statusList = findFrameStatus(elapsedTimeInMs);

            controller.sendAll(castStatusList(statusList));

#ifdef PLAYER_DEBUG
            char buf[1024];
            sprintf(buf, "[OF] Time: %s Frame: %d / %d\n", parseMicroSec(elapsedTime).c_str(),
                    frameId, (int)frameList.size() - 1);

            for (int i = 0; i < statusList.size(); i++) {
                sprintf(buf + strlen(buf), "OF%2d: [%3d,%3d,%3d,%3d]\n", i, statusList[i].r,
                        statusList[i].g, statusList[i].b, statusList[i].a);
            }
            logFile << buf;

            if (frameId == frameList.size() - 1) {
                break;
            }
#endif
            if (frameId == frameList.size() - 1) {
                break;
            }

            // fprintf(stderr, "[OF] Time: %s, FPS: %4.2f\n",
            //         parseMicroSec(elapsedTime).c_str(), fps);

            this_thread::yield();
        }
    }
    cerr << "[OF] finish\n";

#ifdef PLAYER_DEBUG
    logFile << "[OF] finish\n";
    logFile.close();
#endif
    //fsm->setState(S_STOP);
}
