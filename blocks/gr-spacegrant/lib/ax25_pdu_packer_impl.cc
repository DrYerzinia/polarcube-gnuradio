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
#include "ax25_pdu_packer_impl.h"

namespace gr {
  namespace spacegrant {

    ax25_pdu_packer::sptr
    ax25_pdu_packer::make(const std::string &src_addr, const std::string &dest_addr)
    {
      return gnuradio::get_initial_sptr
        (new ax25_pdu_packer_impl(src_addr, dest_addr));
    }

    /*
     * The private constructor
     */
    ax25_pdu_packer_impl::ax25_pdu_packer_impl(const std::string &src_addr, const std::string &dest_addr)
      : gr::block("ax25_pdu_packer",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_src_addr(src_addr), d_dest_addr(dest_addr)
    {
      message_port_register_out(pmt::mp("pdu_out"));
      message_port_register_in(pmt::mp("pdu_in"));
      set_msg_handler(pmt::mp("pdu_in"), boost::bind(&ax25_pdu_packer_impl::pdu_in, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    ax25_pdu_packer_impl::~ax25_pdu_packer_impl()
    {
    }

    /**
     * Add a address to the the data array of raw packet data
     * @method push_address_to_data
     * @static
     * @param {Array} data Raw data array
     * @param {String} address Address to add
     * @param {int} ssid SSID of the address
     * @param {int} ssid_top Top 3 bits of SSID Field to use depending on type of address
    */
    void
    ax25_pdu_packer_impl::address_to_data(uint8_t * data, std::string address, uint8_t ssid_top){

        int ssid_loc = -1;
        for(int i = 0; i < 6; i++){

            if(address[i] == '-') ssid_loc = i+1;

            if(ssid_loc == -1 && i < address.length()) data[i] = address[i] << 1;
            else data[i] = 0x40; // 0x20 (space) << 1
        }

        if(ssid_loc == -1) ssid_loc = 7;
        uint8_t ssid = std::atoi(address.substr(ssid_loc).c_str());

        data[6] = ((ssid << 1) | ssid_top);
    }

    void
    ax25_pdu_packer_impl::pdu_in(pmt::pmt_t pdu){

      pmt::pmt_t meta = pmt::car(pdu);
      pmt::pmt_t vector = pmt::cdr(pdu);
      size_t data_bytes = pmt::blob_length(vector);
      size_t offset(0);
	    const uint8_t* data = (const uint8_t*) pmt::uniform_vector_elements(vector, offset);

      ///////////////////////////////////////////////////////////////

      uint8_t packet[256+16];

      // Add addressing
      address_to_data(packet,   d_dest_addr, 0xE0);
      address_to_data(packet+7, d_src_addr, 0x60);
      packet[13] |= 1; // End of addresses

      // Add Control and PID
      packet[14] = 0x03; // Unnumbered Information Frame
      packet[15] = 0xF0; // No Layer 3

      memcpy(packet+16, data, data_bytes);
      message_port_pub(pmt::mp("pdu_out"), pmt::cons(pmt::PMT_NIL, pmt::init_u8vector(data_bytes+16, (const uint8_t *) &packet)));

    }

  } /* namespace spacegrant */
} /* namespace gr */
