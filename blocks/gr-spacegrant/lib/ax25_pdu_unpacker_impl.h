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

#ifndef INCLUDED_SPACEGRANT_AX25_PDU_UNPACKER_IMPL_H
#define INCLUDED_SPACEGRANT_AX25_PDU_UNPACKER_IMPL_H

#include <spacegrant/ax25_pdu_unpacker.h>

#define ADDR_FILT_NONE 0
#define ADDR_FILT_SRC  1
#define ADDR_FILT_DEST 2
#define ADDR_FILT_BOTH 3

namespace gr {
  namespace spacegrant {

    class ax25_pdu_unpacker_impl : public ax25_pdu_unpacker
    {
     private:
      int d_addr_filt;
      std::string d_src_addr;
      std::string d_dest_addr;

      void pdu_in(pmt::pmt_t pdu);

     public:
      ax25_pdu_unpacker_impl(int addr_filt, const std::string &src_addr, const std::string &dest_addr);
      ~ax25_pdu_unpacker_impl();

    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_AX25_PDU_UNPACKER_IMPL_H */
