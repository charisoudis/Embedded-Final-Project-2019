# Install script for directory: /home/achariso/Desktop/Embedded Systems/Projects/Final/test/lib/cmocka

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xpkgconfigx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/cmocka.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/cmocka" TYPE FILE FILES
    "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/cmocka-config.cmake"
    "/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/cmocka-config-version.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/include/cmake_install.cmake")
  include("/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/src/cmake_install.cmake")
  include("/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/doc/cmake_install.cmake")
  include("/home/achariso/Desktop/Embedded Systems/Projects/Final/cmake-build-debug/test/lib/cmocka/example/cmake_install.cmake")

endif()

