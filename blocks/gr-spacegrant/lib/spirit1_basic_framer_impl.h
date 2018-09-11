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

#ifndef INCLUDED_SPACEGRANT_SPIRIT1_BASIC_FRAMER_IMPL_H
#define INCLUDED_SPACEGRANT_SPIRIT1_BASIC_FRAMER_IMPL_H

#include <spacegrant/spirit1_basic_framer.h>

#define CRC8_TABLE_SIZE 256

namespace gr {
  namespace spacegrant {

    class spirit1_basic_framer_impl : public spirit1_basic_framer
    {
     private:

        uint8_t crc_table[CRC8_TABLE_SIZE];

        void crc8_populate_msb(uint8_t table[CRC8_TABLE_SIZE], uint8_t polynomial);
        uint8_t crc8(const uint8_t table[CRC8_TABLE_SIZE], uint8_t *pdata, size_t nbytes, uint8_t crc);

        std::vector<std::vector<unsigned char> > d_leftovers;
        //pmt::pmt_t d_frame_tag, d_me;
        std::vector<unsigned char> unpack(std::vector<unsigned char> &pkt);

     public:
      spirit1_basic_framer_impl(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode);
      ~spirit1_basic_framer_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_SPIRIT1_BASIC_FRAMER_IMPL_H */

