#include "ledplayer.h"

LEDPlayer::LEDStatus::LEDStatus(const int& cc, const int& a) {
    colorCode = cc;
    alpha = a;
}

LEDPlayer::LEDStatus& LEDPlayer::LEDStatus::operator=(const LEDPlayer::LEDStatus& ls) {
    colorCode = ls.colorCode;
    alpha = ls.alpha;
    return *this;
}

LEDPlayer::Frame::Frame(const size_t& _start, const bool& _fade, const json& _status) {
    start = _start;
    fade = _fade;
    status.reserve(_status.size());
    for (auto& ls : _status)
        status.push_back(LEDStatus((int)ls["colorCode"], (int)ls["alpha"]));
}

LEDPlayer::Frame& LEDPlayer::Frame::operator=(const LEDPlayer::Frame& f) {
    start = f.start;
    fade = f.fade;
    status = f.status;
    return *this;
}

void LEDPlayer::load(const json& pl) {
    playList.reserve(pl.size());
    for (auto& f : pl) {
        Frame frame(f["start"], f["fade"], f["status"]);
        if (frame.status.size() != len) {
            cout << name << endl;
            cout << "Frame status length: " << frame.status.size();
            cout << "Frame time: " << frame.start << endl;
            cout << "Expected Frame status length: " << len << endl;
            continue;
        }
        playList.push_back(frame);
    }
    playList[playList.size() - 1].fade = false;
}

LEDPlayer::Frame LEDPlayer::getFrame(const size_t& time) {
    frameId = getFrameId(time);
    return playList[frameId].fade ? getFadeFrame(time) : playList[frameId];
}

bool LEDPlayer::isFinished() const {
    return playList.size() - 1 == frameId;
}

size_t LEDPlayer::getFrameId(const size_t& time) const {
    const size_t totalFrame = playList.size();
    if (totalFrame == 0)
        return 0;
    if (time > playList[playList.size() - 1].start)
        return playList.size() - 1;
    if (time == 0)
        return 0;
    if (time >= playList[frameId].start) {
        if (frameId < playList.size() - 1)
            if (time < playList[frameId + 1].start)
                return frameId;
        if (frameId < playList.size() - 2)
            if (time >= playList[frameId + 1].start && time < playList[frameId + 2].start)
                return frameId + 1;
    }

    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first <= last) {
        const size_t mid = (first + last) >> 1;
        if (time > playList[mid].start)
            first = mid + 1;
        else if (time == playList[mid].start)
            return mid;
        else
            last = mid - 1;
    }
    if (time < playList[first].start)
        --first;
    return first;
}

LEDPlayer::Frame LEDPlayer::getFadeFrame(const size_t& time) const {
    const size_t firstTime = playList[frameId].start;
    const size_t secondTime = playList[frameId + 1].start;
    const float rate = (float)(time - firstTime) / (float)(secondTime - firstTime);

    Frame f;
    f.start = time;
    f.fade = true;
    for (int i = 0; i < len; ++i) {
        const int cc = rgbHexInterpolate(playList[frameId].status[i].colorCode, playList[frameId + 1].status[i].colorCode, rate);
        const int a = (int)round((1 - rate) * double(playList[frameId].status[i].alpha) + rate * double(playList[frameId + 1].status[i].alpha));
        f.status.push_back(LEDStatus(cc, a));
    }
    return f;
}

int LEDPlayer::getFrameNum() const {
    return playList.size();
}

size_t LEDPlayer::getEndTime() const {
    return playList[playList.size() - 1].start;
}