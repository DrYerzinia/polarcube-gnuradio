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

#ifndef INCLUDED_SPACEGRANT_MESSAGE_DEBUG_IMPL_H
#define INCLUDED_SPACEGRANT_MESSAGE_DEBUG_IMPL_H

#include <spacegrant/message_debug.h>

namespace gr {
  namespace spacegrant {

    class message_debug_impl : public message_debug
    {
     private:
      void print_pdu(pmt::pmt_t pdu);

     public:
      message_debug_impl();
      ~message_debug_impl();

    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_MESSAGE_DEBUG_IMPL_H */

