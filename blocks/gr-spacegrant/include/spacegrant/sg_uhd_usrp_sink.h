/* -*- c++ -*- */
/* 
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_SPACEGRANT_SG_UHD_USRP_SINK_H
#define INCLUDED_SPACEGRANT_SG_UHD_USRP_SINK_H

#include <spacegrant/api.h>
#include <gnuradio/sync_block.h>
#include <uhd/usrp/multi_usrp.hpp>

namespace gr {
  namespace spacegrant {

    /*!
     * \brief <+description of block+>
     * \ingroup spacegrant
     *
     */
    class SPACEGRANT_API sg_uhd_usrp_sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<sg_uhd_usrp_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of spacegrant::sg_uhd_usrp_sink.
       *
       * To avoid accidental use of raw pointers, spacegrant::sg_uhd_usrp_sink's
       * constructor is in a private implementation
       * class. spacegrant::sg_uhd_usrp_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string& device_addr,
                       const ::uhd::stream_args_t& stream_args,
                       const std::string& length_tag_name);

      virtual ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                                   size_t chan = 0) = 0;
      ::uhd::tune_result_t set_center_freq(double freq, size_t chan = 0)
      {
          return set_center_freq(::uhd::tune_request_t(freq), chan);
      }
      virtual void set_samp_rate(double rate) = 0;

    };

  } // namespace spacegrant
} // namespace gr

#endif /* INCLUDED_SPACEGRANT_SG_UHD_USRP_SINK_H */

