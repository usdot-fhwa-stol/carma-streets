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
include CMakeFiles/streets_singleton_lib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/streets_singleton_lib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/streets_singleton_lib.dir/flags.make

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o: CMakeFiles/streets_singleton_lib.dir/flags.make
CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o: ../src/streets_configuration_exception.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration_exception.cpp

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration_exception.cpp > CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.i

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration_exception.cpp -o CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.s

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.requires

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.provides: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_lib.dir/build.make CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.provides

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.provides.build: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o


CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o: CMakeFiles/streets_singleton_lib.dir/flags.make
CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o: ../src/streets_configuration.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o -c /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration.cpp

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration.cpp > CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.i

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ode/carma-streets/streets_utils/streets_singleton/src/streets_configuration.cpp -o CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.s

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.requires:

.PHONY : CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.requires

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.provides: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.requires
	$(MAKE) -f CMakeFiles/streets_singleton_lib.dir/build.make CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.provides.build
.PHONY : CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.provides

CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.provides.build: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o


# Object files for target streets_singleton_lib
streets_singleton_lib_OBJECTS = \
"CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o" \
"CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o"

# External object files for target streets_singleton_lib
streets_singleton_lib_EXTERNAL_OBJECTS =

libstreets_singleton_lib.a: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o
libstreets_singleton_lib.a: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o
libstreets_singleton_lib.a: CMakeFiles/streets_singleton_lib.dir/build.make
libstreets_singleton_lib.a: CMakeFiles/streets_singleton_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libstreets_singleton_lib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/streets_singleton_lib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/streets_singleton_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/streets_singleton_lib.dir/build: libstreets_singleton_lib.a

.PHONY : CMakeFiles/streets_singleton_lib.dir/build

CMakeFiles/streets_singleton_lib.dir/requires: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration_exception.cpp.o.requires
CMakeFiles/streets_singleton_lib.dir/requires: CMakeFiles/streets_singleton_lib.dir/src/streets_configuration.cpp.o.requires

.PHONY : CMakeFiles/streets_singleton_lib.dir/requires

CMakeFiles/streets_singleton_lib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/streets_singleton_lib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/streets_singleton_lib.dir/clean

CMakeFiles/streets_singleton_lib.dir/depend:
	cd /home/ode/carma-streets/streets_utils/streets_singleton/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ode/carma-streets/streets_utils/streets_singleton /home/ode/carma-streets/streets_utils/streets_singleton /home/ode/carma-streets/streets_utils/streets_singleton/build /home/ode/carma-streets/streets_utils/streets_singleton/build /home/ode/carma-streets/streets_utils/streets_singleton/build/CMakeFiles/streets_singleton_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/streets_singleton_lib.dir/depend

