# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND spacegrant_sources
    udp_debug_impl.cc
    hdlc_framer_impl.cc
    ax25_udp_pdu_gen_impl.cc
    ax25_udp_pdu_receiver_impl.cc
    general_burster_2_impl.cc
    binary_sink_impl.cc
    NRZI_impl.cc
    DeNRZI_impl.cc
    invert_bit_impl.cc
    hdlc_deframer_impl.cc
    message_debug_impl.cc
    G3RUH_descramble_impl.cc
    ax25_pdu_packer_impl.cc
    ax25_pdu_unpacker_impl.cc
    spirit1_basic_deframer_impl.cc
    spirit1_basic_framer_impl.cc
    simple_clock_recovery_fb_impl.cc
    udp_pdu_receiver_impl.cc
    sg_uhd_usrp_sink_impl.cc
)

set(spacegrant_sources "${spacegrant_sources}" PARENT_SCOPE)
if(NOT spacegrant_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT spacegrant_sources)

add_library(gnuradio-spacegrant SHARED ${spacegrant_sources})
target_link_libraries(gnuradio-spacegrant ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-spacegrant PROPERTIES DEFINE_SYMBOL "gnuradio_spacegrant_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-spacegrant PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-spacegrant
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_spacegrant_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_spacegrant.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_spacegrant.cc
)

add_executable(test-spacegrant ${test_spacegrant_sources})

target_link_libraries(
  test-spacegrant
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-spacegrant
  uhd
)

GR_ADD_TEST(test_spacegrant test-spacegrant)
