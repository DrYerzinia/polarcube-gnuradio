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

#ifndef INCLUDED_SPACEGRANT_GENERAL_BURSTER_2_IMPL_H
#define INCLUDED_SPACEGRANT_GENERAL_BURSTER_2_IMPL_H

#include <spacegrant/general_burster_2.h>

namespace gr {
  namespace spacegrant {

    class general_burster_2_impl : public general_burster_2
    {
     private:

     void ring_buf_add(gr_complex item);
     gr_complex ring_buf_next();
     int ring_buf_len();

     public:
      general_burster_2_impl();
      ~general_burster_2_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_GENERAL_BURSTER_2_IMPL_H */

