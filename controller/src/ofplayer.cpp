#include "ofplayer.h"

OFPlayer::OFStatus::OFStatus(const int& cc, const int& a) {
    colorCode = cc;
    alpha = a;
}

OFPlayer::Frame::Frame(const size_t& _start, const bool& _fade, const json& _status) {
    start = _start;
    fade = _fade;
    for (auto it = _status.begin(); it != _status.end(); ++it)
        status.insert(pair<string, OFStatus>(it.key(), OFStatus(it.value()["colorCode"], it.value()["alpha"])));
}

void OFPlayer::init(const json& OFparts) {
    for (auto it = OFparts.begin(); it != OFparts.end(); ++it)
        channelId.insert(pair<string, int>(it.key(), it.value()));
}

void OFPlayer::load(const json& pl) {
    playList.reserve(pl.size());
    for (auto& f : pl) {
        Frame frame(f["start"], f["fade"], f["status"]);
        playList.push_back(frame);
    }
    playList[playList.size() - 1].fade = false;
}

OFPlayer::Frame OFPlayer::getFrame(const size_t& time) {
    frameId = getFrameId(time);
    return playList[frameId].fade ? getFadeFrame(time) : playList[frameId];
}

bool OFPlayer::isFinished() const {
    return playList.size() - 1 == frameId;
}

int OFPlayer::getFrameNum() const {
    return playList.size();
}

size_t OFPlayer::getEndTime() const {
    return playList[playList.size() - 1].start;
}

int OFPlayer::getChannelId(const string& _part) {
    return channelId[_part];
}

size_t OFPlayer::getFrameId(const size_t& time) {
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

OFPlayer::Frame OFPlayer::getFadeFrame(const size_t& time) {
    const size_t firstTime = playList[frameId].start;
    const size_t secondTime = playList[frameId + 1].start;
    const float rate = (float)(time - firstTime) / (float)(secondTime - firstTime);

    Frame f;
    f.start = time;
    f.fade = true;
    for (auto it = playList[frameId].status.begin(); it != playList[frameId].status.end(); ++it) {
        const int cc = rgbHexInterpolate(it->second.colorCode, playList[frameId + 1].status[it->first].colorCode, rate);
        const int a = (int)round((1 - rate) * double(it->second.alpha) + rate * double(playList[frameId + 1].status[it->first].alpha));
        f.status.insert(pair<string, OFStatus>(it->first, OFStatus(cc, a)));
    }
    return f;
}

string OFPlayer::getParts() {
    string mes;
    for (auto it = channelId.begin(); it != channelId.end(); ++it) {
        mes += "\t";
        mes += it->first;
        for (int i = 0; i < 15 - it->first.size(); ++i)
            mes += ' ';
        mes += "channel: " + to_string(it->second);
        mes += '\n';
    }
    return mes;
}