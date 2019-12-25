/* -*- c++ -*- */
/*
 * Copyright 2010-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

//#include "usrp_block_impl.h"

//#include <gnuradio/uhd/usrp_sink.h>
#include <pmt/pmt.h>
#include <uhd/usrp/multi_usrp.hpp>
#include <gnuradio/uhd/api.h>
#include <boost/bind.hpp>
#include <boost/dynamic_bitset.hpp>

#include <spacegrant/sg_uhd_usrp_sink.h>

#include <uhd/convert.hpp>

static const pmt::pmt_t SOB_KEY = pmt::string_to_symbol("tx_sob");
static const pmt::pmt_t EOB_KEY = pmt::string_to_symbol("tx_eob");
static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("tx_time");
static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("tx_freq");
static const pmt::pmt_t COMMAND_KEY = pmt::string_to_symbol("tx_command");

// Asynchronous message handling related PMTs
static const pmt::pmt_t ASYNC_MSG_KEY = pmt::string_to_symbol("uhd_async_msg");
static const pmt::pmt_t CHANNEL_KEY = pmt::string_to_symbol("channel");
static const pmt::pmt_t TIME_SPEC_KEY = pmt::string_to_symbol("time_spec");
static const pmt::pmt_t EVENT_CODE_KEY = pmt::string_to_symbol("event_code");
static const pmt::pmt_t BURST_ACK_KEY = pmt::string_to_symbol("burst_ack");
static const pmt::pmt_t UNDERFLOW_KEY = pmt::string_to_symbol("underflow");
static const pmt::pmt_t UNDERFLOW_IN_PACKET_KEY =
    pmt::string_to_symbol("underflow_in_packet");
static const pmt::pmt_t SEQ_ERROR_KEY = pmt::string_to_symbol("seq_error");
static const pmt::pmt_t SEQ_ERROR_IN_BURST_KEY =
    pmt::string_to_symbol("seq_error_in_burst");
static const pmt::pmt_t TIME_ERROR_KEY = pmt::string_to_symbol("time_error");
static const pmt::pmt_t ASYNC_MSGS_PORT_KEY = pmt::string_to_symbol("async_msgs");


namespace gr { namespace uhd {
    GR_UHD_API const pmt::pmt_t cmd_chan_key();
    GR_UHD_API const pmt::pmt_t cmd_gain_key();
    GR_UHD_API const pmt::pmt_t cmd_freq_key();
    GR_UHD_API const pmt::pmt_t cmd_lo_offset_key();
    GR_UHD_API const pmt::pmt_t cmd_tune_key();
    GR_UHD_API const pmt::pmt_t cmd_lo_freq_key();
    GR_UHD_API const pmt::pmt_t cmd_dsp_freq_key();
    GR_UHD_API const pmt::pmt_t cmd_rate_key();
    GR_UHD_API const pmt::pmt_t cmd_bandwidth_key();
    GR_UHD_API const pmt::pmt_t cmd_time_key();
    GR_UHD_API const pmt::pmt_t cmd_mboard_key();
    GR_UHD_API const pmt::pmt_t cmd_antenna_key();
    GR_UHD_API const pmt::pmt_t cmd_direction_key();
    GR_UHD_API const pmt::pmt_t cmd_tag_key();

    GR_UHD_API const pmt::pmt_t ant_direction_rx();
    GR_UHD_API const pmt::pmt_t ant_direction_tx();
}}

namespace gr {
namespace spacegrant {

inline io_signature::sptr args_to_io_sig(const ::uhd::stream_args_t& args)
{
    const size_t nchan = std::max<size_t>(args.channels.size(), 1);
    const size_t size = ::uhd::convert::get_bytes_per_item(args.cpu_format);
    return io_signature::make(nchan, nchan, size);
}

//static const std::string ALL_GAINS = "";//::uhd::usrp::multi_usrp::ALL_GAINS;

#ifdef UHD_USRP_MULTI_USRP_LO_CONFIG_API
static const std::string ALL_LOS = "all";//::uhd::usrp::multi_usrp::ALL_LOS;
#else
static const std::string ALL_LOS;
#endif

/***********************************************************************
 * UHD Multi USRP Sink Impl
 **********************************************************************/
class usrp_sink_impl : public sg_uhd_usrp_sink //, public usrp_block_impl
{
public:

    usrp_sink_impl(const ::uhd::device_addr_t& device_addr,
                   const ::uhd::stream_args_t& stream_args,
                   const std::string& length_tag_name);
    ~usrp_sink_impl();

