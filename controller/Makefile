CXX = g++
OBJS = ./build/cmdMgr.o ./build/rpiMgr.o
INC = ./include
LIB = ./lib
OBJDIR = ./build
EXEDIR = ./bin
CXXFLAGS=./src/main.cpp
CFLAGS=-std=c++11

all: directories $(EXEDIR)/controller

directories: $(OBJDIR) $(EXEDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXEDIR):
	mkdir -p $(EXEDIR)

$(OBJDIR)/%.o: ./src/%.cpp
	$(CXX) -c -o $@ $< -g $(CFLAGS) -I$(INC)

$(EXEDIR)/controller: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ./bin/controller $(CFLAGS) -I$(INC) $(OBJS)

clean:
	rm -f $(OBJDIR)/*
	rm -f $(EXEDIR)/*