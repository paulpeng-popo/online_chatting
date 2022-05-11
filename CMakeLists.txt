cmake_minimum_required(VERSION 3.16)

# set the project name
project(net_hw3 VERSION 1.0)

# add the libraries
add_library(TCPLIB STATIC src/TcpLib.c)
target_include_directories(TCPLIB PUBLIC include/)

add_library(OTHERUTIL STATIC src/OtherUtil.c)
target_include_directories(OTHERUTIL PUBLIC include/)

# add ncurses library
# find_package(Curses REQUIRED)
# include_directories(${CURSES_INCLUDE_DIR})

# add the executable
add_executable(server src/Server.c)
target_link_libraries(server PUBLIC TCPLIB OTHERUTIL )

add_executable(client src/Client.c)
target_link_libraries(client PUBLIC TCPLIB OTHERUTIL )
# target_link_libraries(client PUBLIC TCPLIB OTHERUTIL ${CURSES_LIBRARIES} )
