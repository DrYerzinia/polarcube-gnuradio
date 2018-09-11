# Install script for directory: /home/cassini/blocks/gr-spacegrant/include/spacegrant

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/spacegrant" TYPE FILE FILES
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/api.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/udp_debug.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/hdlc_framer.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/ax25_udp_pdu_gen.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/ax25_udp_pdu_receiver.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/general_burster_2.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/binary_sink.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/NRZI.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/DeNRZI.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/invert_bit.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/hdlc_deframer.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/message_debug.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/G3RUH_descramble.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/ax25_pdu_packer.h"
    "/home/cassini/blocks/gr-spacegrant/include/spacegrant/ax25_pdu_unpacker.h"
    )
endif()

