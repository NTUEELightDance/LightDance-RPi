#include "LEDPlayer.h"

// ==================== LEDStatus ============================

// TODO: implement archive

template <class Archive>
void LEDStatus::serialize(Archive &archive, const unsigned int version) {}

LEDStatus::LEDStatus() : r(0), g(0), b(0), a(0) {}

LEDStatus::LEDStatus(const uint &_r, const uint &_g, const uint &_b,
                     const uint &_a)
    : r(_r), g(_g), b(_b), a(_a) {}

// ==================== LEDFrame =============================

// TODO: implement archive

template <class Archive>
void LEDFrame::serialize(Archive &archive, const unsigned int version) {}

LEDFrame::LEDFrame() : start(0), fade(false) {}

LEDFrame::LEDFrame(const int &_start, const bool &_fade,
                   const vector<LEDStatus> &_statusList) {
    start = _start;
    fade = _fade;
    statusList.clear();
    statusList.assign(_statusList.begin(), _statusList.end());
}

LEDFrame::LEDFrame(const int &_start, const bool &_fade, const int &_len) {
    start = _start;
    fade = _fade;
    statusList.clear();
    for (int i = 0; i < _len; i++) {
        statusList.push_back(LEDStatus());
    }
}

// ==================== LEDPlayer ============================

// TODO: implement archive, list, save, and restore

template <class Archive>
void LEDPlayer::serialize(Archive &archive, const unsigned int version) {}

string LEDPlayer::list() const {}

ostream &operator<<(ostream &os, const LEDPlayer &player) {}

void save(const char *filename) {}

bool restore(const char *filename) {}

LEDPlayer::LEDPlayer() : fps(0) {}

LEDPlayer::LEDPlayer(const int &_fps,
                     const vector<vector<LEDFrame>> &_frameLists,
                     const vector<int> &_frameIds,
                     const vector<int> &_stripShapes) {
    fps = _fps;
    frameLists.assign(_frameLists.begin(), _frameLists.end());
    frameIds.assign(_frameIds.begin(), _frameIds.end());
    stripShapes.assign(_stripShapes.begin(), _stripShapes.end());

    printf("%d\n", fps);
    printf("%d\n", (int)frameLists.size());
    printf("%d\n", (int)frameIds.size());
    printf("%d\n", (int)stripShapes.size());
}

long LEDPlayer::getElapsedTime(const struct timeval &base,
                               const struct timeval &current) {
    // Get time elapsed from start time in microsecond
    return (long)(current.tv_sec - base.tv_sec) * 1000000l +
           (long)(current.tv_usec - base.tv_usec);
}

int LEDPlayer::getTimeId(const long &elapsedTime) {
    // Get id of elapsed time
    return (elapsedTime * (long)fps) / 1000000l;
}

void LEDPlayer::calculateFrameIds(const int &timeId) {
    // Find current frame by time id
    for (int i = 0; i < frameIds.size(); i++) {
        const vector<LEDFrame> &frameList = frameLists[i];

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

vector<LEDStatus> LEDPlayer::interpolateFadeFrame(const LEDFrame &origin,
                                                  const LEDFrame &target,
                                                  const float &rate) {
    vector<LEDStatus> statusList(0);
    for (int i = 0; i < origin.statusList.size(); i++) {
        const LEDStatus &originLEDStatus = origin.statusList[i];
        const LEDStatus &targetLEDStatus = target.statusList[i];

        const int &r = (int)round((1 - rate) * (float)originLEDStatus.r +
                                  rate * (float)targetLEDStatus.r);
        const int &g = (int)round((1 - rate) * (float)originLEDStatus.g +
                                  rate * (float)targetLEDStatus.g);
        const int &b = (int)round((1 - rate) * (float)originLEDStatus.b +
                                  rate * (float)targetLEDStatus.b);
        const int &a = (int)round((1 - rate) * (float)originLEDStatus.a +
                                  rate * (float)targetLEDStatus.a);
        statusList.push_back(LEDStatus(r, g, b, a));
    }

    return statusList;
}

void LEDPlayer::loop(const bool *playing, const timeval *baseTime) {
    struct timeval currentTime;
    vector<vector<LEDStatus>> statusLists;

    while (true) {
        // if (playing && checkReady()) {
        if (*playing) {
            gettimeofday(&currentTime, NULL);
            const long elapsedTime = getElapsedTime(*baseTime, currentTime);
            printf("Time: %f\n", elapsedTime / 1000000.0f);

            const int currentTimeId = getTimeId(elapsedTime);
            // printf("Time Id: %d\n", currentTimeId);
            calculateFrameIds(currentTimeId);

            // printf("LEDFrame Ids: ");
            // for (const int &id : frameIds) {
            //     printf(" %d", id);
            // }
            // printf("\n");

            statusLists.clear();
            for (int i = 0; i < frameIds.size(); i++) {
                const int &frameId = frameIds[i];
                const vector<LEDFrame> &frameList = frameLists[i];

                if (frameId < 0) {
                    // TODO: handle invalid time or non-exist part
                    statusLists.push_back(vector<LEDStatus>(stripShapes[i]));
                    continue;
                }

                const LEDFrame &frame = frameList[frameId];
                if (frame.fade) {
                    const long startTime =
                        (long)frameList[frameId].start * 1000000l / (long)fps;
                    const long endTime = (long)frameList[frameId + 1].start *
                                         1000000l / (long)fps;
                    const float rate = (float)(elapsedTime - startTime) /
                                       (float)(endTime - startTime);
                    statusLists.push_back(interpolateFadeFrame(
                        frame, frameList[frameId + 1], rate));
                } else {
                    statusLists.push_back(frameList[frameId].statusList);
                }

                // printf("  LEDFrame: %d\n", i);
                // const vector<LEDStatus> &statusList = statusLists[i];
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
