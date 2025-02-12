cmake_minimum_required(VERSION 3.20)

# set the project name
project(ChatSystem VERSION 1.0 LANGUAGES C)

# add local libraries
set(LIBSRC src/tcplib.c src/clienthelp.c)
add_library(TCPLIB STATIC ${LIBSRC})
target_include_directories(TCPLIB PUBLIC include)

# add the executable
add_executable(server src/server.c)
target_link_libraries(server PUBLIC TCPLIB)

add_executable(client src/client.c)
target_link_libraries(client PUBLIC TCPLIB)
