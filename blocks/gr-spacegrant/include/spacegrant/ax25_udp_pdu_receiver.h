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


#ifndef INCLUDED_SPACEGRANT_AX25_UDP_PDU_RECEIVER_H
#define INCLUDED_SPACEGRANT_AX25_UDP_PDU_RECEIVER_H

#include <spacegrant/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace spacegrant {

    /*!
     * \brief <+description of block+>
     * \ingroup spacegrant
     *
     */
    class SPACEGRANT_API ax25_udp_pdu_receiver : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<ax25_udp_pdu_receiver> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of spacegrant::ax25_udp_pdu_receiver.
       *
       * To avoid accidental use of raw pointers, spacegrant::ax25_udp_pdu_receiver's
       * constructor is in a private implementation
       * class. spacegrant::ax25_udp_pdu_receiver::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string &host, int port, const std::string &sdest_addr);

      /*! \brief Change the connection to a new destination
       *
       * \param host         The name or IP address of the receiving host; use
       *                     NULL or None to break the connection without closing
       * \param port         Destination port to connect to on receiving host
       *
       * Calls disconnect() to terminate any current connection first.
       */
      virtual void connect(const std::string &host, int port) = 0;

      /*! \brief Send zero-length packet (if eof is requested) then stop sending
       *
       * Zero-byte packets can be interpreted as EOF by gr_udp_source.
       * Note that disconnect occurs automatically when the sink is
       * destroyed, but not when its top_block stops.*/
      virtual void disconnect() = 0;

    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_AX25_UDP_PDU_RECEIVER_H */

