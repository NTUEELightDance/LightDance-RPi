/****************************************************************************
  FileName     [ dbCmd.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include "util.h"
#include "rpiCmd.h"
#include "dbJson.h"

// Global variable
RPIMgr rpiMgr;


bool
initRPICmd()
{
   // TODO...
   vector<bool> check;
   check.push_back(cmdMgr->regCmd("PLay", 2, new PlayCmd));
   //check.push_back(cmdMgr->regCmd("PAuse", 2, new PauseCmd));
   check.push_back(cmdMgr->regCmd("STOp", 3, new StopCmd));
   check.push_back(cmdMgr->regCmd("LOad", 2, new LoadCmd));
   check.push_back(cmdMgr->regCmd("ELTest", 3, new ELTestCmd));
   check.push_back(cmdMgr->regCmd("LEDTest", 4, new LEDTestCmd));
    check.push_back(cmdMgr->regCmd("LIst", 2, new ListCmd));
    check.push_back(cmdMgr->regCmd("SETDancer", 4, new SetDancerCmd));
    check.push_back(cmdMgr->regCmd("STAtuslight", 3, new StatusLightCmd));
    for (auto i : check) {
      if (!i) {
         cerr << "Registering \"init\" commands fails... exiting" << endl;
         return false;
      }
   }
   return true;
}

//----------------------------------------------------------------------
//    PLay [(size_t time)]
//----------------------------------------------------------------------
CmdExecStatus
PlayCmd::exec(const string& option)
{
    // TODO...
    // check option
    // no option
   vector<string> tokens;
    size_t startTime;
    bool givenTime = true;
    lexOptions(option, tokens);
    if (tokens.size() == 0)
        givenTime = false;
    else if (tokens.size() == 1) {
        int value;
        if (!myStr2Int(tokens[0], value))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
        startTime = value;
    }
    else
        return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);
    cout << "press ctrl-c to pause" << endl;
    rpiMgr.play(givenTime, startTime);
    return CMD_EXEC_DONE;
    
}

void
PlayCmd::usage(ostream& os) const
{
   os << "Usage: PLay [(size_t time)]" << endl;
}

void
PlayCmd::help() const
{
   cout << setw(15) << left << "Play: "
        << "start play from time(default continue)" << endl;
}


//----------------------------------------------------------------------
//    PAuse
//----------------------------------------------------------------------
/*
CmdExecStatus
RPIPauseCmd::exec(const string& option)
{  
    // check option
    if (!CmdExec::lexNoOption(option))
        return CMD_EXEC_ERROR;
    cout << "pause" << endl;
    return CMD_EXEC_DONE;
}

void
RPIPauseCmd::usage(ostream& os) const
{     
   os << "Usage: RPIPAuse" << endl;
}

void
RPIPauseCmd::help() const
{
   cout << setw(15) << left << "RPIPause: "
        << "pause execution" << endl;
}
*/

//----------------------------------------------------------------------
//    STOp
//----------------------------------------------------------------------
CmdExecStatus
StopCmd::exec(const string& option)
{  
    // check option
    if (!CmdExec::lexNoOption(option))
        return CMD_EXEC_ERROR;
    cout << "stop" << endl;
    rpiMgr.stop();
    return CMD_EXEC_DONE;
}

void
StopCmd::usage(ostream& os) const
{     
   os << "Usage: STOp" << endl;
}

void
StopCmd::help() const
{
   cout << setw(15) << left << "Stop: "
        << "stop execution" << endl;
}


//----------------------------------------------------------------------
//    LOad [(string path)]
//----------------------------------------------------------------------

CmdExecStatus
LoadCmd::exec(const string& option)
{
    const string defaultPath = "./data/control.json";    //default path
    string path = defaultPath;
    // check option
    vector<string> tokens;
    CmdExec::lexOptions(option, tokens);
    if (tokens.size() == 1)
        path = tokens[0];
    else if (tokens.size() > 1)
        return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);
    /*
    if (!CmdExec::lexNoOption(option)) {
        vector<string> tokens;
        if (!CmdExec::lexOptions(option, tokens, 1))
            return CMD_EXEC_ERROR;
        path = tokens[0];
    }
    
    cout << "load file from \"" << path << "\"" << endl;
    if (!readFile(infile, path)) {
        cerr << "Error: cannot open file \"" << path << "\"" << endl;
        return CMD_EXEC_ERROR;
    }
    cout << "success loading file from \"" << path << "\"" << endl;
    */
    if (!rpiMgr.load(path))
        return CMD_EXEC_ERROR;
    return CMD_EXEC_DONE;
}

void
LoadCmd::usage(ostream& os) const
{     
   os << "Usage: LOad [(string path)]" << endl;
}

