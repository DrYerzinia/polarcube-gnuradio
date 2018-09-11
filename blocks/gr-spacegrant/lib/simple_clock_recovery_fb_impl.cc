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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "simple_clock_recovery_fb_impl.h"

#include <cmath>

namespace gr {
  namespace spacegrant {

    simple_clock_recovery_fb::sptr
    simple_clock_recovery_fb::make(int samples_per_symbol)
    {
      return gnuradio::get_initial_sptr
        (new simple_clock_recovery_fb_impl(samples_per_symbol));
    }

    /*
     * The private constructor
     */
    simple_clock_recovery_fb_impl::simple_clock_recovery_fb_impl(int samples_per_symbol)
      : gr::block("simple_clock_recovery_fb",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {

      sps = samples_per_symbol;

      last = 0;

      correction_count = 0;
      correction_drop = 1;
      correction_length = 30;

    }

    /*
     * Our virtual destructor.
     */
    simple_clock_recovery_fb_impl::~simple_clock_recovery_fb_impl()
    {
    }

    void simple_clock_recovery_fb_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required){

    	ninput_items_required[0] = 128 * noutput_items;

    }

    int
    simple_clock_recovery_fb_impl::general_work(int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      int n_input_items = ninput_items[0];
      const float *in = (const float *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      //printf("\n n_in %d \n", n_input_items);

      int n_in = 0;
      int n_out = 0;
      while (n_in < n_input_items) {

        n_in++;
        float data = *in++;

        // Known baud rate error compenstation
        /*
        // Works but trying internal percentage based correction in gnuradio
        if(correction_count == correction_length){
          correction_count = 0;
          continue;
        }
        correction_count++;
        */

        int val = 0;
        if(data > 0) val = 1;

        if(val != last){
          int bits = round(count/((float)sps));
          n_out += bits;
          //printf("\n bits %d count %d \n", bits, count);
          for(int i = 0; i < bits; i++){
            *out++ = last;
          }
          count = 0;
        } else {
          count++;
        }

        last = val;

    }

    this->consume(0, n_in);

    // Tell runtime system how many output items we produced.
    return n_out;
  }

  } /* namespace spacegrant */
} /* namespace gr */