    void recv_loop();

    ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan);
    double get_samp_rate(void);
    ::uhd::meta_range_t get_samp_rates(void);
    double get_center_freq(size_t chan);
    ::uhd::freq_range_t get_freq_range(size_t chan);
    double get_gain(size_t chan);
    double get_gain(const std::string& name, size_t chan);
    double get_normalized_gain(size_t chan);
    std::vector<std::string> get_gain_names(size_t chan);
    ::uhd::gain_range_t get_gain_range(size_t chan);
    ::uhd::gain_range_t get_gain_range(const std::string& name, size_t chan);
    std::string get_antenna(size_t chan);
    std::vector<std::string> get_antennas(size_t chan);
    ::uhd::sensor_value_t get_sensor(const std::string& name, size_t chan);
    std::vector<std::string> get_sensor_names(size_t chan);
    ::uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan);
    std::vector<std::string> get_lo_names(size_t chan);
    const std::string get_lo_source(const std::string& name, size_t chan);
    std::vector<std::string> get_lo_sources(const std::string& name, size_t chan);
    bool get_lo_export_enabled(const std::string& name, size_t chan);
    double get_lo_freq(const std::string& name, size_t chan);
    ::uhd::freq_range_t get_lo_freq_range(const std::string& name, size_t chan);

    void set_subdev_spec(const std::string& spec, size_t mboard);
    std::string get_subdev_spec(size_t mboard);
    void set_samp_rate(double rate);
    ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                         size_t chan);
    void set_gain(double gain, size_t chan);
    void set_gain(double gain, const std::string& name, size_t chan);
    void set_normalized_gain(double gain, size_t chan);
    void set_antenna(const std::string& ant, size_t chan);
    void set_bandwidth(double bandwidth, size_t chan);
    double get_bandwidth(size_t chan);
    ::uhd::freq_range_t get_bandwidth_range(size_t chan);
    void set_dc_offset(const std::complex<double>& offset, size_t chan);
    void set_iq_balance(const std::complex<double>& correction, size_t chan);
    void set_stream_args(const ::uhd::stream_args_t& stream_args);
    void set_start_time(const ::uhd::time_spec_t& time);
    void set_lo_source(const std::string& src,
                       const std::string& name = ALL_LOS,
                       size_t chan = 0);
    void set_lo_export_enabled(bool enabled,
                               const std::string& name = ALL_LOS,
                               size_t chan = 0);
    double set_lo_freq(double freq, const std::string& name, size_t chan);

    bool start(void);
    bool stop(void);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    inline void tag_work(int& ninput_items);

    void setup_rpc();

    // USRP_BLOCK /////////////////////////////////////////////////////////////
        typedef boost::function<::uhd::sensor_value_t(const std::string&)> get_sensor_fn_t;
    typedef boost::function<void(const pmt::pmt_t&, int, const pmt::pmt_t&)>
        cmd_handler_t;

    static const double LOCK_TIMEOUT;

    ::uhd::sensor_value_t get_mboard_sensor(const std::string& name, size_t mboard);
    std::vector<std::string> get_mboard_sensor_names(size_t mboard);
    std::string get_time_source(const size_t mboard);
    std::vector<std::string> get_time_sources(const size_t mboard);
    std::string get_clock_source(const size_t mboard);
    std::vector<std::string> get_clock_sources(const size_t mboard);
    double get_clock_rate(size_t mboard);
    ::uhd::time_spec_t get_time_now(size_t mboard = 0);
    ::uhd::time_spec_t get_time_last_pps(size_t mboard);
    ::uhd::usrp::multi_usrp::sptr get_device(void);
    std::vector<std::string> get_gpio_banks(const size_t mboard);
    boost::uint32_t get_gpio_attr(const std::string& bank,
                                  const std::string& attr,
                                  const size_t mboard = 0);
    size_t get_num_mboards();

    // Setters
    void set_time_source(const std::string& source, const size_t mboard);
    void set_clock_source(const std::string& source, const size_t mboard);
    void set_clock_rate(double rate, size_t mboard);
    void set_time_now(const ::uhd::time_spec_t& time_spec, size_t mboard);
    void set_time_next_pps(const ::uhd::time_spec_t& time_spec);
    void set_time_unknown_pps(const ::uhd::time_spec_t& time_spec);
    void set_command_time(const ::uhd::time_spec_t& time_spec, size_t mboard);
    void set_user_register(const uint8_t addr, const uint32_t data, size_t mboard);
    void clear_command_time(size_t mboard);
    void set_gpio_attr(const std::string& bank,
                       const std::string& attr,
                       const boost::uint32_t value,
                       const boost::uint32_t mask,
                       const size_t mboard);
    ///////////////////////////////////////////////////////////////////////////

