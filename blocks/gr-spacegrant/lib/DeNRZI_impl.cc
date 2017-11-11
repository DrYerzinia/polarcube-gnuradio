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
#include "DeNRZI_impl.h"

namespace gr {
  namespace spacegrant {

    DeNRZI::sptr
    DeNRZI::make()
    {
      return gnuradio::get_initial_sptr
        (new DeNRZI_impl());
    }

    /*
     * The private constructor
     */
    DeNRZI_impl::DeNRZI_impl()
      : gr::sync_block("DeNRZI",
              gr::io_signature::make(1, 1024, sizeof(char)),
              gr::io_signature::make(1, 1024, sizeof(char))),
      last(0)
    {}

    /*
     * Our virtual destructor.
     */
    DeNRZI_impl::~DeNRZI_impl()
    {
    }

    int
    DeNRZI_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char *) output_items[0];

      for(int i = 0; i < noutput_items; i++){
        // If change its a zero, else its a 1
        if(in[i] == last){
          out[i] = 1;
        } else {
          out[i] = 0;
        }
        last = in[i];

      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace spacegrant */
} /* namespace gr */

