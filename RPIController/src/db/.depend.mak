dbJson.o: dbJson.cpp dbJson.h ../../include/nlohmann/json.hpp \
  ../../include/util.h
rpiCmd.o: rpiCmd.cpp ../../include/util.h rpiCmd.h \
  ../../include/cmdParser.h ../../include/cmdCharDef.h dbJson.h \
  ../../include/nlohmann/json.hpp
