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


#ifndef INCLUDED_SPACEGRANT_AX25_PDU_UNPACKER_H
#define INCLUDED_SPACEGRANT_AX25_PDU_UNPACKER_H

#include <spacegrant/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace spacegrant {

    /*!
     * \brief <+description of block+>
     * \ingroup spacegrant
     *
     */
    class SPACEGRANT_API ax25_pdu_unpacker : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<ax25_pdu_unpacker> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of spacegrant::ax25_pdu_unpacker.
       *
       * To avoid accidental use of raw pointers, spacegrant::ax25_pdu_unpacker's
       * constructor is in a private implementation
       * class. spacegrant::ax25_pdu_unpacker::make is the public interface for
       * creating new instances.
       */
      static sptr make(int addr_filt, const std::string &src_addr, const std::string &dest_addr);
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_AX25_PDU_UNPACKER_H */
