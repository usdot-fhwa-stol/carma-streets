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
CMAKE_SOURCE_DIR = /home/focal/workspace/repos/carma-streets/signal_opt_service

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/focal/workspace/repos/carma-streets/signal_opt_service/build

# Include any dependencies generated for this target.
include CMakeFiles/signal_opt_service.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/signal_opt_service.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/signal_opt_service.dir/flags.make

CMakeFiles/signal_opt_service.dir/src/main.cpp.o: CMakeFiles/signal_opt_service.dir/flags.make
CMakeFiles/signal_opt_service.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/focal/workspace/repos/carma-streets/signal_opt_service/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/signal_opt_service.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/signal_opt_service.dir/src/main.cpp.o -c /home/focal/workspace/repos/carma-streets/signal_opt_service/src/main.cpp

CMakeFiles/signal_opt_service.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/signal_opt_service.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/focal/workspace/repos/carma-streets/signal_opt_service/src/main.cpp > CMakeFiles/signal_opt_service.dir/src/main.cpp.i

CMakeFiles/signal_opt_service.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/signal_opt_service.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/focal/workspace/repos/carma-streets/signal_opt_service/src/main.cpp -o CMakeFiles/signal_opt_service.dir/src/main.cpp.s

CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.o: CMakeFiles/signal_opt_service.dir/flags.make
CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.o: ../src/signal_opt_service.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/focal/workspace/repos/carma-streets/signal_opt_service/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.o -c /home/focal/workspace/repos/carma-streets/signal_opt_service/src/signal_opt_service.cpp

CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/focal/workspace/repos/carma-streets/signal_opt_service/src/signal_opt_service.cpp > CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.i

CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/focal/workspace/repos/carma-streets/signal_opt_service/src/signal_opt_service.cpp -o CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.s

# Object files for target signal_opt_service
signal_opt_service_OBJECTS = \
"CMakeFiles/signal_opt_service.dir/src/main.cpp.o" \
"CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.o"

# External object files for target signal_opt_service
signal_opt_service_EXTERNAL_OBJECTS =

signal_opt_service: CMakeFiles/signal_opt_service.dir/src/main.cpp.o
signal_opt_service: CMakeFiles/signal_opt_service.dir/src/signal_opt_service.cpp.o
signal_opt_service: CMakeFiles/signal_opt_service.dir/build.make
signal_opt_service: libsignal_opt_service_lib.a
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.71.0
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.71.0
signal_opt_service: /usr/local/lib/libspdlog.a
signal_opt_service: /usr/local/lib/libstreets_service_base_lib.a
signal_opt_service: /usr/local/lib/libstreets_vehicle_list_lib.a
signal_opt_service: /usr/lib/x86_64-linux-gnu/libQt5Network.so.5.12.8
signal_opt_service: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.12.8
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.71.0
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.71.0
signal_opt_service: /usr/lib/x86_64-linux-gnu/libboost_atomic.so.1.71.0
signal_opt_service: /usr/local/lib/libspdlog.a
signal_opt_service: CMakeFiles/signal_opt_service.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/focal/workspace/repos/carma-streets/signal_opt_service/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable signal_opt_service"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/signal_opt_service.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/signal_opt_service.dir/build: signal_opt_service

.PHONY : CMakeFiles/signal_opt_service.dir/build

CMakeFiles/signal_opt_service.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/signal_opt_service.dir/cmake_clean.cmake
.PHONY : CMakeFiles/signal_opt_service.dir/clean

CMakeFiles/signal_opt_service.dir/depend:
	cd /home/focal/workspace/repos/carma-streets/signal_opt_service/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/focal/workspace/repos/carma-streets/signal_opt_service /home/focal/workspace/repos/carma-streets/signal_opt_service /home/focal/workspace/repos/carma-streets/signal_opt_service/build /home/focal/workspace/repos/carma-streets/signal_opt_service/build /home/focal/workspace/repos/carma-streets/signal_opt_service/build/CMakeFiles/signal_opt_service.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/signal_opt_service.dir/depend

