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
#include "ax25_udp_pdu_receiver_impl.h"

#include <cstdio>

namespace gr {
  namespace spacegrant {

    ax25_udp_pdu_receiver::sptr
    ax25_udp_pdu_receiver::make(const std::string &host, int port, const std::string &dest_addr)
    {
      return gnuradio::get_initial_sptr
        (new ax25_udp_pdu_receiver_impl(host, port, dest_addr));
    }

    /*
     * The private constructor
     */
    ax25_udp_pdu_receiver_impl::ax25_udp_pdu_receiver_impl(const std::string &host, int port, const std::string &dest_addr)
      : gr::block("ax25_udp_pdu_receiver",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0)),
      d_connected(false), d_dest_addr(dest_addr)
    {

      message_port_register_in(pmt::mp("pdu"));
      set_msg_handler(pmt::mp("pdu"),
      boost::bind(&ax25_udp_pdu_receiver_impl::rx_pdu, this, _1));

      connect(host, port);
    }

    void
    ax25_udp_pdu_receiver_impl::rx_pdu(pmt::pmt_t pdu){

      pmt::pmt_t meta = pmt::car(pdu);
      pmt::pmt_t vector = pmt::cdr(pdu);
      size_t len = pmt::blob_length(vector);
      size_t offset(0);
	    const uint8_t* d = (const uint8_t*) pmt::uniform_vector_elements(vector, offset);

      // Get destination address
      char buf[8], buf2[20];
      int end_call = -1;

      bool match = true;

      for(int i = 0; i < 7; i++){
        char c = (signed char)((unsigned char)(d[i])>>1);
        if(c == 0x20 | i == 6) c = 0x0;
        buf[i] = c;
      }
      sprintf(buf2, "%s-%d", buf, ((d[6] >> 1) & 0x0F));
      if(d_dest_addr.compare(std::string(buf2)) != 0) match = false;
      //printf("%s, %s\n", d_dest_addr.c_str(), buf2);

      // Get source address
      for(int i = 7; i < 14; i++){
        char c = (signed char)((unsigned char)(d[i])>>1);
        if(c == 0x20 | i == 13) c = 0x0;
        buf[i-7] = c;
      }
      sprintf(buf2, "%s-%d", buf, ((d[13] >> 1) & 0x0F));

      //if(!match) printf("Packet not for us!");

      // Get sequence number
      // check end of address
      // check control
      // check PID

      if(d[16] == 0x01 && match){

        bool end_of_packet = d[17] & 0x80;
        size_t frag = d[17] & 0x07;
        size_t seq = (d[17] >> 3) & 0x0F;

        memcpy(pkt_buf+frag*250,d+18,len-18);

        if(end_of_packet){
          if(d_connected) {
            try {
                size_t r = d_socket->send_to(boost::asio::buffer((void*)pkt_buf, (frag*250)+len-18), d_endpoint);
            }
            catch(std::exception& e) {
              GR_LOG_ERROR(d_logger, boost::format("send error: %s") % e.what());
              return;
            }
          }
        }
      }

    }

    /*
     * Our virtual destructor.
     */
    ax25_udp_pdu_receiver_impl::~ax25_udp_pdu_receiver_impl()
    {
      if(d_connected)
        disconnect();
    }

    void
    ax25_udp_pdu_receiver_impl::connect(const std::string &host, int port)
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
    ax25_udp_pdu_receiver_impl::disconnect()
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

