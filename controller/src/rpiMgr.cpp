#include <fstream>
#include <iomanip>
#include <sys/time.h>
#include <time.h>
#include <thread>
#include <chrono>
#include <math.h>

#include "rpiMgr.h"
#include "definition.h"
#include "utils.h"
#include "EL.h"

EL el1(16, 0x40), el2(8, 0x60);

// TODO
static long getsystime() // ms
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    //return tv.tv_usec/1000;
}

bool RPiMgr::setDancer()
{
    string path = "../data/" + _dancerName + ".json";
    ifstream infile(path.c_str());
    if (!infile)
        return false;
    json j;
    infile >> j;
    _ELparts.clear();
    _LEDparts.clear();
    for (json::iterator it = j.begin(); it != j.end(); ++it)
    {
        if (it.key() == "ELPARTS")
            _ELparts = it.value();
        if (it.key() == "LEDPARTS")
            _LEDparts = it.value();
    }
    return true;
}

void RPiMgr::load(const string &path)
{
    cout << "Loading " << path << endl;
    ifstream infile(path.c_str());
    if (!infile)
    {
        cerr << "Error: failed cannot open file " << path << endl;
        return;
    }
    infile >> _ctrlJson;
    _loaded = true;
    cout << "success" << endl;
}

void RPiMgr::play(bool givenStartTime, unsigned start, unsigned delay)
{
    long timeIntoFunc = getsystime();
    if (!_loaded)
    {
        cerr << "Error: play failed, need to load first" << endl;
        return;
    }
    //  TODO need to handle whenToPlay

    if (_ctrlJson.size() == 0)
    {
        cout << "Warning: control.json is empty" << endl;
        cout << "end of playing" << endl;
        cout << "success" << endl;
        return;
    }
    if (givenStartTime)
        _startTime = start;
    if (_startTime > _ctrlJson[_ctrlJson.size() - 1]["start"])
    {
        cout << "Warning: startTime excess totalTime" << endl;
        cout << "end of playing" << endl;
        cout << "success" << endl;
        return;
    }
    size_t currentFrameId = getFrameId();
    //cout << "FrameId: " << currentFrameId << endl;

    long hadDelay = getsystime() - timeIntoFunc;
    if (hadDelay < delay)
        std::this_thread::sleep_for(std::chrono::milliseconds(delay - hadDelay));
    /*
    if (_ctrlJson[currentFrameId]["fade"])
        lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId+1]));
    else
        lightOneStatus(_ctrlJson[currentFrameId]["status"]);
    */
    cout << "start play" << endl;
    long sysStartTime = getsystime();
    //cout << "hi" << endl;
    //_playing = true;

    while (1)
    {
        //cout << "Time: " << _startTime << " FrameId: " << currentFrameId << endl;
        if (_startTime >= _ctrlJson[_ctrlJson.size() - 1]["start"])
        {
            lightOneStatus(_ctrlJson[_ctrlJson.size() - 1]["status"]);
            cout << "end of playing" << endl;
            _playing = false;
            break;
        }
        if (_startTime >= _ctrlJson[currentFrameId + 1]["start"])
        {
            currentFrameId++;
            if (_ctrlJson[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
            else
                lightOneStatus(_ctrlJson[currentFrameId]["status"]);
        }
        else
        {
            if (_ctrlJson[currentFrameId]["fade"])
                lightOneStatus(getFadeStatus(_startTime, _ctrlJson[currentFrameId], _ctrlJson[currentFrameId + 1]));
        }
        /*
         if (_ctrlJSON[currentFrameId]["fade"]) {
         cout << "fade" << endl;
         lightOneStatus(getFadeStatus(_startTime, _ctrlJSON[currentFrameId], _ctrlJSON[currentFrameId+1]));
         }
         else
         lightOneStatus(_ctrlJSON[currentFrameId]["status"]);
         */
        _startTime = (getsystime() - sysStartTime);
    }
    //
    cout << "success" << endl;
}

void RPiMgr::stop()
{
    _startTime = 0;
    cout << "success" << endl;
}

void RPiMgr::statuslight()
{
    ifstream infile("../data/status.json");
    if (!infile)
    {
        //cerr << "Error: cannot open ./data/status/json" << endl;
        //sendToRPIClient(false, "Error: cannot open ./data/status/json");
        cout << "Error: cannot open ../data/status.json" << endl;
        return;
    }
    json status;
    infile >> status;
    lightOneStatus(status);
    cout << "success" << endl;
}

void RPiMgr::list()
{
    cout << "ELPARTS:" << endl;
    for (json::const_iterator it = _ELparts.begin(); it != _ELparts.end(); ++it)
    {
        cout << '\t' << setw(15) << left << it.key()
             << it.value() << endl;
    }
    cout << "LEDPARTS:" << endl;
    for (json::const_iterator it = _LEDparts.begin(); it != _LEDparts.end(); ++it)
    {
        cout << '\t' << setw(15) << left << it.key()
             << it.value() << endl;
    }
}

void RPiMgr::eltest()
{
}

void RPiMgr::ledtest()
{
}

void RPiMgr::quit()
{
    cout << "success" << endl;
}

size_t
RPiMgr::getFrameId() const
{
    size_t totalFrame = _ctrlJson.size();
    if (totalFrame == 0)
    {
        //cout << "Error: totalFrame is 0" << endl;
        cout << "Warning: totalFrame is 0" << endl;
        return 0;
    }
    if (_startTime > _ctrlJson[totalFrame - 1]["start"])
    {
        cout << "Warning: startTime exceed total time" << endl;
        return 0;
    }
    size_t first = 0;
    size_t last = totalFrame - 1;
    while (first < last)
    {
        size_t mid = (first + last) / 2;
        if (_startTime < _ctrlJson[mid]["start"])
            last = mid - 1;
        else if (_startTime > _ctrlJson[mid]["start"])
            first = mid + 1;
        else
            return mid;
    }
    if (_ctrlJson[first]["start"] > _startTime)
        first--;
    return first;
}

json RPiMgr::getFadeStatus(const size_t &currentTime, const json &firstFrame, const json &secondFrame) const
{
    size_t firstTime = firstFrame["start"];
    size_t secondTime = secondFrame["start"];
    float rate = (float)(currentTime - firstTime) / (float)(secondTime - firstTime);
    json ret;
    for (json::const_iterator it = firstFrame["status"].begin(); it != firstFrame["status"].end(); ++it)
    {
        if (_ELparts.find(it.key()) != _ELparts.end())
        {
            json::const_iterator it2 = secondFrame["status"].find(it.key());
            float temp = (1 - rate) * float(it.value()) + rate * float(it2.value());
            ret[it.key()] = roundf(temp * 10) / 10.0;
        }
        else if (_LEDparts.find(it.key()) != _LEDparts.end())
        {
            json LEDinfo;
            LEDinfo["src"] = it.value()["src"];
            json::const_iterator it2 = secondFrame["status"].find(it.key());
            float temp = (1 - rate) * float(it.value()["alpha"]) + rate * float(it2.value()["alpha"]);
            LEDinfo["alpha"] = roundf(temp * 10) / 10.0;
            if (it.value()["src"] != it2.value()["src"])
            {
                cout << "Error: the src in two fade frame is different" << endl;
            }
            ret[it.key()] = LEDinfo;
        }
        else
            cerr << "Error: light name " << it.key() << " not found!" << endl;
    }
    return ret;
}

void RPiMgr::lightOneStatus(const json &status) const
{
    for (json::const_iterator it = status.begin(); it != status.end(); ++it)
    {
        json::const_iterator temp = _ELparts.find(it.key());
        if (temp != _ELparts.end())
        { //ELparts
            cout << "ELlightName: " << it.key() << ", "
                 << "alpha: " << getELBright(it.value()) << ", "
                 << "number: " << temp.value() << endl;
        }
        else
        {
            temp = _LEDparts.find(it.key());
            if (temp != _LEDparts.end())
            { //LEDparts
                cout << "LEDlightName: " << it.key() << ", "
                     << "alpha: " << (it.value()["alpha"]) << ", "
                     << "number: " << temp.value() << ", "
                     << "src: " << (it.value()["src"]) << endl;
            }
            else
                cerr << "Error: lightName " << it.key() << " not found!" << endl;
        }
    }
}
