/****************************************************************************
  FileName     [ dbCmd.h ]
  PackageName  [ db ]
  Synopsis     [ Define database command classes ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef RPI_CMD_H
#define RPI_CMD_H

#include "cmdParser.h"

CmdClass(PlayCmd);
//CmdClass(PauseCmd);
CmdClass(StopCmd);
CmdClass(LoadCmd);
CmdClass(ELTestCmd);
CmdClass(LEDTestCmd);
CmdClass(ListCmd);
CmdClass(SetDancerCmd);
CmdClass(StatusLightCmd);

#endif // RPI_CMD_H

