#include "LEDPlayer.h"
#include <cmath>
#include <unistd.h>

extern bool playing;
extern struct timeval baseTime;

namespace LEDPlayer {

// ==================== Status definition ====================

Status::Status() : r(0), g(0), b(0), a(0) {}

Status::Status(const uint &_r, const uint &_g, const uint &_b, const uint &_a)
    : r(_r), g(_g), b(_b), a(_a) {}

// Status::Status(const uint &colorCode, const uint &alpha) {
//     this->r = (colorCode >> 24) & 0xff;
//     this->g = (colorCode >> 16) & 0xff;
//     this->b = (colorCode >> 8) & 0xff;
//     this->a = alpha;
// }

// ==================== Frame definition =====================

Frame::Frame() : start(0), fade(false) {}

Frame::Frame(const int &_start, const bool &_fade, const json &_status_json) {
    this->start = _start;
    this->fade = _fade;
    this->statusList.clear();
    for (auto &status_json : _status_json) {
        this->statusList.push_back(
            Status(status_json[0], status_json[1], status_json[2], status_json[3]));
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
    // printf("FPS: %d\n", fps);

    // TODO: load from .h file, instead of hard coded
    int partNum = 16;

    frameIds.clear();
    frameIds.resize(partNum);
    fill(frameIds.begin(), frameIds.end(), -1);

    stripShapes.resize(partNum);
    fill(stripShapes.begin(), stripShapes.end(), 0);

    frameLists.clear();
    frameLists.resize(partNum);

    for (auto &part_it : parts_json.items()) {
        const string partName = part_it.key();
        const json part = part_it.value();

        const json &frames_json = data_json[partName];

        const int id = part["id"];
        const int len = part["len"];

        stripShapes[id] = len;

        // If no frames are given, push only dark frame
        const Frame darkFrame(0, false, len);
        if (frames_json.size() == 0) {
            frameLists[id].push_back(darkFrame);
            continue;
        }

        // If not starting at time = 0, use dark frame as first frame
        const int firstStartTime = frames_json.begin().value()["start"];
        if (firstStartTime != 0) {
            frameLists[id].push_back(darkFrame);
        }

        for (const json &frame_json : frames_json) {
            if (frame_json["status"].size() != len) {
                // TODO: print size mismatch warning
                continue;
            }
            frameLists[id].push_back(
                Frame(frame_json["start"], frame_json["fade"], frame_json["status"]));
        }
    }

    // for (int i = 0; i < frameLists.size(); i++) {
    //     const vector<Frame> &frameList = frameLists[i];
    //     printf("part Id: %d\n", i);
    //     for (int j = 0; j < frameList.size(); j++) {
    //         const Frame &frame = frameList[j];
    //         printf("  frame Id: %d\n", j);
    //         printf("    start: %d\n", frame.start);
    //         printf("    fade: %s\n", frame.fade ? "true" : "false");
    //         printf("    status number: %d\n", (int)frame.statusList.size());
    //     }
    // }

    // init(stripShapes);
}

long getElapsedTime(const struct timeval &base, const struct timeval &current) {
    // Get time elapsed from start time in microsecond
    return (long)(current.tv_sec - base.tv_sec) * 1000000l +
           (long)(current.tv_usec - base.tv_usec);
}

int getTimeId(const long &elapsedTime) {
    // Get id of elapsed time
    return (elapsedTime * (long)fps) / 1000000l;
}

void calculateFrameIds(const int &timeId) {
    // Find current frame by time id
    for (int i = 0; i < frameIds.size(); i++) {
        const vector<Frame> &frameList = frameLists[i];

        // Do not process if no frame is empty or time is invalid
        if (frameList.size() == 0 || timeId < 0) {
            frameIds[i] = -1;
            continue;
        }
        // Use last frame if time exceeded the limit
        if (timeId >= frameList.back().start) {
            frameIds[i] = frameLists[i].size() - 1;
            continue;
        }

        // Check if simple move is available
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
                continue;
            }
        }

        // Otherwise, apply binary search
        int left = 0;
        int right = frameList.size() - 1;
        while (left < right) {
            int mid = (left + right) >> 1;
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

vector<Status> interpolateFadeFrame(const Frame &origin, const Frame &target,
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

void loop() {
    struct timeval currentTime;
    vector<vector<Status>> statusLists;

    while (true) {
        // if (playing && checkReady()) {
        if (playing) {
            gettimeofday(&currentTime, NULL);
            const long elapsedTime = getElapsedTime(baseTime, currentTime);
            printf("Time: %f\n", elapsedTime / 1000000.0f);

            const int currentTimeId = getTimeId(elapsedTime);
            // printf("Time Id: %d\n", currentTimeId);
            calculateFrameIds(currentTimeId);

            printf("Frame Ids: ");
            for (const int &id : frameIds) {
                printf(" %d", id);
            }
            printf("\n");

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
                    const long startTime =
                        (long)frameList[frameId].start * 1000000l / (long)fps;
                    const long endTime =
                        (long)frameList[frameId + 1].start * 1000000l / (long)fps;
                    const float rate =
                        (float)(elapsedTime - startTime) / (float)(endTime - startTime);
                    statusLists.push_back(
                        interpolateFadeFrame(frame, frameList[frameId + 1], rate));
                } else {
                    statusLists.push_back(frameList[frameId].statusList);
                }

                // printf("  Frame: %d\n", i);
                // const vector<Status> &statusList = statusLists[i];
                // for (int j = 0; j < 1; j++) {
                //     printf("    status: %d\n", j);
                //     printf("      r: %d\n", statusList[j].r);
                //     printf("      g: %d\n", statusList[j].g);
                //     printf("      b: %d\n", statusList[j].b);
                //     printf("      a: %d\n", statusList[j].a);
                // }
            }

            // sendAll(statusList);
            usleep(1000);
        }
    }
}

}; // namespace LEDPlayer
