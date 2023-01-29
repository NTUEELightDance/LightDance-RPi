#include "OFPlayer.h"

extern bool playing;
extern struct timeval baseTime;

namespace OFPlayer {

Status::Status() : r(0), g(0), b(0), a(0) {}

Status::Status(const int &_r, const int &_g, const int &_b, const int &_a)
    : r(_r), g(_g), b(_b), a(_a) {}

Frame::Frame() : start(0), fade(false) {}

Frame::Frame(const long &_start, const bool &_fade, const json &_status) {
    this->start = _start;
    this->fade = _fade;
    this->status.clear();
    // append all status
    for (auto &it : _status.items()) {
        this->status.push_back(pair<string, Status>(
            it.key(), Status(it.value()[0], it.value()[1], it.value()[2],
                             it.value()[3])));
    }
}

// ====================== OFPlayer definition ===============================

vector<Frame> frameList;
vector<vector<Status>> statusList;
int frameId;
unordered_map<string, int> channelIds;
int fps;

// WARNING: this will crash
void load(const json &data_json, const json &parts_json, const int &_fps) {
    fps = _fps;

    frameList.clear();
    statusList.clear();
    frameList.resize(26);

    // build frames
    for (auto &data : data_json) {
        long _start = data["start"];
        bool _fade = data["fade"];

        // set size of one frame
        // twenty-six channel
        vector<Status> newStatus;
        newStatus.resize(26);

        // init darkFrame
        Frame darkFrame;
        vector<Status> darkStatus;
        darkStatus.resize(26);

        darkFrame.start = 0;
        darkFrame.fade = false;
        for (auto &part : parts_json.items()) {
            darkFrame.status.push_back(
                pair<string, Status>(part.key(), Status(0, 0, 0, 0)));
            darkStatus.push_back(Status(0, 0, 0, 0));
        }

        if (data.size() == 0) {
            frameList.push_back(darkFrame);
            statusList.push_back(darkStatus);
            return;
        }

        // init channelId
        for (auto &part : parts_json.items()) {
            string partName = part.key();
            channelIds[partName] = part.value();
            // map channelId to every part's status and frame
            int channelId = part.value();
            newStatus[channelId] = Status(
                data["status"][partName][0], data["status"][partName][1],
                data["status"][partName][2], data["status"][partName][3]);
        }

        statusList.push_back(newStatus);
        frameList.push_back(Frame(_start, _fade, data["status"]));
    }
}

vector<Status> findFrameStatus(const long &time) {
    frameId = findFrameId(time);
    return frameList[frameId].fade ? findFadeFrameStatus(time)
                                   : statusList[frameId];
}

vector<Status> findFadeFrameStatus(const long &time) {
    const long startTime = frameList[frameId].start;
    const long endTime = frameList[frameId + 1].start;
    const float rate = (float)(time - startTime) / (float)(endTime - startTime);

    vector<Status> fadeFrameStatus;
    fadeFrameStatus.resize(26);
    // Do interpolate
    for (auto &status_pair : frameList[frameId].status) {
        const string &partName = status_pair.first;
        const Status &status = status_pair.second;
        int channelId = findChannelId(partName);
        const Status &statusNext =
            frameList[frameId + 1].status[channelId].second;
        const int &r = (int)round((1 - rate) * (float)status.r +
                                  rate * (float)statusNext.r);
        const int &g = (int)round((1 - rate) * (float)status.g +
                                  rate * (float)statusNext.g);
        const int &b = (int)round((1 - rate) * (float)status.b +
                                  rate * (float)statusNext.b);
        const int &a = (int)round((1 - rate) * (float)status.a +
                                  rate * (float)statusNext.a);
        fadeFrameStatus[channelId] = Status(r, g, b, a);
    }

    return fadeFrameStatus;
}

int findFrameId(const long &time) {
    const int totalFrame = frameList.size();
    if (totalFrame == 0 || time <= 0) return 0;
    if (time > frameList[frameList.size() - 1].start)
        return frameList.size() - 1;
    if (time >= frameList[frameId].start) {
        if (frameId < frameList.size() - 1) {
            if (time < frameList[frameId + 1].start) return frameId;
        }
        if (frameId < frameList.size() - 2) {
            if (time >= frameList[frameId + 1].start &&
                time < frameList[frameId + 2].start)
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
int findChannelId(const string &partName) { return channelIds[partName]; }

long timeTransformation(const struct timeval &tm) {
    return (long)(tm.tv_sec * 1000000l) + (long)(tm.tv_usec);
}

void loop() {
    // struct timeval currentTime;
    vector<Status> status;

    while (true) {
        if (playing) {
            // find frame status by start time
            //  gettimeofday(&currentTime, NULL);
            const long startTime = timeTransformation(baseTime);
            status.clear();

            // find status
            status = findFrameStatus(startTime);
            sleep((long)(1000 / fps));
        }
    }
}
}  // namespace OFPlayer
