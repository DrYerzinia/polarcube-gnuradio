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

#ifndef INCLUDED_SPACEGRANT_SIMPLE_CLOCK_RECOVERY_FB_IMPL_H
#define INCLUDED_SPACEGRANT_SIMPLE_CLOCK_RECOVERY_FB_IMPL_H

#include <spacegrant/simple_clock_recovery_fb.h>

namespace gr {
  namespace spacegrant {

    class simple_clock_recovery_fb_impl : public simple_clock_recovery_fb
    {
     private:

      int sps;

      int last;

      int count;

      int correction_count;
      int correction_drop;
      int correction_length;

     public:
      simple_clock_recovery_fb_impl(int samples_per_symbol);
      ~simple_clock_recovery_fb_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
         gr_vector_int &ninput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_SIMPLE_CLOCK_RECOVERY_FB_IMPL_H */
