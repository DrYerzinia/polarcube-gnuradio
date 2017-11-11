#ifndef INCLUDED_SPACEGRANT_AX25_UDP_PDU_GEN_IMPL_H
#define INCLUDED_SPACEGRANT_AX25_UDP_PDU_GEN_IMPL_H

#include <spacegrant/ax25_udp_pdu_gen.h>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace spacegrant {

    class ax25_udp_pdu_gen_impl : public ax25_udp_pdu_gen
    {
     private:
      int     d_payload_size; // maximum transmission unit (packet length)
      bool    d_connected;    // are we connected?
      char   *d_rxbuf;        // get UDP buffer items
      char   *d_residbuf;     // hold buffer between calls
      ssize_t d_residual;     // hold information about number of bytes stored in residbuf
      ssize_t d_sent;         // track how much of d_residbuf we've outputted
      size_t  d_offset;       // point to residbuf location offset

      static const int BUF_SIZE_PAYLOADS; //!< The d_residbuf size in multiples of d_payload_size

      std::string d_host;
      unsigned short d_port;
      uint8_t d_packet_seq;

      boost::asio::ip::udp::socket *d_socket;
      boost::asio::ip::udp::endpoint d_endpoint;
      boost::asio::ip::udp::endpoint d_endpoint_rcvd;
      boost::asio::io_service d_io_service;

      std::string d_src_addr;
      std::string d_dest_addr;

      gr::thread::condition_variable d_cond_wait;
      gr::thread::mutex d_udp_mutex;
      gr::thread::thread d_udp_thread;

      void address_to_data(uint8_t * data, std::string address, uint8_t ssid_top);

      void start_receive();
      void handle_read(const boost::system::error_code& error,
                       size_t bytes_transferred);
      void run_io_service() { d_io_service.run(); }

     public:
      ax25_udp_pdu_gen_impl(const std::string &host, int port, const std::string &src_addr, const std::string &dest_addr);
      ~ax25_udp_pdu_gen_impl();

      void connect(const std::string &host, int port);
      void disconnect();

      int payload_size() { return d_payload_size; }
      int get_port();
    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_AX25_UDP_PDU_GEN_IMPL_H */

