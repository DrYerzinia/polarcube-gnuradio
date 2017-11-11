# Install script for directory: /home/cassini/blocks/gr-spacegrant/grc

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_udp_debug.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_hdlc_framer.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_general_burster.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_ax25_udp_pdu_gen.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_ax25_udp_pdu_receiver.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_general_burster_2.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_binary_sink.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_NRZI.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_DeNRZI.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_invert_bit.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_hdlc_deframer.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_message_debug.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_G3RUH_descramble.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_sat_doppler_correction.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_ax25_pdu_packer.xml"
    "/home/cassini/blocks/gr-spacegrant/grc/spacegrant_ax25_pdu_unpacker.xml"
    )
endif()

