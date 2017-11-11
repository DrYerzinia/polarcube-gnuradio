/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ax25_pdu_unpacker_impl.h"

namespace gr {
  namespace spacegrant {

    ax25_pdu_unpacker::sptr
    ax25_pdu_unpacker::make(int addr_filt, const std::string &src_addr, const std::string &dest_addr)
    {
      return gnuradio::get_initial_sptr
        (new ax25_pdu_unpacker_impl(addr_filt, src_addr, dest_addr));
    }

    /*
     * The private constructor
     */
    ax25_pdu_unpacker_impl::ax25_pdu_unpacker_impl(int addr_filt, const std::string &src_addr, const std::string &dest_addr)
      : gr::block("ax25_pdu_unpacker",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_addr_filt(addr_filt), d_src_addr(src_addr), d_dest_addr(dest_addr)
    {
      message_port_register_out(pmt::mp("pdu_out"));
      message_port_register_in(pmt::mp("pdu_in"));
      set_msg_handler(pmt::mp("pdu_in"), boost::bind(&ax25_pdu_unpacker_impl::pdu_in, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    ax25_pdu_unpacker_impl::~ax25_pdu_unpacker_impl()
    {
    }

    void
    ax25_pdu_unpacker_impl::pdu_in(pmt::pmt_t pdu){

      pmt::pmt_t meta = pmt::car(pdu);
      pmt::pmt_t vector = pmt::cdr(pdu);
      size_t len = pmt::blob_length(vector);
      size_t offset(0);
	    const uint8_t* d = (const uint8_t*) pmt::uniform_vector_elements(vector, offset);

      // Get destination address
      char buf[8], buf2[20];
      int end_call = -1;

      for(int i = 0; i < 7; i++){
        char c = (signed char)((unsigned char)(d[i])>>1);
        if(c == 0x20 | i == 6) c = 0x0;
        buf[i] = c;
      }
      if(d_addr_filt == ADDR_FILT_DEST || d_addr_filt == ADDR_FILT_BOTH){
        sprintf(buf2, "%s-%d", buf, ((d[6] >> 1) & 0x0F));
        if(d_dest_addr.compare(std::string(buf2)) != 0) return;
      }

      // Get source address
      for(int i = 7; i < 14; i++){
        char c = (signed char)((unsigned char)(d[i])>>1);
        if(c == 0x20 | i == 13) c = 0x0;
        buf[i-7] = c;
      }
      if(d_addr_filt == ADDR_FILT_SRC || d_addr_filt == ADDR_FILT_BOTH){
        sprintf(buf2, "%s-%d", buf, ((d[13] >> 1) & 0x0F));
        if(d_src_addr.compare(std::string(buf2)) != 0) return;
      }

      // check end of address
      // check control
      // check PID

      ///////////////////////////////////////////////////////////////

      uint8_t packet[256];
      memcpy(packet, d+16, len);
      message_port_pub(pmt::mp("pdu_out"), pmt::cons(pmt::PMT_NIL, pmt::init_u8vector(len-16, (const uint8_t *) &packet)));

    }

  } /* namespace spacegrant */
} /* namespace gr */
