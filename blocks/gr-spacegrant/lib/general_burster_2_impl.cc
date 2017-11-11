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
#include "general_burster_2_impl.h"

namespace gr {
  namespace spacegrant {

    general_burster_2::sptr
    general_burster_2::make()
    {
      return gnuradio::get_initial_sptr
        (new general_burster_2_impl());
    }

    /*
     * The private constructor
     */
    general_burster_2_impl::general_burster_2_impl()
      : gr::block("general_burster_2",
              gr::io_signature::make(0, 8192, sizeof(gr_complex)),
              gr::io_signature::make(0, 8192, sizeof(gr_complex)))
    {}

    /*
     * Our virtual destructor.
     */
    general_burster_2_impl::~general_burster_2_impl()
    {
    }

    void
    general_burster_2_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = 0;
    }

    int
    general_burster_2_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      for(int i = 0; i < noutput_items; i++){
        if(ninput_items[0] > i) out[i] = in[i];
        else out[i] = 0;
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      int consumed = ninput_items[0];
      if(noutput_items < ninput_items[0]) consumed = noutput_items;
      consume_each (consumed);

      return noutput_items;
    }

  } /* namespace spacegrant */
} /* namespace gr */

