# Copyright 2011 Free Software Foundation, Inc.
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

install(FILES
    spacegrant_udp_debug.xml
    spacegrant_hdlc_framer.xml
    spacegrant_general_burster.xml
    spacegrant_ax25_udp_pdu_gen.xml
    spacegrant_ax25_udp_pdu_receiver.xml
    spacegrant_general_burster_2.xml
    spacegrant_binary_sink.xml
    spacegrant_NRZI.xml
    spacegrant_DeNRZI.xml
    spacegrant_invert_bit.xml
    spacegrant_hdlc_deframer.xml
    spacegrant_message_debug.xml
    spacegrant_G3RUH_descramble.xml
    spacegrant_sat_doppler_correction.xml
    spacegrant_ax25_pdu_packer.xml
    spacegrant_ax25_pdu_unpacker.xml
    spacegrant_spirit1_basic_deframer.xml
    spacegrant_spirit1_basic_framer.xml
    spacegrant_simple_clock_recovery_fb.xml
    spacegrant_udp_pdu_receiver.xml DESTINATION share/gnuradio/grc/blocks
)
