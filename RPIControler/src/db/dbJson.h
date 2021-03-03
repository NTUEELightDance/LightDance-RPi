/****************************************************************************
  FileName     [ dbJson.h ]
  PackageName  [ db ]
  Synopsis     [ Define database JSON classes ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef DB_JSON_H
#define DB_JSON_H

#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

class RPIMgr {
    
public:
    RPIMgr() {
        _startTime = 0;
        _loaded = false;
        _playing = false;
        /*
        if (!loadBoardConfig()) {
            cerr << "Error: cannot load board_config.json... exiting" << endl;
            exit(1);
        }
        if (!loadMapping()) {
            cerr << "Error: cannot load dancer.json... exiting" << endl;
            exit(1);
        }
         */
    }
    ~RPIMgr() {}
    bool setDancer(const string& name);
    bool load(const string& path);
    void play(bool givenTime, size_t startTime);
    void pause() {_playing = false;}
    void stop() {_startTime = 0;}
    void testEL() const;
    void testLED() const;
    void listPart() const;
    //void setStartTime(size_t startTime) {_startTime = startTime;}
    size_t getStartTime() const {return _startTime;}
    //void sigHandler(int sig);
    bool statusLight() const;
    //bool get_playing() const {return _playing;}
private:
    size_t _startTime;
    bool _loaded;
    json _ctrlJSON;
    json _ELparts;
    json _LEDparts;
    string _name;
    bool _playing;
    
    //function
    void lightOneStatus(const json& status) const;
    size_t getFrameId() const;
    json getFadeStatus(const size_t& currentTime, const json& firstStatus, const json& secondStatus) const;
    //json getStatus(size_t currentTime,
    /*
    bool loadBoardConfig() {
        cout << "loading ./data/board_config.json..." << endl;
        ifstream infile("./data/board_config.json");
        if (!infile)
            return false;
        json boardConfigJSON;
        infile >> boardConfigJSON;
        bool find = false;
        for (json::iterator it = boardConfigJSON.begin(); it != boardConfigJSON.end(); ++it) {
            if (it.key() == "name") {
                find = true;
                _name = it.value();
                break;
            }
        }
        if (!find) {
            cerr << "Error: no key named \"name\" in board_config.json" << endl;
            return false;
        }
        return true;
    }
     */
};


/*
struct DBSortKey;
struct DBSortValue;

class DBJsonElem
{
public:
   // TODO: define constructor & member functions on your own
   DBJsonElem() {}
   DBJsonElem(const string& k, int v): _key(k), _value(v) {}

   const string& key() const { return _key; }
   string& key() { return _key; }
   const int& value() const { return _value; }
   int& value() { return _value; }

   friend ostream& operator << (ostream&, const DBJsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class DBJson
{
public:
   // TODO: define constructor & member functions on your own
   DBJson() {}
   bool add(const DBJsonElem&); // return false if key is repeated
   float ave() const;
   int max(size_t&) const;
   int min(size_t&) const;
   void sort(const DBSortKey&);
   void sort(const DBSortValue&);
   int sum() const;

   // Basic access functions
   void reset();
   size_t size() const { return _obj.size(); }
   bool empty() const { return _obj.empty(); }
   DBJsonElem& operator [] (size_t i) { return _obj[i]; }
   const DBJsonElem& operator [] (size_t i) const { return _obj[i]; }

   // TODO modify these two functions according to the comments
   // return true if JSON file hasn't been read in
   bool operator !()
    {
       if (have_read)/////////wwwwwwwwwww
           return false;
        return true;
    }
   // return this if JSON file has been read in; return NLL if not.
   operator void* () const
    {
        if (have_read)
            return (void*)this;
        return NULL;                    /////////wwwwwwwwwww
    }

   // Read DBJson
   friend istream& operator >> (istream& is, DBJson& j);
   friend ostream& operator << (ostream& os, const DBJson& j);
    bool have_read = false;
private:
   vector<DBJsonElem>       _obj;  // DO NOT change this definition.
                                   // Use it to store JSON elements.
};

struct DBSortKey
{
   bool operator() (const DBJsonElem& m1, const DBJsonElem& m2) const {
      return (m1.key() < m2.key());
   }
};

struct DBSortValue
{
   bool operator() (const DBJsonElem& m1, const DBJsonElem& m2) const {
      return (m1.value() < m2.value());
   }
};
*/


#endif // DB_JSON_H
