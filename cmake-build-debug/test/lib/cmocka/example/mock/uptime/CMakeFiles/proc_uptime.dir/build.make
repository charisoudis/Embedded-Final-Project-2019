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
include test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/depend.make

# Include the progress variables for this target.
include test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/progress.make

# Include the compile flags for this target's objects.
include test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/flags.make

test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/proc_uptime.c.o: test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/flags.make
test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/proc_uptime.c.o: ../test/lib/cmocka/example/mock/uptime/proc_uptime.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/proc_uptime.c.o"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/proc_uptime.dir/proc_uptime.c.o   -c "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/lib/cmocka/example/mock/uptime/proc_uptime.c"

test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/proc_uptime.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/proc_uptime.dir/proc_uptime.c.i"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/lib/cmocka/example/mock/uptime/proc_uptime.c" > CMakeFiles/proc_uptime.dir/proc_uptime.c.i

test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/proc_uptime.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/proc_uptime.dir/proc_uptime.c.s"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/lib/cmocka/example/mock/uptime/proc_uptime.c" -o CMakeFiles/proc_uptime.dir/proc_uptime.c.s

# Object files for target proc_uptime
proc_uptime_OBJECTS = \
"CMakeFiles/proc_uptime.dir/proc_uptime.c.o"

# External object files for target proc_uptime
proc_uptime_EXTERNAL_OBJECTS =

test/lib/cmocka/example/mock/uptime/libproc_uptime.a: test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/proc_uptime.c.o
test/lib/cmocka/example/mock/uptime/libproc_uptime.a: test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/build.make
test/lib/cmocka/example/mock/uptime/libproc_uptime.a: test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libproc_uptime.a"
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" && $(CMAKE_COMMAND) -P CMakeFiles/proc_uptime.dir/cmake_clean_target.cmake
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/proc_uptime.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/build: test/lib/cmocka/example/mock/uptime/libproc_uptime.a

.PHONY : test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/build

test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/clean:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" && $(CMAKE_COMMAND) -P CMakeFiles/proc_uptime.dir/cmake_clean.cmake
.PHONY : test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/clean

test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/depend:
	cd "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/achariso/Desktop/Embedded Systems/Projects/Final" "/home/achariso/Desktop/Embedded Systems/Projects/Final/test/lib/cmocka/example/mock/uptime" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime" "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : test/lib/cmocka/example/mock/uptime/CMakeFiles/proc_uptime.dir/depend

