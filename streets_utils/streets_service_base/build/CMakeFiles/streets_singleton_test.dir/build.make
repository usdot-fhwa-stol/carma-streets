# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/ode/carma-streets/streets_utils/streets_singleton

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ode/carma-streets/streets_utils/streets_singleton/build

# Include any dependencies generated for this target.
include CMakeFiles/streets_singleton_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/streets_singleton_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/streets_singleton_test.dir/flags.make

CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o: CMakeFiles/streets_singleton_test.dir/flags.make
CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o: ../test/test_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/test/test_main.cpp

CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/test/test_main.cpp > CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.i

CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/test/test_main.cpp -o CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.s

CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.requires

CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.provides: CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_test.dir/build.make CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.provides

CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.provides.build: CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o


CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o: CMakeFiles/streets_singleton_test.dir/flags.make
CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o: ../test/test_streets_configuration.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/test/test_streets_configuration.cpp

CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/test/test_streets_configuration.cpp > CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.i

CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/test/test_streets_configuration.cpp -o CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.s

CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.requires

CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.provides: CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_test.dir/build.make CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.provides

CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.provides.build: CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o


CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o: CMakeFiles/streets_singleton_test.dir/flags.make
CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o: ../test/test_streets_singleton.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/test/test_streets_singleton.cpp

CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/test/test_streets_singleton.cpp > CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.i

CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/test/test_streets_singleton.cpp -o CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.s

CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.requires

CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.provides: CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_test.dir/build.make CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.provides

CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.provides.build: CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o


CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o: CMakeFiles/streets_singleton_test.dir/flags.make
CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o: ../src/streets_configuration_exception.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration_exception.cpp

CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration_exception.cpp > CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.i

CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration_exception.cpp -o CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.s

CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.requires

CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.provides: CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_test.dir/build.make CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.provides

CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.provides.build: CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o


CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o: CMakeFiles/streets_singleton_test.dir/flags.make
CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o: ../src/streets_configuration.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration.cpp

CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration.cpp > CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.i

CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration.cpp -o CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.s

CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.requires

CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.provides: CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_test.dir/build.make CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.provides

CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.provides.build: CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o


# Object files for target streets_singleton_test
streets_singleton_test_OBJECTS = \
"CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o" \
"CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o" \
"CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o" \
"CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o" \
"CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o"

# External object files for target streets_singleton_test
streets_singleton_test_EXTERNAL_OBJECTS =

streets_singleton_test: CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o
streets_singleton_test: CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o
streets_singleton_test: CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o
streets_singleton_test: CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o
streets_singleton_test: CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o
streets_singleton_test: CMakeFiles/streets_singleton_test.dir/build.make
streets_singleton_test: /usr/lib/x86_64-linux-gnu/libboost_system.so
streets_singleton_test: /usr/lib/x86_64-linux-gnu/libboost_thread.so
streets_singleton_test: /usr/local/lib/libspdlog.a
streets_singleton_test: libstreets_singleton_lib.a
streets_singleton_test: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
streets_singleton_test: /usr/lib/x86_64-linux-gnu/libboost_system.so
streets_singleton_test: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
streets_singleton_test: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
streets_singleton_test: CMakeFiles/streets_singleton_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable streets_singleton_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/streets_singleton_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/streets_singleton_test.dir/build: streets_singleton_test

.PHONY : CMakeFiles/streets_singleton_test.dir/build

CMakeFiles/streets_singleton_test.dir/requires: CMakeFiles/streets_singleton_test.dir/test/test_main.cpp.o.requires
CMakeFiles/streets_singleton_test.dir/requires: CMakeFiles/streets_singleton_test.dir/test/test_streets_configuration.cpp.o.requires
CMakeFiles/streets_singleton_test.dir/requires: CMakeFiles/streets_singleton_test.dir/test/test_streets_singleton.cpp.o.requires
CMakeFiles/streets_singleton_test.dir/requires: CMakeFiles/streets_singleton_test.dir/src/streets_configuration_exception.cpp.o.requires
CMakeFiles/streets_singleton_test.dir/requires: CMakeFiles/streets_singleton_test.dir/src/streets_configuration.cpp.o.requires

.PHONY : CMakeFiles/streets_singleton_test.dir/requires

CMakeFiles/streets_singleton_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/streets_singleton_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/streets_singleton_test.dir/clean

CMakeFiles/streets_singleton_test.dir/depend:
	cd /home/ode/carma-streets/streets_utils/streets_singleton/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ode/carma-streets/streets_utils/streets_singleton /home/ode/carma-streets/streets_utils/streets_singleton /home/ode/carma-streets/streets_utils/streets_singleton/build /home/ode/carma-streets/streets_utils/streets_singleton/build /home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles/streets_singleton_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/streets_singleton_test.dir/depend

