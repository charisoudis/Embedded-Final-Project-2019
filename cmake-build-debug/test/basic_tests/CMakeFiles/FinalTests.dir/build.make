# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /home/achariso/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/183.5429.37/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/achariso/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/183.5429.37/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/achariso/Desktop/Embedded Systems/Projects/Final"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug"

# Include any dependencies generated for this target.
include test/basic_tests/CMakeFiles/FinalTests.dir/depend.make

# Include the progress variables for this target.
include test/basic_tests/CMakeFiles/FinalTests.dir/progress.make

# Include the compile flags for this target's objects.
include test/basic_tests/CMakeFiles/FinalTests.dir/flags.make

test/basic_tests/CMakeFiles/FinalTests.dir/test_utils.c.o: test/basic_tests/CMakeFiles/FinalTests.dir/flags.make
test/basic_tests/CMakeFiles/FinalTests.dir/test_utils.c.o: ../test/basic_tests/test_utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/basic_tests/CMakeFiles/FinalTests.dir/test_utils.c.o"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalTests.dir/test_utils.c.o   -c "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests/test_utils.c"

test/basic_tests/CMakeFiles/FinalTests.dir/test_utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalTests.dir/test_utils.c.i"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests/test_utils.c" > CMakeFiles/FinalTests.dir/test_utils.c.i

test/basic_tests/CMakeFiles/FinalTests.dir/test_utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalTests.dir/test_utils.c.s"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests/test_utils.c" -o CMakeFiles/FinalTests.dir/test_utils.c.s

test/basic_tests/CMakeFiles/FinalTests.dir/run.c.o: test/basic_tests/CMakeFiles/FinalTests.dir/flags.make
test/basic_tests/CMakeFiles/FinalTests.dir/run.c.o: ../test/basic_tests/run.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object test/basic_tests/CMakeFiles/FinalTests.dir/run.c.o"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/FinalTests.dir/run.c.o   -c "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests/run.c"

test/basic_tests/CMakeFiles/FinalTests.dir/run.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/FinalTests.dir/run.c.i"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests/run.c" > CMakeFiles/FinalTests.dir/run.c.i

test/basic_tests/CMakeFiles/FinalTests.dir/run.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/FinalTests.dir/run.c.s"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests/run.c" -o CMakeFiles/FinalTests.dir/run.c.s

# Object files for target FinalTests
FinalTests_OBJECTS = \
"CMakeFiles/FinalTests.dir/test_utils.c.o" \
"CMakeFiles/FinalTests.dir/run.c.o"

# External object files for target FinalTests
FinalTests_EXTERNAL_OBJECTS =

test/basic_tests/FinalTests: test/basic_tests/CMakeFiles/FinalTests.dir/test_utils.c.o
test/basic_tests/FinalTests: test/basic_tests/CMakeFiles/FinalTests.dir/run.c.o
test/basic_tests/FinalTests: test/basic_tests/CMakeFiles/FinalTests.dir/build.make
test/basic_tests/FinalTests: test/basic_tests/CMakeFiles/FinalTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable FinalTests"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FinalTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/basic_tests/CMakeFiles/FinalTests.dir/build: test/basic_tests/FinalTests

.PHONY : test/basic_tests/CMakeFiles/FinalTests.dir/build

test/basic_tests/CMakeFiles/FinalTests.dir/clean:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" && $(CMAKE_COMMAND) -P CMakeFiles/FinalTests.dir/cmake_clean.cmake
.PHONY : test/basic_tests/CMakeFiles/FinalTests.dir/clean

test/basic_tests/CMakeFiles/FinalTests.dir/depend:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/achariso/Desktop/Embedded Systems/Projects/Final" "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/basic_tests" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/basic_tests/CMakeFiles/FinalTests.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : test/basic_tests/CMakeFiles/FinalTests.dir/depend

