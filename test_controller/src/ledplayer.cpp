#include "ledplayer.h"

LEDPlayer::led_status::led_status(const int& cc, const int& a) {
    colorCode = cc;
    alpha = a;
}

LEDPlayer::led_status& LEDPlayer::led_status::operator=(const LEDPlayer::led_status& ls) {
    colorCode = ls.colorCode;
    alpha = ls.alpha;
    return *this;
}

LEDPlayer::frame::frame(const size_t& _start, const bool& _fade, const json& _status) {
    start = _start;
    fade = _fade;
    status.reserve(_status.size());
    for (auto& ls : _status)
        status.push_back(led_status((int)ls["colorCode"], (int)ls["alpha"]));
}

LEDPlayer::frame& LEDPlayer::frame::operator=(const LEDPlayer::frame& f) {
    start = f.start;
    fade = f.fade;
    status = f.status;
    return *this;
}

void LEDPlayer::load(const json& pl) {
    play_list.reserve(pl.size());
    for (auto& f : pl) {
        frame _frame(f["start"], f["fade"], f["status"]);
        if (_frame.status.size() != len) {
            cout << name << endl;
            cout << "Frame status length: " << _frame.status.size();
            cout << "Frame time: " << _frame.start << endl;
            cout << "Expected frame status length: " << len << endl;
            continue;
        }
        play_list.push_back(_frame);
    }
    play_list[play_list.size() - 1].fade = false;
}

LEDPlayer::frame LEDPlayer::getFrame(const size_t& time) {
    frame_id = getFrameId(time);
    return play_list[frame_id].fade ? getFadeFrame(time) : play_list[frame_id];
}

bool LEDPlayer::is_finished() const {
    return play_list.size() - 1 == frame_id;
}

size_t LEDPlayer::getFrameId(const size_t& time) const {
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

LEDPlayer::frame LEDPlayer::getFadeFrame(const size_t& time) const {
    const size_t firstTime = play_list[frame_id].start;
    const size_t secondTime = play_list[frame_id + 1].start;
    const float rate = (float)(time - firstTime) / (float)(secondTime - firstTime);

    frame f;
    f.start = time;
    f.fade = true;
    for (int i = 0; i < len; ++i) {
        const int cc = rgbHexInterpolate(play_list[frame_id].status[i].colorCode, play_list[frame_id + 1].status[i].colorCode, rate);
        const int a = (int)round((1 - rate) * double(play_list[frame_id].status[i].alpha) + rate * double(play_list[frame_id + 1].status[i].alpha));
        f.status.push_back(led_status(cc, a));
    }
    return f;
}

int LEDPlayer::getFrameNum() const {
    return play_list.size();
}

size_t LEDPlayer::getEndTime() const {
    return play_list[play_list.size() - 1].start;
}