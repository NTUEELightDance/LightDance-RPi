CXX = g++
OBJS = ./build/cmdMgr.o ./build/rpiMgr.o
INC = ./include
LIB = ./lib
OBJDIR = ./build
EXEDIR = ./bin
CXXFLAGS=./src/main.cpp

all: directories $(EXEDIR)/controller

directories: $(OBJDIR) $(EXEDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXEDIR):
	mkdir -p $(EXEDIR)

$(OBJDIR)/%.o: ./src/%.cpp
	$(CXX) -c -o $@ $< -g -I$(INC)

$(EXEDIR)/controller: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ./bin/controller -I$(INC) $(OBJS)

clean:
	rm -f $(OBJDIR)/*
	rm -f $(EXEDIR)/*