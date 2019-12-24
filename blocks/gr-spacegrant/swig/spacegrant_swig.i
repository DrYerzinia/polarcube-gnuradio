/* -*- c++ -*- */

#define SPACEGRANT_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "spacegrant_swig_doc.i"

%{
#include "spacegrant/udp_debug.h"
#include "spacegrant/hdlc_framer.h"
#include "spacegrant/ax25_udp_pdu_gen.h"
#include "spacegrant/ax25_udp_pdu_receiver.h"
#include "spacegrant/general_burster_2.h"
#include "spacegrant/binary_sink.h"
#include "spacegrant/NRZI.h"
#include "spacegrant/DeNRZI.h"
#include "spacegrant/invert_bit.h"
#include "spacegrant/hdlc_deframer.h"
#include "spacegrant/message_debug.h"
#include "spacegrant/G3RUH_descramble.h"
#include "spacegrant/ax25_pdu_packer.h"
#include "spacegrant/ax25_pdu_unpacker.h"
#include "spacegrant/spirit1_basic_deframer.h"
#include "spacegrant/spirit1_basic_framer.h"
#include "spacegrant/simple_clock_recovery_fb.h"
#include "spacegrant/udp_pdu_receiver.h"
#include "spacegrant/sg_uhd_usrp_sink.h"
%}


%include "spacegrant/udp_debug.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, udp_debug);
%include "spacegrant/hdlc_framer.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, hdlc_framer);
%include "spacegrant/ax25_udp_pdu_gen.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, ax25_udp_pdu_gen);
%include "spacegrant/ax25_udp_pdu_receiver.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, ax25_udp_pdu_receiver);
%include "spacegrant/general_burster_2.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, general_burster_2);
%include "spacegrant/binary_sink.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, binary_sink);
%include "spacegrant/NRZI.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, NRZI);
%include "spacegrant/DeNRZI.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, DeNRZI);
%include "spacegrant/invert_bit.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, invert_bit);
%include "spacegrant/hdlc_deframer.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, hdlc_deframer);
%include "spacegrant/message_debug.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, message_debug);
%include "spacegrant/G3RUH_descramble.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, G3RUH_descramble);
%include "spacegrant/ax25_pdu_packer.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, ax25_pdu_packer);
%include "spacegrant/ax25_pdu_unpacker.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, ax25_pdu_unpacker);
%include "spacegrant/spirit1_basic_deframer.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, spirit1_basic_deframer);
%include "spacegrant/spirit1_basic_framer.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, spirit1_basic_framer);
%include "spacegrant/simple_clock_recovery_fb.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, simple_clock_recovery_fb);
%include "spacegrant/udp_pdu_receiver.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, udp_pdu_receiver);

%include "spacegrant/sg_uhd_usrp_sink.h"
GR_SWIG_BLOCK_MAGIC2(spacegrant, sg_uhd_usrp_sink);
