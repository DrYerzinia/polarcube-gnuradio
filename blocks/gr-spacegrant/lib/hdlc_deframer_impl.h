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

#ifndef INCLUDED_SPACEGRANT_HDLC_DEFRAMER_IMPL_H
#define INCLUDED_SPACEGRANT_HDLC_DEFRAMER_IMPL_H

#include <spacegrant/hdlc_deframer.h>

namespace gr {
  namespace spacegrant {

    class hdlc_deframer_impl : public hdlc_deframer
    {
     private:
        bool d_validate_crc;
        size_t d_length_min;
        size_t d_length_max;
        size_t d_ones;
        size_t d_bytectr;
        size_t d_bitctr;
        unsigned char *d_pktbuf;

        unsigned int crc_ccitt(unsigned char *data, size_t len);

     public:
      hdlc_deframer_impl(bool validate_crc, int length_min, int length_max);
      ~hdlc_deframer_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_HDLC_DEFRAMER_IMPL_H */

