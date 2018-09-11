/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_SPACEGRANT_SPIRIT1_BASIC_DEFRAMER_IMPL_H
#define INCLUDED_SPACEGRANT_SPIRIT1_BASIC_DEFRAMER_IMPL_H

#include <spacegrant/spirit1_basic_deframer.h>

namespace gr {
  namespace spacegrant {

    typedef enum {
      SYNC_PREAMBLE,
      LENGTH,
      ADDRESS,
      CONTROL,
      PAYLOAD,
      CRC
    } SPIRIT1_BASIC_DEFRAMER_STATE;

    class spirit1_basic_deframer_impl : public spirit1_basic_deframer
    {
     private:

      uint8_t d_length_len;

      uint8_t packet_data_buffer[300];

      uint8_t lfsr[9];

      uint8_t syncronizer[8];

      int bit_counter;
      SPIRIT1_BASIC_DEFRAMER_STATE state;

      uint16_t packet_length;
      uint8_t crc;
      uint8_t computed_crc;

     public:
      spirit1_basic_deframer_impl(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode);
      ~spirit1_basic_deframer_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_SPIRIT1_BASIC_DEFRAMER_IMPL_H */

