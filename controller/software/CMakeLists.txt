cmake_minimum_required(VERSION 3.10)
project(controller)

find_package(Boost 1.65.1 REQUIRED COMPONENTS serialization)
find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(SOURCE src)
set(INCLUDES inc)
set(INSTALL ${CMAKE_SOURCE_DIR}/bin)

set(HARDWARE_INCLUDES ../hardware/inc)
set(HARDWARE_LIBRARIES hardware)

# umbrella
option(UMBRELLA "compile for umbrella" OFF)
if(UMBRELLA)
  message("-- Compiling for umbrella...")
  add_definitions(-DUMBRELLA)
endif()

# hardware debug
option(HARDWARE_DEBUG "compile with hardware debug" OFF)
if(HARDWARE_DEBUG)
  message("-- Compiling with hardware debug...")
  add_definitions(-DHARDWARE_DEBUG)
endif()

# player debug
option(PLAYER_DEBUG "compile with player debug" OFF)
if(PLAYER_DEBUG)
  message("-- Compiling with player debug...")
  add_definitions(-DPLAYER_DEBUG)
endif()

set(EXECUTABLES
  list
  load
  parttest
  playerctl
  player

  # boardinfo
  # ledtest
  # oftest
  # color_palette
  # socket
)

include_directories(${INCLUDES} ${HARDWARE_INCLUDES})

add_library(playerLIB STATIC
  ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp
  ${SOURCE}/commands/player.cpp )
  # ${SOURCE}/commands/player.cpp ${SOURCE}/player/StateMachine.cpp)
  add_library(StateMachineLIB STATIC ${SOURCE}/player/StateMachine.cpp ${SOURCE}/player/FSM_Common.cpp)
# target_include_directories(playerLIB PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(playerLIB StateMachineLIB ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES})
target_link_libraries(StateMachineLIB playerLIB ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES})

# list
add_executable(
  list ${SOURCE}/commands/list.cpp)
# target_include_directories(list PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(list ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES} playerLIB)

# load
add_executable(
  load ${SOURCE}/commands/load.cpp)
# target_include_directories(load PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(load ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES} playerLIB)

# boardinfo
# add_executable(
# boardinfo ${SOURCE}/commands/boardinfo.cpp ${SOURCE}/commands/player.cpp
# ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
# target_include_directories(boardinfo PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
# target_link_libraries(boardinfo ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES})

# playerctl
add_executable(playerctl ${SOURCE}/commands/playerctl.cpp)
# target_include_directories(playerctl PUBLIC ${INCLUDES})
target_link_libraries(playerctl ${Boost_LIBRARIES})

# ledtest
# add_executable(
# ledtest ${SOURCE}/commands/LEDTest.cpp ${SOURCE}/commands/player.cpp
# ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
# target_include_directories(ledtest PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
# target_link_libraries(ledtest ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES})

# oftest
# add_executable(
# oftest ${SOURCE}/commands/OFTest.cpp ${SOURCE}/commands/player.cpp
# ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
# target_include_directories(oftest PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
# target_link_libraries(oftest ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES})

# parttest
add_executable(
  parttest ${SOURCE}/commands/partTest.cpp)
# target_include_directories(parttest PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(parttest ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES} playerLIB StateMachineLIB)

# color_palette
# add_executable(
# color_palette ${SOURCE}/commands/color_palette.cpp ${SOURCE}/commands/player.cpp
# ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
# target_include_directories(color_palette PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
# target_link_libraries(color_palette ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES})

# player
add_executable(
  player ${SOURCE}/player/playLoop.cpp)

set_target_properties(player PROPERTIES COMPILE_FLAGS "-pthread"
  LINK_FLAGS "-pthread")
# target_include_directories(player PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(player ${Boost_LIBRARIES} ${HARDWARE_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT} playerLIB StateMachineLIB Threads::Threads pthread)
# ${CMAKE_THREAD_LIBS_INIT} playerLIB StateMachineLIB Threads::Threads pthread)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
# socket
# add_executable(socket ${SOURCE}/socket/client.cpp)
# target_include_directories(socket PUBLIC ${INCLUDES})

# install TODO: change installation path to /usr/local
file(MAKE_DIRECTORY ${INSTALL})
install(TARGETS ${EXECUTABLES} RUNTIME DESTINATION ${INSTALL})
