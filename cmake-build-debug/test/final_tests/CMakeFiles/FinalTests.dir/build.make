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
include test/final_tests/CMakeFiles/FinalTests.dir/depend.make

# Include the progress variables for this target.
include test/final_tests/CMakeFiles/FinalTests.dir/progress.make

# Include the compile flags for this target's objects.
include test/final_tests/CMakeFiles/FinalTests.dir/flags.make

test/final_tests/CMakeFiles/FinalTests.dir/UtilsTest.cpp.o: test/final_tests/CMakeFiles/FinalTests.dir/flags.make
test/final_tests/CMakeFiles/FinalTests.dir/UtilsTest.cpp.o: ../test/final_tests/UtilsTest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/final_tests/CMakeFiles/FinalTests.dir/UtilsTest.cpp.o"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests" && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/FinalTests.dir/UtilsTest.cpp.o -c "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/final_tests/UtilsTest.cpp"

test/final_tests/CMakeFiles/FinalTests.dir/UtilsTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FinalTests.dir/UtilsTest.cpp.i"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests" && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/final_tests/UtilsTest.cpp" > CMakeFiles/FinalTests.dir/UtilsTest.cpp.i

test/final_tests/CMakeFiles/FinalTests.dir/UtilsTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FinalTests.dir/UtilsTest.cpp.s"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests" && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/final_tests/UtilsTest.cpp" -o CMakeFiles/FinalTests.dir/UtilsTest.cpp.s

# Object files for target FinalTests
FinalTests_OBJECTS = \
"CMakeFiles/FinalTests.dir/UtilsTest.cpp.o"

# External object files for target FinalTests
FinalTests_EXTERNAL_OBJECTS =

test/final_tests/FinalTests: test/final_tests/CMakeFiles/FinalTests.dir/UtilsTest.cpp.o
test/final_tests/FinalTests: test/final_tests/CMakeFiles/FinalTests.dir/build.make
test/final_tests/FinalTests: lib/libgtestd.a
test/final_tests/FinalTests: lib/libgtest_maind.a
test/final_tests/FinalTests: src/libFINAL_LIB.a
test/final_tests/FinalTests: lib/libgtestd.a
test/final_tests/FinalTests: test/final_tests/CMakeFiles/FinalTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable FinalTests"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FinalTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/final_tests/CMakeFiles/FinalTests.dir/build: test/final_tests/FinalTests

.PHONY : test/final_tests/CMakeFiles/FinalTests.dir/build

test/final_tests/CMakeFiles/FinalTests.dir/clean:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests" && $(CMAKE_COMMAND) -P CMakeFiles/FinalTests.dir/cmake_clean.cmake
.PHONY : test/final_tests/CMakeFiles/FinalTests.dir/clean

test/final_tests/CMakeFiles/FinalTests.dir/depend:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/achariso/Desktop/Embedded Systems/Projects/Final" "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/final_tests" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/final_tests/CMakeFiles/FinalTests.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : test/final_tests/CMakeFiles/FinalTests.dir/depend

