#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ax25_udp_pdu_gen_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <pmt/pmt.h>
#include <string>

namespace gr {
  namespace spacegrant {

    const int  ax25_udp_pdu_gen_impl::BUF_SIZE_PAYLOADS =
      gr::prefs::singleton()->get_long("udp_blocks", "buf_size_payloads", 50);

    ax25_udp_pdu_gen::sptr
    ax25_udp_pdu_gen::make(const std::string &host, int port, const std::string &src_addr, const std::string &dest_addr)
    {
      return gnuradio::get_initial_sptr
        (new ax25_udp_pdu_gen_impl(host, port, src_addr, dest_addr));
    }

    ax25_udp_pdu_gen_impl::ax25_udp_pdu_gen_impl(const std::string &host, int port, const std::string &src_addr, const std::string &dest_addr)
      : gr::block("ax25_udp_pdu_gen",
              io_signature::make(0, 0, 0),
              io_signature::make(0, 0, 0)),
        d_connected(false), d_residual(0), d_sent(0), d_offset(0), d_packet_seq(0), d_src_addr(src_addr), d_dest_addr(dest_addr)
    {

      d_payload_size = 2048;

      message_port_register_out(pmt::mp("pdu"));

      d_rxbuf = new char[4*d_payload_size];
      d_residbuf = new char[BUF_SIZE_PAYLOADS*d_payload_size];

      connect(host, port);
    }

    ax25_udp_pdu_gen_impl::~ax25_udp_pdu_gen_impl()
    {
      if(d_connected)
        disconnect();

      delete [] d_rxbuf;
      delete [] d_residbuf;
    }

    void
    ax25_udp_pdu_gen_impl::connect(const std::string &host, int port)
    {
      if(d_connected)
        disconnect();

      d_host = host;
      d_port = static_cast<unsigned short>(port);

      std::string s_port;
      s_port = (boost::format("%d")%d_port).str();

      if(host.size() > 0) {
        boost::asio::ip::udp::resolver resolver(d_io_service);
        boost::asio::ip::udp::resolver::query query(d_host, s_port,
                                                    boost::asio::ip::resolver_query_base::passive);
        d_endpoint = *resolver.resolve(query);

        d_socket = new boost::asio::ip::udp::socket(d_io_service);
        d_socket->open(d_endpoint.protocol());

        boost::asio::socket_base::reuse_address roption(true);
        d_socket->set_option(roption);

        d_socket->bind(d_endpoint);

        start_receive();
        d_udp_thread = gr::thread::thread(boost::bind(&ax25_udp_pdu_gen_impl::run_io_service, this));
        d_connected = true;
      }
    }

    void
    ax25_udp_pdu_gen_impl::disconnect()
    {
      gr::thread::scoped_lock lock(d_setlock);

      if(!d_connected)
        return;

      d_io_service.reset();
      d_io_service.stop();
      d_udp_thread.join();

      d_socket->close();
      delete d_socket;

      d_connected = false;
    }

    // Return port number of d_socket
    int
    ax25_udp_pdu_gen_impl::get_port(void)
    {
      //return d_endpoint.port();
      return d_socket->local_endpoint().port();
    }
    
    void
    ax25_udp_pdu_gen_impl::start_receive()
    {
      d_socket->async_receive_from(boost::asio::buffer((void*)d_rxbuf, d_payload_size), d_endpoint_rcvd,
                                   boost::bind(&ax25_udp_pdu_gen_impl::handle_read, this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred));
    }

    /**
     * Add a address to the the data array of raw packet data
     * @method push_address_to_data
     * @static
     * @param {Array} data Raw data array
     * @param {String} address Address to add
     * @param {int} ssid SSID of the address
     * @param {int} ssid_top Top 3 bits of SSID Field to use depending on type of address
    */
    void
    ax25_udp_pdu_gen_impl::address_to_data(uint8_t * data, std::string address, uint8_t ssid_top){
 
        int ssid_loc = -1;
        for(int i = 0; i < 6; i++){

            if(address[i] == '-') ssid_loc = i+1;

            if(ssid_loc == -1 && i < address.length()) data[i] = address[i] << 1;
            else data[i] = 0x40; // 0x20 (space) << 1
        }

        if(ssid_loc == -1) ssid_loc = 7;
        uint8_t ssid = std::atoi(address.substr(ssid_loc).c_str());

        data[6] = ((ssid << 1) | ssid_top);
    }
 

    void
    ax25_udp_pdu_gen_impl::handle_read(const boost::system::error_code& error,
                                 size_t bytes_transferred)
    {
      if(!error) {
        {
          boost::lock_guard<gr::thread::mutex> lock(d_udp_mutex);
          if(bytes_transferred == 0) {
            // If we are using EOF notification, test for it and don't
            // add anything to the output.
            d_residual = WORK_DONE;
            d_cond_wait.notify_one();
            return;
          }
          else {

            // Packets are split into 250 byte segments

            int remaining_bytes = bytes_transferred;
            uint8_t fragment = 0;
            while(remaining_bytes > 0){

              uint8_t packet[280];
              int data_bytes = 250;
              if(remaining_bytes < 250) data_bytes = remaining_bytes;

              // Add addressing
              address_to_data(packet,   d_dest_addr, 0xE0);
              address_to_data(packet+7, d_src_addr, 0x60);
              packet[13] |= 1; // End of addresses

              // Add Control and PID
              packet[14] = 0x03; // Unnumbered Information Frame
              packet[15] = 0xF0; // No Layer 3

              packet[16] = 0x01; // Indicate this is UDP data
              packet[17] = (d_packet_seq << 3) | fragment++; // packet fragment number

              remaining_bytes -= data_bytes;
              if(remaining_bytes <= 0){
                packet[17] |= 0x80; // Set high bit to indicate end of packet
              }

              memcpy(packet+18, d_rxbuf+((fragment-1)*250), data_bytes);
              message_port_pub(pmt::mp("pdu"), pmt::cons(pmt::PMT_NIL, pmt::init_u8vector(data_bytes+18, (const uint8_t *) &packet)));

            }
          }

          d_packet_seq++; // Packet sequence numbering to tell packets appart incase of dataloss
          if(d_packet_seq >= 0x0F) d_packet_seq = 0;

          d_cond_wait.notify_one();
        }
      }
      start_receive();
    }

  } /* namespace spacegrant */
} /* namespace gr */

