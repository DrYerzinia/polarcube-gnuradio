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
#include "udp_debug_impl.h"

#include <stdio.h>

namespace gr {
  namespace spacegrant {

    udp_debug::sptr
    udp_debug::make()
    {
      return gnuradio::get_initial_sptr
        (new udp_debug_impl());
    }

    /*
     * The private constructor
     */
    udp_debug_impl::udp_debug_impl()
      : gr::sync_block("udp_debug",
              gr::io_signature::make(0, 1024, sizeof(char)),
              gr::io_signature::make(0, 0, 0))
    {}

    /*
     * Our virtual destructor.
     */
    udp_debug_impl::~udp_debug_impl()
    {
    }

    int
    udp_debug_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];

      // Do <+signal processing+>
      for(int i = 0; i < noutput_items; i++){
          printf("%02x", in[i]);
      }
      printf("\n");

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace spacegrant */
} /* namespace gr */