protected:

    gr::thread::thread _amsg_thread;
    bool _running;

    bool _transmitting;

    // USRP_BLOCK /////////////////////////////////////////////////////////////
 
    /**********************************************************************
     * Command Interface
     **********************************************************************/
    //! Receives commands and handles them
    void msg_handler_command(pmt::pmt_t msg);

    //! For a given argument, call the associated handler, or if none exists,
    // show a warning through the logging interface.
    void dispatch_msg_cmd_handler(const pmt::pmt_t& cmd,
                                  const pmt::pmt_t& val,
                                  int chan,
                                  pmt::pmt_t& msg);

    //! Register a new handler for command key \p cmd
    void register_msg_cmd_handler(const pmt::pmt_t& cmd, cmd_handler_t handler);


    // Default handlers
    void _cmd_handler_freq(const pmt::pmt_t& freq, int chan, const pmt::pmt_t& msg);
    void
    _cmd_handler_looffset(const pmt::pmt_t& lo_offset, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_gain(const pmt::pmt_t& gain, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_antenna(const pmt::pmt_t& ant, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_rate(const pmt::pmt_t& rate, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_tune(const pmt::pmt_t& tune, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_bw(const pmt::pmt_t& bw, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_lofreq(const pmt::pmt_t& lofreq, int chan, const pmt::pmt_t& msg);
    void _cmd_handler_dspfreq(const pmt::pmt_t& dspfreq, int chan, const pmt::pmt_t& msg);

    /**********************************************************************
     * Helpers
     **********************************************************************/
    bool _check_mboard_sensors_locked();

    void _update_stream_args(const ::uhd::stream_args_t& stream_args_);

    // should be const, doesn't work though 'cause missing operator=() for tune_request_t
    void _update_curr_tune_req(::uhd::tune_request_t& tune_req, int chan);

    /*! \brief Wait until a timeout or a sensor returns 'locked'.
     *
     * If a given sensor is not found, this still returns 'true', so we don't throw
     * errors or warnings if a sensor wasn't implemented.
     *
     * \returns true if the sensor locked in time or doesn't exist
     */
    bool _wait_for_locked_sensor(std::vector<std::string> sensor_names,
                                 const std::string& sensor_name,
                                 get_sensor_fn_t get_sensor_fn);

    //! Helper function for msg_handler_command:
    // - Extracts command and the command value from the command PMT
    // - Returns true if the command PMT is well formed
    // - If a channel is given, return that as well, otherwise set the channel to -1
    static bool _unpack_chan_command(std::string& command,
                                     pmt::pmt_t& cmd_val,
                                     int& chan,
                                     const pmt::pmt_t& cmd_pmt);

    //! Helper function for msg_handler_command:
    // - Sets a value in vector_to_update to cmd_val, depending on chan
    // - If chan is a positive integer, it will set vector_to_update[chan]
    // - If chan is -1, it depends on minus_one_updates_all:
    //   - Either set vector_to_update[0] or
    //   - Set *all* entries in vector_to_update
    // - Returns a dynamic_bitset, all indexes that where changed in
    //   vector_to_update are set to 1
    template <typename T>
    static boost::dynamic_bitset<>
    _update_vector_from_cmd_val(std::vector<T>& vector_to_update,
                                int chan,
                                const T cmd_val,
                                bool minus_one_updates_all = false)
    {
        boost::dynamic_bitset<> vals_updated(vector_to_update.size());
        if (chan == -1) {
            if (minus_one_updates_all) {
                for (size_t i = 0; i < vector_to_update.size(); i++) {
                    if (vector_to_update[i] != cmd_val) {
                        vals_updated[i] = true;
                        vector_to_update[i] = cmd_val;
                    }
                }
                return vals_updated;
            }
            chan = 0;
        }
        if (vector_to_update[chan] != cmd_val) {
            vector_to_update[chan] = cmd_val;
            vals_updated[chan] = true;
        }

        return vals_updated;
    }

    //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
    //virtual ::uhd::tune_result_t
    //_set_center_freq_from_internals(size_t chan, pmt::pmt_t direction) = 0;

    //! Calls _set_center_freq_from_internals() on all channels
    void _set_center_freq_from_internals_allchans(pmt::pmt_t direction);

    /**********************************************************************
     * Members
     *********************************************************************/
    //! Shared pointer to the underlying multi_usrp object
    ::uhd::usrp::multi_usrp::sptr _dev;
    ::uhd::stream_args_t _stream_args;
    //! Number of channels (i.e. number of in- or outputs)
    size_t _nchan;
    bool _stream_now;
    ::uhd::time_spec_t _start_time;
    bool _start_time_set;

    /****** Command interface related **********/
    //! Stores a list of commands for later execution
    std::vector<pmt::pmt_t> _pending_cmds;
    //! Shadows the last value we told the USRP to tune to for every channel
    // (this is not necessarily the true value the USRP is currently tuned to!).
    std::vector<::uhd::tune_request_t> _curr_tune_req;
    boost::dynamic_bitset<> _chans_to_tune;

    //! Stores the individual command handlers
    ::uhd::dict<pmt::pmt_t, cmd_handler_t> _msg_cmd_handlers;
    ///////////////////////////////////////////////////////////////////////////

private:
    //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
    ::uhd::tune_result_t _set_center_freq_from_internals(size_t chan,
                                                         pmt::pmt_t direction);

    ::uhd::tx_streamer::sptr _tx_stream;
    ::uhd::tx_metadata_t _metadata;
    double _sample_rate;

    // stream tags related stuff
    std::vector<tag_t> _tags;
    const pmt::pmt_t _length_tag_key;
    long _nitems_to_send;

    // asynchronous messages related stuff
    bool _async_event_loop_running;
    void async_event_loop();
    gr::thread::thread _async_event_thread;
};

} /* namespace uhd */
} /* namespace gr */
