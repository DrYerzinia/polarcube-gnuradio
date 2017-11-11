#ifndef INCLUDED_SPACEGRANT_AX25_UDP_PDU_GEN_H
#define INCLUDED_SPACEGRANT_AX25_UDP_PDU_GEN_H

#include <spacegrant/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace spacegrant {

    /*!
     * \brief <+description of block+>
     * \ingroup spacegrant
     *
     */
    class SPACEGRANT_API ax25_udp_pdu_gen : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<ax25_udp_pdu_gen> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of spacegrant::ax25_udp_pdu_gen.
       *
       * To avoid accidental use of raw pointers, spacegrant::ax25_udp_pdu_gen's
       * constructor is in a private implementation
       * class. spacegrant::ax25_udp_pdu_gen::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string &host, int port, const std::string &src_addr, const std::string &dest_addr);

      /*! \brief Change the connection to a new destination
       *
       * \param host         The name or IP address of the receiving host; use
       *                     NULL or None to break the connection without closing
       * \param port         Destination port to connect to on receiving host
       *
       * Calls disconnect() to terminate any current connection first.
       */
      virtual void connect(const std::string &host, int port) = 0;

      /*! \brief Cut the connection if we have one set up.
       */
      virtual void disconnect() = 0;

      /*! \brief return the PAYLOAD_SIZE of the socket */
      virtual int payload_size() = 0;

      /*! \brief return the port number of the socket */
      virtual int get_port() = 0;

    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_AX25_UDP_PDU_GEN_H */

