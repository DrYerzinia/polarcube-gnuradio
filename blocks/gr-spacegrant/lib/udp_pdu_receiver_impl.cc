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
#include "udp_pdu_receiver_impl.h"

#include <cstdio>

namespace gr {
  namespace spacegrant {

    udp_pdu_receiver::sptr
    udp_pdu_receiver::make(const std::string &host, int port)
    {
      return gnuradio::get_initial_sptr
        (new udp_pdu_receiver_impl(host, port));
    }

    /*
     * The private constructor
     */
    udp_pdu_receiver_impl::udp_pdu_receiver_impl(const std::string &host, int port)
      : gr::block("udp_pdu_receiver",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0)),
      d_connected(false)
    {

      message_port_register_in(pmt::mp("pdu"));
      set_msg_handler(pmt::mp("pdu"),
      boost::bind(&udp_pdu_receiver_impl::rx_pdu, this, _1));

      connect(host, port);
    }

    void
    udp_pdu_receiver_impl::rx_pdu(pmt::pmt_t pdu){

      pmt::pmt_t meta = pmt::car(pdu);
      pmt::pmt_t vector = pmt::cdr(pdu);
      size_t len = pmt::blob_length(vector);
      size_t offset(0);
	    const uint8_t* d = (const uint8_t*) pmt::uniform_vector_elements(vector, offset);

          if(d_connected) {
            try {
                size_t r = d_socket->send_to(boost::asio::buffer((void*)d, len), d_endpoint);
            }
            catch(std::exception& e) {
              GR_LOG_ERROR(d_logger, boost::format("send error: %s") % e.what());
              return;
            }
          }

    }

    /*
     * Our virtual destructor.
     */
    udp_pdu_receiver_impl::~udp_pdu_receiver_impl()
    {
      if(d_connected)
        disconnect();
    }

    void
    udp_pdu_receiver_impl::connect(const std::string &host, int port)
    {
      if(d_connected)
        disconnect();

      std::string s_port = (boost::format("%d")%port).str();
      if(host.size() > 0) {
        boost::asio::ip::udp::resolver resolver(d_io_service);
        boost::asio::ip::udp::resolver::query query(host, s_port,
                                                    boost::asio::ip::resolver_query_base::passive);
        d_endpoint = *resolver.resolve(query);

        d_socket = new boost::asio::ip::udp::socket(d_io_service);
        d_socket->open(d_endpoint.protocol());

        boost::asio::socket_base::reuse_address roption(true);
        d_socket->set_option(roption);

        d_connected = true;
      }
    }

    void
    udp_pdu_receiver_impl::disconnect()
    {
      if(!d_connected)
        return;

      gr::thread::scoped_lock guard(d_mutex);  // protect d_socket from work()

      d_socket->close();
      delete d_socket;

      d_connected = false;
    }

  } /* namespace spacegrant */
} /* namespace gr */
