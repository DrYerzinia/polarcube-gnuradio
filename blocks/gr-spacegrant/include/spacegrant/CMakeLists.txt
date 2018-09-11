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
# Install public header files
########################################################################
install(FILES
    api.h
    udp_debug.h
    hdlc_framer.h
    ax25_udp_pdu_gen.h
    ax25_udp_pdu_receiver.h
    general_burster_2.h
    binary_sink.h
    NRZI.h
    DeNRZI.h
    invert_bit.h
    hdlc_deframer.h
    message_debug.h
    G3RUH_descramble.h
    ax25_pdu_packer.h
    ax25_pdu_unpacker.h
    spirit1_basic_deframer.h
    spirit1_basic_framer.h
    simple_clock_recovery_fb.h
    udp_pdu_receiver.h DESTINATION include/spacegrant
)
