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

# Utility rule file for ExperimentalStart.

# Include the progress variables for this target.
include tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/progress.make

tests/lib/cmocka/CMakeFiles/ExperimentalStart:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka" && /home/achariso/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/183.5429.37/bin/cmake/linux/bin/ctest -D ExperimentalStart

ExperimentalStart: tests/lib/cmocka/CMakeFiles/ExperimentalStart
ExperimentalStart: tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/build.make

.PHONY : ExperimentalStart

# Rule to build all files generated by this target.
tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/build: ExperimentalStart

.PHONY : tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/build

tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/clean:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka" && $(CMAKE_COMMAND) -P CMakeFiles/ExperimentalStart.dir/cmake_clean.cmake
.PHONY : tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/clean

tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/depend:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/achariso/Desktop/Embedded Systems/Projects/Final" "/home/achariso/Desktop/Embedded Systems/Projects/Final/tests/lib/cmocka" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : tests/lib/cmocka/CMakeFiles/ExperimentalStart.dir/depend

