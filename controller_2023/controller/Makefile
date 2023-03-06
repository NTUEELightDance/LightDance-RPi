CXX = g++
MAKE = make
CFLAGS = -std=c++17 -Wall
LIBS = -lpthread -lboost_serialization

SRC_DIR = src/
INC_DIR = inc/
BIN_DIR = bin/
LIB_DIR = lib/
OBJ_DIR = build/
HARDWARE_DIR = hardware/

INC = -I$(INC_DIR) -I$(HARDWARE_DIR)inc/
SRC = $(wildcard $(SRC_DIR)*/*.cpp)
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
HARDWARE_OBJ = $(wildcard $(HARDWARE_DIR)build/*.o)

all: commands player socket

# Commands
commands: list load boardinfo ledtest oftest playerctl

list: directories $(OBJ_DIR)commands/list.o $(OBJ_DIR)commands/player.o $(OBJ_DIR)player/LEDPlayer.o $(OBJ_DIR)player/OFPlayer.o
	$(CXX) -o $(BIN_DIR)list \
		$(OBJ_DIR)commands/list.o \
		$(OBJ_DIR)commands/player.o \
		$(OBJ_DIR)player/LEDPlayer.o \
		$(OBJ_DIR)player/OFPlayer.o \
		$(HARDWARE_OBJ) $(CFLAGS) $(LIBS)

load: directories $(OBJ_DIR)commands/load.o $(OBJ_DIR)commands/player.o $(OBJ_DIR)player/LEDPlayer.o $(OBJ_DIR)player/OFPlayer.o
	$(CXX) -o $(BIN_DIR)load \
		$(OBJ_DIR)commands/load.o \
		$(OBJ_DIR)commands/player.o \
		$(OBJ_DIR)player/LEDPlayer.o \
		$(OBJ_DIR)player/OFPlayer.o \
		$(HARDWARE_OBJ) $(CFLAGS) $(LIBS)

boardinfo: directories $(OBJ_DIR)commands/boardinfo.o $(OBJ_DIR)commands/player.o $(OBJ_DIR)player/LEDPlayer.o $(OBJ_DIR)player/OFPlayer.o
	$(CXX) -o $(BIN_DIR)boardinfo \
		$(OBJ_DIR)commands/boardinfo.o \
		$(OBJ_DIR)commands/player.o \
		$(OBJ_DIR)player/LEDPlayer.o \
		$(OBJ_DIR)player/OFPlayer.o \
		$(HARDWARE_OBJ) $(CFLAGS) $(LIBS)

playerctl: directories $(OBJ_DIR)commands/playerctl.o
	$(CXX) -o $(BIN_DIR)playerctl \
		$(OBJ_DIR)commands/playerctl.o \
		$(CFLAGS) $(LIBS)

ledtest: directories hardware $(OBJ_DIR)commands/LEDTest.o
	$(CXX) -o $(BIN_DIR)ledtest \
		$(OBJ_DIR)commands/LEDTest.o \
		$(HARDWARE_OBJ) $(CFLAGS) $(LIBS)

oftest: directories hardware $(OBJ_DIR)commands/OFTest.o
	$(CXX) -o $(BIN_DIR)oftest \
		$(OBJ_DIR)commands/OFTest.o \
		$(HARDWARE_OBJ) $(CFLAGS) $(LIBS)

# Players
player: directories hardware $(OBJ_DIR)commands/player.o $(OBJ_DIR)player/LEDPlayer.o $(OBJ_DIR)player/OFPlayer.o $(OBJ_DIR)player/playLoop.o
	$(CXX) -o $(BIN_DIR)player \
		$(OBJ_DIR)commands/player.o \
		$(OBJ_DIR)player/LEDPlayer.o \
		$(OBJ_DIR)player/OFPlayer.o \
		$(OBJ_DIR)player/playLoop.o \
		$(HARDWARE_OBJ) $(CFLAGS) $(LIBS)

# Socket
socket: directories $(OBJ_DIR)socket/client.o
	$(CXX) -o $(BIN_DIR)socket \
		$(OBJ_DIR)socket/client.o \
		$(CFLAGS) $(LIBS)

directories: $(OBJ_DIR) $(BIN_DIR)

hardware: directories
	$(MAKE) -C $(HARDWARE_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR) $(OBJ_DIR)commands $(OBJ_DIR)player $(OBJ_DIR)socket

$(BIN): $(OBJ)
	$(CXX) $(CFLAGS) $(OPTFLAGS) $(LDFLAGS) $^ $(LIB) -o $(BIN)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@echo ">> compiling: $<"
	$(CXX) $(CFLAGS) $(INC) $(LIB) $(LIBDEP) -c $< -o $@

clean:
	$(MAKE) -C $(HARDWARE_DIR) clean
	rm -rf $(OBJ_DIR) $(BIN_DIR)