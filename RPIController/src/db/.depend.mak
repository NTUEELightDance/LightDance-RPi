RPIMgr.o: RPIMgr.cpp RPIMgr.h ../../include/nlohmann/json.hpp \
  ../../include/util.h
rpiCmd.o: rpiCmd.cpp ../../include/util.h rpiCmd.h \
  ../../include/cmdParser.h ../../include/cmdCharDef.h RPIMgr.h \
  ../../include/nlohmann/json.hpp
