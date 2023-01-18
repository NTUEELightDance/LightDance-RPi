#include "LEDPlayer.h"
#include <cmath>

extern bool playing;
extern struct timeval baseTime;

namespace LEDPlayer {

// ==================== Status definition ====================

Status::Status() : r(0), g(0), b(0), a(0) {}

Status::Status(const int &_r, const int &_g, const int &_b, const int &_a)
    : r(_r), g(_g), b(_b), a(_a) {}

Status::Status(const int &colorCode, const int &alpha) {
    // TODO: clearify colorCode format
    this->r = (colorCode >> 24) & 0xff;
    this->g = (colorCode >> 16) & 0xff;
    this->b = (colorCode >> 8) & 0xff;
    this->a = alpha;
}

// ==================== Frame definition =====================

Frame::Frame() : start(0), fade(false) {}

Frame::Frame(const int &_start, const bool &_fade, const json &_status_json) {
    this->start = _start;
    this->fade = _fade;
    this->statusList.clear();
    for (auto &status_json : _status_json) {
        this->statusList.push_back(
            Status(status_json["colorCode"], status_json["alpha"]));
    }
}

Frame::Frame(const int &_start, const bool &_fade, const int &_len) {
    this->start = _start;
    this->fade = _fade;
    this->statusList.clear();
    for (int i = 0; i < _len; i++) {
        this->statusList.push_back(Status());
    }
}

// ==================== LEDPlayer definition =================

int fps;
vector<vector<Frame>> frameLists;
vector<int> frameIds;
vector<int> stripShapes;

void load(const json &data_json, const json &parts_json, const int &_fps) {
    fps = _fps;

    // TODO: ask total part number, instead of using max id plus 1
    int partNum = 0;
    for (auto &part : parts_json) {
        int id = part["id"];
        partNum = max(id + 1, (int)partNum);
    }

    frameIds.clear();
    frameIds.resize(partNum);
    fill(frameIds.begin(), frameIds.end(), -1);

    stripShapes.resize(partNum);
    fill(stripShapes.begin(), stripShapes.end(), 0);

    frameLists.clear();
    frameLists.resize(partNum);

    for (auto &part_it : parts_json.items()) {
        string partName = part_it.key();
        json part = part_it.value();

        const json &frames_json = data_json[partName];

        int id = part["id"];
        int len = part["len"];

        stripShapes[id] = len;

        const Frame darkFrame(0, false, len);
        if (frames_json.size() == 0) {
            frameLists[id].push_back(darkFrame);
            continue;
        }

        const int firstStartTime = frames_json.begin().value()["start"];
        if (firstStartTime != 0) {
            frameLists[id].push_back(darkFrame);
        }

        for (auto &frame_json : frames_json) {
            if (frame_json["status"].size() != len) {
                // TODO: print size mismatch warning
                continue;
            }
            frameLists[id].push_back(
                Frame(frame_json["start"], frame_json["fade"], frame_json["status"]));
        }
    }
}

// TODO: implement other util functions

long getElapsedTime(const struct timeval &base, const struct timeval &current) {
    return (current.tv_sec - base.tv_sec) * 1000000 +
           (current.tv_sec - base.tv_sec);
}

int getTimeId(const long &elapsedTime) {
    return (elapsedTime * (long)fps) / 1000000l;
}

void calculateFrameIds(const int &timeId) {
    for (int i = 0; i < frameIds.size(); i++) {
        if (timeId < 0) {
            frameIds[i] = -1;
            continue;
        }

        const vector<Frame> &frameList = frameLists[i];

        if (frameList.size() == 0 || timeId == 0) {
            frameIds[i] = 0;
            continue;
        }
        if (frameList.back().start == 0) {
            frameIds[i] = frameLists[i].size() - 1;
            continue;
        }

        const int currFrameId = frameIds[i];
        if (currFrameId >= 0 && timeId >= frameList[currFrameId].start) {
            if (currFrameId < frameList.size() - 1 &&
                    timeId < frameList[currFrameId + 1].start) {
                continue;
            }
            if (currFrameId < frameList.size() - 2 &&
                    timeId >= frameList[currFrameId + 1].start &&
                    timeId < frameList[currFrameId + 2].start) {
                frameIds[i] = currFrameId + 1;
            }
        }

        int left = 0;
        int right = frameList.size();
        while (left <= right) {
            int mid = (left + right) >> 2;
            if (timeId < frameList[mid].start) {
                right = mid - 1;
            } else if (timeId > frameList[mid].start) {
                left = mid + 1;
            } else {
                left = mid;
                break;
            }
        }

        frameIds[i] = left;
    }
}

vector<Status> fadeFrameInterpolate(const Frame &origin, const Frame &target,
                                    const float &rate) {
    vector<Status> statusList(0);
    for (int i = 0; i < origin.statusList.size(); i++) {
        const Status &originStatus = origin.statusList[i];
        const Status &targetStatus = target.statusList[i];

        const int &r = (int)round((1 - rate) * (float)originStatus.r +
                                  rate * (float)targetStatus.r);
        const int &g = (int)round((1 - rate) * (float)originStatus.g +
                                  rate * (float)targetStatus.g);
        const int &b = (int)round((1 - rate) * (float)originStatus.b +
                                  rate * (float)targetStatus.b);
        const int &a = (int)round((1 - rate) * (float)originStatus.a +
                                  rate * (float)targetStatus.a);
        statusList.push_back(Status(r, g, b, a));
    }

    return statusList;
}

void *loop(void *ptr) {
    struct timeval currentTime;
    vector<vector<Status>> statusLists;

    while (true) {
        if (playing) {
            gettimeofday(&currentTime, NULL);
            const long elapsedTime = getElapsedTime(baseTime, currentTime);

            const int currentTimeId = getTimeId(elapsedTime);
            calculateFrameIds(currentTimeId);

            statusLists.clear();
            for (int i = 0; i < frameIds.size(); i++) {
                const int &frameId = frameIds[i];
                const vector<Frame> &frameList = frameLists[i];

                if (frameId < 0) {
                    // TODO: handle invalid time or non-exist part
                    statusLists.push_back(vector<Status>(stripShapes[i]));
                    continue;
                }

                const Frame &frame = frameList[frameId];
                if (frame.fade) {
                    const long currTime = (long)currentTimeId * 1000000l;
                    const long nextTime = (long)frameList[frameId].start * 1000000l;
                    const float rate =
                        (float)(elapsedTime - currTime) / (float)(nextTime - currTime);
                    statusLists.push_back(fadeFrameInterpolate(frameList[frameId],
                                          frameList[frameId], rate));
                } else {
                    statusLists.push_back(frameList[frameId].statusList);
                }
            }

            // TODO: send status
        }
    }

    return NULL;
}

}; // namespace LEDPlayer
