#include "LEDPlayer.h"

#include <thread>

#include "utils.h"

// ==================== LEDStatus ============================

template <class Archive>
void LEDStatus::serialize(Archive &archive, const unsigned int version) {
    archive &r;
    archive &g;
    archive &b;
    archive &a;
}

LEDStatus::LEDStatus() : r(0), g(0), b(0), a(0) {}

LEDStatus::LEDStatus(const uint &_r, const uint &_g, const uint &_b,
                     const uint &_a)
    : r(_r), g(_g), b(_b), a(_a) {}

// ==================== LEDFrame =============================

template <class Archive>
void LEDFrame::serialize(Archive &archive, const unsigned int version) {
    archive &start;
    archive &fade;
    archive &statusList;
}

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

template <class Archive>
void LEDPlayer::serialize(Archive &archive, const unsigned int version) {
    archive &fps;
    archive &frameLists;
    archive &stripShapes;
    archive &currentStatus;
}

string LEDPlayer::list() const {
    stringstream ostr;
    ostr << "*************** LEDPlayer ***************\n";
    for (auto part_it : frameLists) {
        ostr << "[";
        for (auto frame_it : part_it) {
            ostr << "\n\t{\n\tstart: " << frame_it.start
                 << ",\n\tfade: " << frame_it.fade << ",\n\tstatus: [\n";
            for (auto status_it : frame_it.statusList) {
                ostr << "\t\t[ " << status_it.r << " , " << status_it.g << " , "
                     << status_it.b << " , " << status_it.a << " ],\n";
            }
            ostr << "\t]\n\t},\n";
        }
        ostr << "],\n";
    }
    ostr << "*****************************************\n";

    return ostr.str();
}

ostream &operator<<(ostream &os, const LEDPlayer &player) {
    os << player.list();
    return os;
}

void saveLEDPlayer(const LEDPlayer &player, const char *filename) {
    // make an archive
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "[LEDPlayer] File Not Found! ( " << filename << " )" << endl;
        return;
    }
    boost::archive::text_oarchive oa(ofs);
    oa << player;
}

bool restoreLEDPlayer(LEDPlayer &player, const char *filename) {
    // open the archive
    ifstream ifs(filename);
    if (!ifs) {
        cerr << "[LEDPlayer] File Not Found! ( " << filename << " )" << endl;
        return false;
    }
    boost::archive::text_iarchive ia(ifs);

    // restore the schedule from the archive
    ia >> player;

    return true;
}

LEDPlayer::LEDPlayer() : fps(0) {}

