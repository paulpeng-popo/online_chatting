# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build

# Include any dependencies generated for this target.
include CMakeFiles/TCPLIB.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TCPLIB.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TCPLIB.dir/flags.make

CMakeFiles/TCPLIB.dir/src/TcpLib.c.o: CMakeFiles/TCPLIB.dir/flags.make
CMakeFiles/TCPLIB.dir/src/TcpLib.c.o: ../src/TcpLib.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/TCPLIB.dir/src/TcpLib.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/TCPLIB.dir/src/TcpLib.c.o   -c /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/src/TcpLib.c

CMakeFiles/TCPLIB.dir/src/TcpLib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/TCPLIB.dir/src/TcpLib.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/src/TcpLib.c > CMakeFiles/TCPLIB.dir/src/TcpLib.c.i

CMakeFiles/TCPLIB.dir/src/TcpLib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/TCPLIB.dir/src/TcpLib.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/src/TcpLib.c -o CMakeFiles/TCPLIB.dir/src/TcpLib.c.s

# Object files for target TCPLIB
TCPLIB_OBJECTS = \
"CMakeFiles/TCPLIB.dir/src/TcpLib.c.o"

# External object files for target TCPLIB
TCPLIB_EXTERNAL_OBJECTS =

libTCPLIB.a: CMakeFiles/TCPLIB.dir/src/TcpLib.c.o
libTCPLIB.a: CMakeFiles/TCPLIB.dir/build.make
libTCPLIB.a: CMakeFiles/TCPLIB.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libTCPLIB.a"
	$(CMAKE_COMMAND) -P CMakeFiles/TCPLIB.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TCPLIB.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TCPLIB.dir/build: libTCPLIB.a

.PHONY : CMakeFiles/TCPLIB.dir/build

CMakeFiles/TCPLIB.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TCPLIB.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TCPLIB.dir/clean

CMakeFiles/TCPLIB.dir/depend:
	cd /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build /mnt/c/Users/paulpeng/Desktop/netprog/online_chatting/build/CMakeFiles/TCPLIB.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TCPLIB.dir/depend

