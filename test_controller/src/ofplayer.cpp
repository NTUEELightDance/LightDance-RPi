#include "ofplayer.h"

OFPlayer::of_status::of_status(const int& cc, const int& a) {
    colorCode = cc;
    alpha = a;
}

OFPlayer::frame::frame(const size_t& _start, const bool& _fade, const json& _status) {
    start = _start;
    fade = _fade;
    for (auto it = _status.begin(); it != _status.end(); ++it)
        status.insert(pair<string, of_status>(it.key(), of_status(it.value()["colorCode"], it.value()["alpha"])));
}

void OFPlayer::init(const json& OFparts) {
    for (auto it = OFparts.begin(); it != OFparts.end(); ++it)
        channel_id.insert(pair<string, int>(it.key(), it.value()));
}

void OFPlayer::load(const json& pl) {
    play_list.reserve(pl.size());
    for (auto& f : pl) {
        frame _frame(f["start"], f["fade"], f["status"]);
        play_list.push_back(_frame);
    }
    play_list[play_list.size() - 1].fade = false;
}

OFPlayer::frame OFPlayer::getFrame(const size_t& time) {
    frame_id = getFrameId(time);
    return play_list[frame_id].fade ? getFadeFrame(time) : play_list[frame_id];
}

bool OFPlayer::is_finished() const {
    return play_list.size() - 1 == frame_id;
}

int OFPlayer::getFrameNum() const {
    return play_list.size();
}

size_t OFPlayer::getEndTime() const {
    return play_list[play_list.size() - 1].start;
}

int OFPlayer::getChannelId(const string& _part) {
    return channel_id[_part];
}

size_t OFPlayer::getFrameId(const size_t& time) {
    const size_t totalFrame = play_list.size();
    if (totalFrame == 0)
        return 0;
    if (time > play_list[play_list.size() - 1].start)
        return play_list.size() - 1;
    if (time == 0)
        return 0;
    if (time >= play_list[frame_id].start) {
        if (frame_id < play_list.size() - 1)
            if (time < play_list[frame_id + 1].start)
                return frame_id;
        if (frame_id < play_list.size() - 2)
            if (time >= play_list[frame_id + 1].start && time < play_list[frame_id + 2].start)
                return frame_id + 1;
    }

    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first <= last) {
        const size_t mid = (first + last) >> 1;
        if (time > play_list[mid].start)
            first = mid + 1;
        else if (time == play_list[mid].start)
            return mid;
        else
            last = mid - 1;
    }
    if (time < play_list[first].start)
        --first;
    return first;
}

OFPlayer::frame OFPlayer::getFadeFrame(const size_t& time) {
    const size_t firstTime = play_list[frame_id].start;
    const size_t secondTime = play_list[frame_id + 1].start;
    const float rate = (float)(time - firstTime) / (float)(secondTime - firstTime);

    frame f;
    f.start = time;
    f.fade = true;
    for (auto it = play_list[frame_id].status.begin(); it != play_list[frame_id].status.end(); ++it) {
        const int cc = rgbHexInterpolate(it->second.colorCode, play_list[frame_id + 1].status[it->first].colorCode, rate);
        const int a = (int)round((1 - rate) * double(it->second.alpha) + rate * double(play_list[frame_id + 1].status[it->first].alpha));
        f.status.insert(pair<string, of_status>(it->first, of_status(cc, a)));
    }
    return f;
}

string OFPlayer::getParts() {
    string mes;
    for (auto it = channel_id.begin(); it != channel_id.end(); ++it) {
        mes += "\t";
        mes += it->first;
        for (int i = 0; i < 15 - it->first.size(); ++i)
            mes += ' ';
        mes += "channel: " + to_string(it->second);
        mes += '\n';
    }
    return mes;
}