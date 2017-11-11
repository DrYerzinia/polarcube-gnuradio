# Install script for directory: /home/cassini/blocks/gr-spacegrant/python

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/spacegrant" TYPE FILE FILES
    "/home/cassini/blocks/gr-spacegrant/python/__init__.py"
    "/home/cassini/blocks/gr-spacegrant/python/general_burster.py"
    "/home/cassini/blocks/gr-spacegrant/python/sat_doppler_correction.py"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/spacegrant" TYPE FILE FILES
    "/home/cassini/blocks/gr-spacegrant/cmake/python/__init__.pyc"
    "/home/cassini/blocks/gr-spacegrant/cmake/python/general_burster.pyc"
    "/home/cassini/blocks/gr-spacegrant/cmake/python/sat_doppler_correction.pyc"
    "/home/cassini/blocks/gr-spacegrant/cmake/python/__init__.pyo"
    "/home/cassini/blocks/gr-spacegrant/cmake/python/general_burster.pyo"
    "/home/cassini/blocks/gr-spacegrant/cmake/python/sat_doppler_correction.pyo"
    )
endif()

