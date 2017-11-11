#ifndef INCLUDED_DIGITAL_HDLC_FRAMER_IMPL_H
#define INCLUDED_DIGITAL_HDLC_FRAMER_IMPL_H

#include <spacegrant/hdlc_framer.h>
#include <pmt/pmt.h>

namespace gr {
  namespace spacegrant {

    class hdlc_framer_impl : public hdlc_framer
    {
     private:
        std::vector<std::vector<unsigned char> > d_leftovers;
        pmt::pmt_t d_frame_tag, d_me;
        int number_of_flags;
        unsigned int crc_ccitt(std::vector<unsigned char> &data);
        std::vector<unsigned char> unpack(std::vector<unsigned char> &pkt);
        void stuff(std::vector<unsigned char> &pkt);

     public:
      hdlc_framer_impl(const std::string frame_tag_name, const int number_of_flags);
      ~hdlc_framer_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_FRAMER_IMPL_H */


