#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Command {
   public:
    Command();
    void addFlag(string flag, string description);
    void help();
    bool cmdOptionExists(char** begin, char** end, const string& option);
    vector<string> getCmdOptionStr(char** begin, char** end, const string& option);
    vector<int> getCmdOptionInt(char** begin, char** end, const string& option);
    vector<float> getCmdOptionFloat(char** begin, char** end, const string& option);
    vector<long> getCmdOptionLong(char** begin, char** end, const string& option);

   private:
    map<string, string> flags;
};

Command::Command() {}

void Command::addFlag(string flag, string description) { flags[flag] = description; }

void Command::help() {
    cout << "Usage: " << endl;
    for (auto it = flags.begin(); it != flags.end(); ++it) {
        cout << it->first << " " << it->second << endl;
    }
}

bool Command::cmdOptionExists(char** begin, char** end, const string& option) {
    return std::find(begin, end, option) != end;
}

vector<string> Command::getCmdOptionStr(char** begin, char** end, const string& option) {
    char** itr = std::find(begin, end, option);
    vector<string> result;
    if (itr != end && ++itr != end) {
        while (itr != end && *itr[0] != '-') {
            result.push_back(*itr);
            ++itr;
        }
    }
    return result;
}

vector<int> Command::getCmdOptionInt(char** begin, char** end, const string& option) {
    vector<string> str = getCmdOptionStr(begin, end, option);
    vector<int> result;
    for (auto it = str.begin(); it != str.end(); ++it) {
        for (auto c = it->begin(); c != it->end(); ++c) {
            if (!isdigit(*c)) {
                return vector<int>();
            }
        }
        result.push_back(atoi(it->c_str()));
    }
    return result;
}
vector<long> Command::getCmdOptionLong(char** begin, char** end, const string& option) {
    vector<string> str = getCmdOptionStr(begin, end, option);
    vector<long> result;
    for (auto it = str.begin(); it != str.end(); ++it) {
        for (auto c = it->begin(); c != it->end(); ++c) {
            if (!isdigit(*c)) {
                return vector<long>();
            }
        }
        result.push_back(atoi(it->c_str()));
    }
    return result;
}
vector<float> Command::getCmdOptionFloat(char** begin, char** end, const string& option) {
    vector<string> str = getCmdOptionStr(begin, end, option);
    vector<float> result;
    for (auto it = str.begin(); it != str.end(); ++it) {
        for (auto c = it->begin(); c != it->end(); ++c) {
            if (!(isdigit(*c) || (*c) == ('.'))) {
                cout << *c << endl;
                return vector<float>();
            }
        }
        result.push_back(atof(it->c_str()));
    }
    return result;
}