LEDPlayer::LEDPlayer(const int &_fps,
                     const vector<vector<LEDFrame>> &_frameLists,
                     const vector<int> &_stripShapes) {
    fps = _fps;
    frameLists.assign(_frameLists.begin(), _frameLists.end());
    stripShapes.assign(_stripShapes.begin(), _stripShapes.end());

    frameIds.resize(stripShapes.size());
    fill(frameIds.begin(), frameIds.end(), -1);
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

void LEDPlayer::calculateFrameIds(const int &millisec) {
    // Find current frame by time id
    for (unsigned int i = 0; i < frameIds.size(); i++) {
        const vector<LEDFrame> &frameList = frameLists[i];

        // Do not process if no frame is empty or time is invalid
        if (frameList.size() == 0 || millisec < 0) {
            frameIds[i] = -1;
            continue;
        }
        // Use last frame if time exceeded the limit
        if (millisec >= frameList.back().start) {
            frameIds[i] = frameLists[i].size() - 1;
            continue;
        }

        // Check if simple move is available
        const int currFrameId = frameIds[i];
        if (currFrameId >= 0 && millisec >= frameList[currFrameId].start) {
            if ((unsigned int)currFrameId < frameList.size() - 1 &&
                millisec < frameList[currFrameId + 1].start) {
                continue;
            }
            if ((unsigned int)currFrameId < frameList.size() - 2 &&
                millisec >= frameList[currFrameId + 1].start &&
                millisec < frameList[currFrameId + 2].start) {
                frameIds[i] = currFrameId + 1;
                continue;
            }
        }

        // Otherwise, apply binary search
        int left = 0;
        int right = frameList.size() - 1;
        while (left < right) {
            int mid = (left + right) >> 1;
            if (millisec < frameList[mid].start) {
                right = mid - 1;
            } else if (millisec > frameList[mid].start) {
                left = mid + 1;
            } else {
                left = mid;
                break;
            }
        }

        if (millisec < frameList[left].start) --left;

        frameIds[i] = left;
    }
}

vector<LEDStatus> LEDPlayer::interpolateFadeFrame(const LEDFrame &origin,
                                                  const LEDFrame &target,
                                                  const float &rate) {
    vector<LEDStatus> statusList(0);
    for (unsigned int i = 0; i < origin.statusList.size(); i++) {
        const LEDStatus &originLEDStatus = origin.statusList[i];
        const LEDStatus &targetLEDStatus = target.statusList[i];

        const int &r = (int)round((1 - rate) * (float)originLEDStatus.r +
                                  rate * (float)targetLEDStatus.r);
        const int &g = (int)round((1 - rate) * (float)originLEDStatus.g +
                                  rate * (float)targetLEDStatus.g);
        const int &b = (int)round((1 - rate) * (float)originLEDStatus.b +
                                  rate * (float)targetLEDStatus.b);
        const int &a = (int)round(((1 - rate) * (float)originLEDStatus.a +
                                   rate * (float)targetLEDStatus.a));
        statusList.push_back(LEDStatus(r, g, b, a));
    }
    return statusList;
}

vector<vector<int>> LEDPlayer::castStatusLists(
    const vector<vector<LEDStatus>> statusLists) {
    vector<vector<int>> castedLists(statusLists.size());
    for (unsigned int i = 0; i < statusLists.size(); i++) {
        const vector<LEDStatus> &statusList = statusLists[i];
        for (unsigned int j = 0; j < statusList.size(); j++) {
            const LEDStatus &status = statusList[j];
	   // if(i==0&&j==0){cerr<<"[LEDPlayer] StatusList"<<" R "<<status.r<<" G "<<status.g<<" B "<<status.b<<"\n";}
            castedLists[i].push_back((status.r << 24) + (status.g << 16) +
                                     (status.b << 8) + (status.a << 0));
        }
    }

    return castedLists;
}

// NOTE: call init in restart
void LEDPlayer::init() {
    frameIds.resize(stripShapes.size());
    fill(frameIds.begin(), frameIds.end(), -1);

    controller.init(stripShapes);
    printf("LEDPlayer init, Num: %d\n", (int)stripShapes.size());
}

void LEDPlayer::delayDisplay(const bool *delayingDisplay) {
    vector<vector<LEDStatus>> statusLists;

    // Let OF lightall for 1/5 times of delayTime
    if (*delayingDisplay) {
	cerr<<"[LEDPlayer] Now Delaying Display\n";
        statusLists.clear();
        for (unsigned int i = 0; i < frameIds.size(); i++) {
            vector<LEDStatus> statusList;
            for (int j = 0; j < stripShapes[i]; j++) {
                statusList.push_back(LEDStatus(100, 0, 0, 255));
            }
            statusLists.push_back(statusList);
        }
        controller.sendAll(castStatusLists(statusLists));
    } else {
	cerr<<"[LEDPlayer] No Delaying Display\n";
        statusLists.clear();
        for (unsigned int i = 0; i < frameIds.size(); i++) {
            statusLists.push_back(vector<LEDStatus>(stripShapes[i]));
        }
        controller.sendAll(castStatusLists(statusLists));
    }
}

/*void *LEDPlayer::loop_helper(void *context, StateMachine* fsm){
        LEDPlayer* ledptr=(LEDPlayer *)context;
        ledptr->loop(fsm);
	return NULL;
}*/

void LEDPlayer::darkAll(){
        
    vector<vector<LEDStatus>> statusLists;
       	statusLists.clear();
            for (unsigned int i = 0; i < frameIds.size(); i++) {
                // dark all
                statusLists.push_back(vector<LEDStatus>(stripShapes[i], LEDStatus()));
            }
            controller.sendAll(castStatusLists(statusLists));
        cerr<<"[LEDPlayer] Dark All\n";
	    return; 
}
void LEDPlayer::loop(StateMachine *fsm) {
    cerr<<"[LEDPlayer] Entering Loop\n";
    timeval currentTime;
    vector<vector<LEDStatus>> statusLists;
#ifdef PLAYER_DEBUG
    ofstream logFile("/tmp/led.log");
#endif
    //cerr<<"[LED Loop]Current State: "<<fsm->getCurrentState()<<"\n";
    while (true) {  
	    timeval lastTime = currentTime;
        gettimeofday(&currentTime, NULL);
        float fps = 1000000.0 / getElapsedTime(lastTime, currentTime);
	    //cerr<<"[LED Loop] fps: "<<fps<<"\n";
	    //cerr<<"[LED Loop] CurrentState: "<<fsm->getCurrentState()<<endl;
        if (fsm->getCurrentState() == S_STOP) {
	        cerr<<"[LEDPlayer] Now Stopped\n";
	        cerr<<"[LEDPlayer] Break Loop\n";
            break;
        }
        if (fsm->getCurrentState() == S_PAUSE){
	        cerr<<"[LEDPlayer] Now Paused\n";
	        cerr<<"[LEDPlayer] Break Loop\n";
            break;
        }
	    if (fsm->getCurrentState()==S_PLAY) {
	        //cerr<<"[LED Loop]Now Playing\n";	
            const long elapsedTime =
                getElapsedTime(fsm->data.baseTime, currentTime);  // us
            // const int currentTimeId = getTimeId(elapsedTime);
	        //  fprintf(stderr,"[LED Loop] ElapsedTime [%d]\n",elapsedTime);
	        calculateFrameIds(elapsedTime / 1000l);
            statusLists.clear();
            bool ended = true;
            for (unsigned int i = 0; i < frameIds.size(); i++) {
                const int &frameId = frameIds[i];
                const vector<LEDFrame> &frameList = frameLists[i];
	            //	fprintf(stderr,"[LED Loop] frameIDsSize [%d]\n",frameIds.size());
                if (frameId < 0) {
                    // use dark frame for invalid time or non-exist part
                    statusLists.push_back(vector<LEDStatus>(stripShapes[i]));
                    continue;
                }
	            //	fprintf(stderr, "[LED Loop] framelistSize [%d], frameId [%d]\n", (int)frameList.size(), frameId);
                if (frameId != (int)frameList.size() - 1) {
                    ended = false;
                }

                const LEDFrame &frame = frameList[frameId];
                if (frame.fade && (frameId + 1) < frameList.size()) {
                    const long startTime = frameList[frameId].start;    // ms
                    const long endTime = frameList[frameId + 1].start;  // ms
                    const float rate =
                        (float)((elapsedTime / 1000.0) - startTime) /
                        (float)(endTime - startTime);
                    statusLists.push_back(interpolateFadeFrame(
                        frame, frameList[frameId + 1], rate));
                } else {
                    statusLists.push_back(frameList[frameId].statusList);
                }
            }

            controller.sendAll(castStatusLists(statusLists));
	        //cerr<<"[LED Loop] Status Sent\n";
#ifdef PLAYER_DEBUG
            char buf[20000];
            sprintf(buf, "[LED] Time: %s\n",
                    parseMicroSec(elapsedTime).c_str());
            for (int i = 0; i < statusLists.size(); i++) {
                sprintf(buf + strlen(buf), "LED%2d:\n", i);
                for (int j = 0; j < statusLists[i].size(); j++) {
                    sprintf(buf + strlen(buf), "\r%3d:[%3d,%3d,%3d,%3d]\n", j,
                            statusLists[i][j].r, statusLists[i][j].g,
                            statusLists[i][j].b, statusLists[i][j].a);
                }
            }
            logFile << buf;
#endif
            if (ended) {
            	cerr << "[LEDPlayer] Ended\n";
	    	    break;
            }
            this_thread::yield();
        }
    }
    cerr << "[LED] finish\n";
   // controller.finish();
#ifdef PLAYER_DEBUG
    logFile << "[LED] finish\n";
    logFile.close();
#endif
    //fsm->setState(S_STOP);
}
