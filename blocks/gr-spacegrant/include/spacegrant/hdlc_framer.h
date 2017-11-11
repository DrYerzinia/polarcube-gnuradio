#ifndef INCLUDED_DIGITAL_HDLC_FRAMER_H
#define INCLUDED_DIGITAL_HDLC_FRAMER_H

#include <spacegrant/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace spacegrant {

    /*!
     * \brief HDLC framer which takes in PMT binary blobs and outputs HDLC
     * frames as unpacked bits, with CRC and bit stuffing added. The first sample
     * of the frame is tagged with the tag frame_tag_name and includes a
     * length field for tagged_stream use.
     *
     * This block outputs one whole frame at a time; if there is not enough
     * output buffer space to fit a frame, it is pushed onto a queue. As a result
     * flowgraphs which only run for a finite number of samples may not receive
     * all frames in the queue, due to the scheduler's granularity. For
     * flowgraphs that stream continuously (anything using a USRP) this should
     * not be an issue.
     *
     * \ingroup pkt_operators_blk
     *
     */
    class SPACEGRANT_API hdlc_framer : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<hdlc_framer> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of digital::hdlc_framer.
       *
       * \param frame_tag_name: The tag to add to the first sample of each frame.
       */
      static sptr make(const std::string frame_tag_name, const int number_of_flags);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_FRAMER_H */


