# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build

# Include any dependencies generated for this target.
include CMakeFiles/quickstart.out.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/quickstart.out.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/quickstart.out.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/quickstart.out.dir/flags.make

CMakeFiles/quickstart.out.dir/codegen:
.PHONY : CMakeFiles/quickstart.out.dir/codegen

CMakeFiles/quickstart.out.dir/quickstart.c.o: CMakeFiles/quickstart.out.dir/flags.make
CMakeFiles/quickstart.out.dir/quickstart.c.o: /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/quickstart.c
CMakeFiles/quickstart.out.dir/quickstart.c.o: CMakeFiles/quickstart.out.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/quickstart.out.dir/quickstart.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/quickstart.out.dir/quickstart.c.o -MF CMakeFiles/quickstart.out.dir/quickstart.c.o.d -o CMakeFiles/quickstart.out.dir/quickstart.c.o -c /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/quickstart.c

CMakeFiles/quickstart.out.dir/quickstart.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/quickstart.out.dir/quickstart.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/quickstart.c > CMakeFiles/quickstart.out.dir/quickstart.c.i

CMakeFiles/quickstart.out.dir/quickstart.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/quickstart.out.dir/quickstart.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/quickstart.c -o CMakeFiles/quickstart.out.dir/quickstart.c.s

# Object files for target quickstart.out
quickstart_out_OBJECTS = \
"CMakeFiles/quickstart.out.dir/quickstart.c.o"

# External object files for target quickstart.out
quickstart_out_EXTERNAL_OBJECTS =

quickstart.out: CMakeFiles/quickstart.out.dir/quickstart.c.o
quickstart.out: CMakeFiles/quickstart.out.dir/build.make
quickstart.out: /opt/homebrew/lib/libmongoc-1.0.0.0.0.dylib
quickstart.out: /opt/homebrew/lib/libbson-1.0.0.0.0.dylib
quickstart.out: /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib/libsasl2.tbd
quickstart.out: CMakeFiles/quickstart.out.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable quickstart.out"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/quickstart.out.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/quickstart.out.dir/build: quickstart.out
.PHONY : CMakeFiles/quickstart.out.dir/build

CMakeFiles/quickstart.out.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/quickstart.out.dir/cmake_clean.cmake
.PHONY : CMakeFiles/quickstart.out.dir/clean

CMakeFiles/quickstart.out.dir/depend:
	cd /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build /Users/leozhang/Desktop/c-reg/c-reg/cli/c-quickstart/cmake-build/CMakeFiles/quickstart.out.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/quickstart.out.dir/depend

