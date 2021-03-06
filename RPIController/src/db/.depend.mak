dbJson.o: dbJson.cpp dbJson.h ../../include/util.h ../../include/nlohmann/json.hpp
rpiCmd.o: rpiCmd.cpp ../../include/util.h rpiCmd.h \
  ../../include/cmdParser.h ../../include/cmdCharDef.h dbJson.h
  
