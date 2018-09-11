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


#ifndef INCLUDED_SPACEGRANT_SPIRIT1_BASIC_FRAMER_H
#define INCLUDED_SPACEGRANT_SPIRIT1_BASIC_FRAMER_H

#include <spacegrant/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace spacegrant {

    /*!
     * \brief <+description of block+>
     * \ingroup spacegrant
     *
     */
    class SPACEGRANT_API spirit1_basic_framer : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<spirit1_basic_framer> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of spacegrant::spirit1_basic_framer.
       *
       * To avoid accidental use of raw pointers, spacegrant::spirit1_basic_framer's
       * constructor is in a private implementation
       * class. spacegrant::spirit1_basic_framer::make is the public interface for
       * creating new instances.
       */
      static sptr make(pmt::pmt_t preamble_bytes, pmt::pmt_t sync_bytes, int length_len, int address_len, int control_len, int crc_mode);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_SPIRIT1_BASIC_FRAMER_H */

