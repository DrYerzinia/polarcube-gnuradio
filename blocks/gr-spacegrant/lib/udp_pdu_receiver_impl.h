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

#ifndef INCLUDED_SPACEGRANT_UDP_PDU_RECEIVER_IMPL_H
#define INCLUDED_SPACEGRANT_UDP_PDU_RECEIVER_IMPL_H

#include <spacegrant/udp_pdu_receiver.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <gnuradio/thread/thread.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

namespace gr {
  namespace spacegrant {

    class udp_pdu_receiver_impl : public udp_pdu_receiver
    {
     private:
       bool   d_connected;       // are we connected?
       gr::thread::mutex  d_mutex;    // protects d_socket and d_connected

       boost::asio::ip::udp::socket *d_socket;          // handle to socket
       boost::asio::ip::udp::endpoint d_endpoint;
       boost::asio::io_service d_io_service;

       void rx_pdu(pmt::pmt_t pdu);

      public:
       udp_pdu_receiver_impl(const std::string &host, int port);
       ~udp_pdu_receiver_impl();

       void connect(const std::string &host, int port);
       void disconnect();

    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_UDP_PDU_RECEIVER_IMPL_H */
