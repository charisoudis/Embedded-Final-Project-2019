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
include tests/lib/cmocka/example/CMakeFiles/simple_test.dir/depend.make

# Include the progress variables for this target.
include tests/lib/cmocka/example/CMakeFiles/simple_test.dir/progress.make

# Include the compile flags for this target's objects.
include tests/lib/cmocka/example/CMakeFiles/simple_test.dir/flags.make

tests/lib/cmocka/example/CMakeFiles/simple_test.dir/simple_test.c.o: tests/lib/cmocka/example/CMakeFiles/simple_test.dir/flags.make
tests/lib/cmocka/example/CMakeFiles/simple_test.dir/simple_test.c.o: ../tests/lib/cmocka/example/simple_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object tests/lib/cmocka/example/CMakeFiles/simple_test.dir/simple_test.c.o"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/simple_test.dir/simple_test.c.o   -c "/home/achariso/Desktop/Embedded Systems/Projects/Final/tests/lib/cmocka/example/simple_test.c"

tests/lib/cmocka/example/CMakeFiles/simple_test.dir/simple_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple_test.dir/simple_test.c.i"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/achariso/Desktop/Embedded Systems/Projects/Final/tests/lib/cmocka/example/simple_test.c" > CMakeFiles/simple_test.dir/simple_test.c.i

tests/lib/cmocka/example/CMakeFiles/simple_test.dir/simple_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple_test.dir/simple_test.c.s"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/achariso/Desktop/Embedded Systems/Projects/Final/tests/lib/cmocka/example/simple_test.c" -o CMakeFiles/simple_test.dir/simple_test.c.s

# Object files for target simple_test
simple_test_OBJECTS = \
"CMakeFiles/simple_test.dir/simple_test.c.o"

# External object files for target simple_test
simple_test_EXTERNAL_OBJECTS =

tests/lib/cmocka/example/simple_test: tests/lib/cmocka/example/CMakeFiles/simple_test.dir/simple_test.c.o
tests/lib/cmocka/example/simple_test: tests/lib/cmocka/example/CMakeFiles/simple_test.dir/build.make
tests/lib/cmocka/example/simple_test: tests/lib/cmocka/src/libcmocka.so.0.7.0
tests/lib/cmocka/example/simple_test: tests/lib/cmocka/example/CMakeFiles/simple_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable simple_test"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/lib/cmocka/example/CMakeFiles/simple_test.dir/build: tests/lib/cmocka/example/simple_test

.PHONY : tests/lib/cmocka/example/CMakeFiles/simple_test.dir/build

tests/lib/cmocka/example/CMakeFiles/simple_test.dir/clean:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example" && $(CMAKE_COMMAND) -P CMakeFiles/simple_test.dir/cmake_clean.cmake
.PHONY : tests/lib/cmocka/example/CMakeFiles/simple_test.dir/clean

tests/lib/cmocka/example/CMakeFiles/simple_test.dir/depend:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/achariso/Desktop/Embedded Systems/Projects/Final" "/home/achariso/Desktop/Embedded Systems/Projects/Final/tests/lib/cmocka/example" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/tests/lib/cmocka/example/CMakeFiles/simple_test.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : tests/lib/cmocka/example/CMakeFiles/simple_test.dir/depend