void
LoadCmd::help() const
{
   cout << setw(15) << left << "Load: "
        << "load control file from path" << endl;
}


//----------------------------------------------------------------------
//    ELTest <number> [alpha]
//----------------------------------------------------------------------
CmdExecStatus       //not finished
ELTestCmd::exec(const string& option)
{
    vector<string> tokens;
    CmdExec::lexOptions(option, tokens);
    if (tokens.size() == 0) {
        cerr << "Error: Missing Option" << endl;
        return CMD_EXEC_ERROR;
    }
    else if (tokens.size() == 1) {
        int number;
        if (!myStr2Int(tokens[0], number))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
        cout << "ELTest: number=" << number << endl;
    }
    else if (tokens.size() == 2) {
        int number;
        if (!myStr2Int(tokens[0], number))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
        // need to check if alpha is valid
        cout << "ELTest: number=" << number << " alpha=" << tokens[1] << endl;
    }
    else
        return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[2]);
    return CMD_EXEC_DONE;
}

void
ELTestCmd::usage(ostream& os) const
{
    os << "Usage: ELTest <(size_t number)> [(float alpha)]" << endl;
}

void
ELTestCmd::help() const
{
    cout << setw(15) << left << "ELTest: "
    << "test EL" << endl;
}

//----------------------------------------------------------------------
//    RPITESTLED <number> [src] [alpha]
//----------------------------------------------------------------------
CmdExecStatus       //not finished
LEDTestCmd::exec(const string& option)
{
    vector<string> tokens;
    CmdExec::lexOptions(option, tokens);
    if (tokens.size() == 0) {
        cerr << "Error: Missing Option" << endl;
        return CMD_EXEC_ERROR;
    }
    else if (tokens.size() == 1) {
        int number;
        if (!myStr2Int(tokens[0], number))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
        cout << "LEDTest: number=" << number << endl;
    }
    else if (tokens.size() == 2) {
        int number;
        if (!myStr2Int(tokens[0], number))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
        // need to check if src is valid
        cout << "LEDTest: number=" << number << " src=" << tokens[1] << endl;
    }
    else if (tokens.size() == 3) {
        int number;
        if (!myStr2Int(tokens[0], number))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
        // need to check if src and alpha is valid
        cout << "LEDTest: number=" << number << " src=" << tokens[1] << " src=" << tokens[2] << endl;
    }
    else
        return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
    return CMD_EXEC_DONE;
}

void
LEDTestCmd::usage(ostream& os) const
{
    os << "Usage: LEDTest <(size_t number)> [(string src)] [(float alpha)]" << endl;
}

void
LEDTestCmd::help() const
{
    cout << setw(15) << left << "LEDTest: "
    << "test LED" << endl;
}

//----------------------------------------------------------------------
//    LIst
//----------------------------------------------------------------------

CmdExecStatus
ListCmd::exec(const string& option)
{
    if (!CmdExec::lexNoOption(option))
        return CMD_EXEC_ERROR;
    rpiMgr.listPart();
    return CMD_EXEC_DONE;
}

void
ListCmd::usage(ostream& os) const
{
    os << "Usage: RPILIst" << endl;
}

void
ListCmd::help() const
{
    cout << setw(15) << left << "RPIList: "
    << "list part mapping" << endl;
}

//----------------------------------------------------------------------
//    SETDancer <string>
//----------------------------------------------------------------------

CmdExecStatus
SetDancerCmd::exec(const string& option)
{
    vector<string> tokens;
    CmdExec::lexOptions(option, tokens);
    if (tokens.size() < 1) {
        cerr << "Error: Missing option!" << endl;
        return CMD_EXEC_ERROR;
    }
    if (tokens.size() > 1)
        return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);
    rpiMgr.setDancer(tokens[0]);
    return CMD_EXEC_DONE;
}

void
SetDancerCmd::usage(ostream& os) const
{
    os << "Usage: SETDancer <(string Name)>" << endl;
}

void
SetDancerCmd::help() const
{
    cout << setw(15) << left << "SetDancer: "
    << "set dancer" << endl;
}

//----------------------------------------------------------------------
//    STAtuslight
//----------------------------------------------------------------------

CmdExecStatus
StatusLightCmd::exec(const string& option)
{
    if (!CmdExec::lexNoOption(option))
        return CMD_EXEC_ERROR;
    if (!rpiMgr.statusLight())
        return CMD_EXEC_ERROR;
    return CMD_EXEC_DONE;
}

void
StatusLightCmd::usage(ostream& os) const
{
    os << "Usage: SETDancer <(string Name)>" << endl;
}

void
StatusLightCmd::help() const
{
    cout << setw(15) << left << "RPIList: "
    << "list part mapping" << endl;
}
