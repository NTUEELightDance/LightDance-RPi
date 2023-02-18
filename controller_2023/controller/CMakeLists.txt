cmake_minimum_required(VERSION 3.10)
project(controller)

set(SOURCE src)
set(INSTALL ${CMAKE_BINARY_DIR}/../install)

set(INCLUDES inc)
set(LIBRARIES boost_serialization)

set(HARDWARE_INCLUDES ../hardware/inc)
set(HARDWARE_LIBRARIES hardware)

set(EXECUTABLES
    list
    load
    boardinfo
    playerctl
    ledtest
    oftest
    player
    socket)

# list
add_executable(
  list ${SOURCE}/commands/list.cpp ${SOURCE}/commands/player.cpp
       ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
target_include_directories(list PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(list ${LIBRARIES} ${HARDWARE_LIBRARIES})

# load
add_executable(
  load ${SOURCE}/commands/load.cpp ${SOURCE}/commands/player.cpp
       ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
target_include_directories(load PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(load ${LIBRARIES} ${HARDWARE_LIBRARIES})

# boardinfo
add_executable(
  boardinfo ${SOURCE}/commands/boardinfo.cpp ${SOURCE}/commands/player.cpp
            ${SOURCE}/player/LEDPlayer.cpp ${SOURCE}/player/OFPlayer.cpp)
target_include_directories(boardinfo PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(boardinfo ${LIBRARIES} ${HARDWARE_LIBRARIES})

# playerctl
add_executable(playerctl ${SOURCE}/commands/playerctl.cpp)
target_include_directories(playerctl PUBLIC ${INCLUDES})
target_link_libraries(playerctl ${LIBRARIES})

# ledtest
add_executable(ledtest ${SOURCE}/commands/LEDTest.cpp)
target_include_directories(ledtest PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(ledtest ${LIBRARIES} ${HARDWARE_LIBRARIES})

# oftest
add_executable(oftest ${SOURCE}/commands/OFTest.cpp)
target_include_directories(oftest PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(oftest ${LIBRARIES} ${HARDWARE_LIBRARIES})

# player
add_executable(
  player ${SOURCE}/player/playLoop.cpp ${SOURCE}/player/LEDPlayer.cpp
         ${SOURCE}/player/OFPlayer.cpp)
target_include_directories(player PUBLIC ${INCLUDES} ${HARDWARE_INCLUDES})
target_link_libraries(player ${LIBRARIES} ${HARDWARE_LIBRARIES})

# socket
add_executable(socket ${SOURCE}/socket/client.cpp)
target_include_directories(socket PUBLIC ${INCLUDES})
# target_link_libraries(socket ${LIBRARIES})

# install TODO: change installation path to /usr/local
file(MAKE_DIRECTORY ${INSTALL})
install(TARGETS ${EXECUTABLES} RUNTIME DESTINATION ${INSTALL})
