# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/e/sylar_Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/e/sylar_Project/build

# Include any dependencies generated for this target.
include CMakeFiles/test_fiber.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test_fiber.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_fiber.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_fiber.dir/flags.make

CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o: CMakeFiles/test_fiber.dir/flags.make
CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o: ../tests/test_fiber.cc
CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o: CMakeFiles/test_fiber.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/e/sylar_Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o -MF CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o.d -o CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o -c /mnt/e/sylar_Project/tests/test_fiber.cc

CMakeFiles/test_fiber.dir/tests/test_fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_fiber.dir/tests/test_fiber.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/e/sylar_Project/tests/test_fiber.cc > CMakeFiles/test_fiber.dir/tests/test_fiber.cc.i

CMakeFiles/test_fiber.dir/tests/test_fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_fiber.dir/tests/test_fiber.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/e/sylar_Project/tests/test_fiber.cc -o CMakeFiles/test_fiber.dir/tests/test_fiber.cc.s

# Object files for target test_fiber
test_fiber_OBJECTS = \
"CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o"

# External object files for target test_fiber
test_fiber_EXTERNAL_OBJECTS =

../bin/test_fiber: CMakeFiles/test_fiber.dir/tests/test_fiber.cc.o
../bin/test_fiber: CMakeFiles/test_fiber.dir/build.make
../bin/test_fiber: ../lib/libsylar.so
../bin/test_fiber: /usr/local/lib/libyaml-cpp.a
../bin/test_fiber: CMakeFiles/test_fiber.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/e/sylar_Project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/test_fiber"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_fiber.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_fiber.dir/build: ../bin/test_fiber
.PHONY : CMakeFiles/test_fiber.dir/build

CMakeFiles/test_fiber.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_fiber.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_fiber.dir/clean

CMakeFiles/test_fiber.dir/depend:
	cd /mnt/e/sylar_Project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/e/sylar_Project /mnt/e/sylar_Project /mnt/e/sylar_Project/build /mnt/e/sylar_Project/build /mnt/e/sylar_Project/build/CMakeFiles/test_fiber.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_fiber.dir/depend

