/* -*- c++ -*- */
/*
 * Copyright 2010-2016,2018 Free Software Foundation, Inc.
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

#include <iostream>

#include "gr_uhd_common.h"
#include "sg_uhd_usrp_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>
#include <stdexcept>

const pmt::pmt_t gr::uhd::cmd_chan_key()
{
    static const pmt::pmt_t val = pmt::mp("chan");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_gain_key()
{
    static const pmt::pmt_t val = pmt::mp("gain");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_freq_key()
{
    static const pmt::pmt_t val = pmt::mp("freq");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_lo_offset_key()
{
    static const pmt::pmt_t val = pmt::mp("lo_offset");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_tune_key()
{
    static const pmt::pmt_t val = pmt::mp("tune");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_lo_freq_key()
{
    static const pmt::pmt_t val = pmt::mp("lo_freq");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_dsp_freq_key()
{
    static const pmt::pmt_t val = pmt::mp("dsp_freq");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_rate_key()
{
    static const pmt::pmt_t val = pmt::mp("rate");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_bandwidth_key()
{
    static const pmt::pmt_t val = pmt::mp("bandwidth");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_time_key()
{
    static const pmt::pmt_t val = pmt::mp("time");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_mboard_key()
{
    static const pmt::pmt_t val = pmt::mp("mboard");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_antenna_key()
{
    static const pmt::pmt_t val = pmt::mp("antenna");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_direction_key()
{
    static const pmt::pmt_t val = pmt::mp("direction");
    return val;
}
const pmt::pmt_t gr::uhd::cmd_tag_key()
{
    static const pmt::pmt_t val = pmt::mp("tag");
    return val;
}

const pmt::pmt_t gr::uhd::ant_direction_rx()
{
    static const pmt::pmt_t val = pmt::mp("RX");
    return val;
}
const pmt::pmt_t gr::uhd::ant_direction_tx()
{
    static const pmt::pmt_t val = pmt::mp("TX");
    return val;
}

namespace gr {
namespace spacegrant {

const double usrp_sink_impl::LOCK_TIMEOUT = 1.5;

sg_uhd_usrp_sink::sptr sg_uhd_usrp_sink::make(const std::string& device_addr,
                                              const ::uhd::stream_args_t& stream_args,
                                              const std::string& length_tag_name)
{
    check_abi();

    std::cerr << "sg_uhd_usrp_sink::sptr sg_uhd_usrp_sink::make" << std::endl;

    ::uhd::device_addr_t device_addr_2(device_addr);

    return sg_uhd_usrp_sink::sptr(new usrp_sink_impl(
        device_addr_2, stream_args_ensure(stream_args), length_tag_name));
}

usrp_sink_impl::usrp_sink_impl(const ::uhd::device_addr_t& device_addr,
                               const ::uhd::stream_args_t& stream_args,
                               const std::string& length_tag_name)
      : sync_block("usrp_sink", args_to_io_sig(stream_args), io_signature::make(0, 0, 0)),
      _stream_args(stream_args),
      _nchan(stream_args.channels.size()),
      _stream_now(_nchan == 1 and length_tag_name.empty()),
      _start_time_set(false),
      _curr_tune_req(stream_args.channels.size(), ::uhd::tune_request_t()),
      _chans_to_tune(stream_args.channels.size()),
      _length_tag_key(length_tag_name.empty() ? pmt::PMT_NIL
                                              : pmt::string_to_symbol(length_tag_name)),
      _nitems_to_send(0),
      _async_event_loop_running(true)

{

    std::cerr << "usrp_sink_impl::usrp_sink_impl" << std::endl;

    _dev = ::uhd::usrp::multi_usrp::make(device_addr);

    _check_mboard_sensors_locked();

    // Set up message ports:
    /*
    message_port_register_in(pmt::mp("command"));
    set_msg_handler(pmt::mp("command"),
                    boost::bind(&usrp_block_impl::msg_handler_command, this, _1));

    // cuz we lazy:
    #define REGISTER_CMD_HANDLER(key, _handler) \
        register_msg_cmd_handler(key,           \
                                boost::bind(&usrp_block_impl::_handler, this, _1, _2, _3))
        // Register default command handlers:
        REGISTER_CMD_HANDLER(cmd_freq_key(), _cmd_handler_freq);
        REGISTER_CMD_HANDLER(cmd_gain_key(), _cmd_handler_gain);
        REGISTER_CMD_HANDLER(cmd_lo_offset_key(), _cmd_handler_looffset);
        REGISTER_CMD_HANDLER(cmd_tune_key(), _cmd_handler_tune);
        REGISTER_CMD_HANDLER(cmd_lo_freq_key(), _cmd_handler_lofreq);
        REGISTER_CMD_HANDLER(cmd_dsp_freq_key(), _cmd_handler_dspfreq);
        REGISTER_CMD_HANDLER(cmd_rate_key(), _cmd_handler_rate);
        REGISTER_CMD_HANDLER(cmd_bandwidth_key(), _cmd_handler_bw);
        REGISTER_CMD_HANDLER(cmd_antenna_key(), _cmd_handler_antenna);
        */

    //message_port_register_out(ASYNC_MSGS_PORT_KEY);
    //_async_event_thread = gr::thread::thread([this]() { this->async_event_loop(); });
    //_sample_rate = get_samp_rate();
}

usrp_sink_impl::~usrp_sink_impl()
{
    _async_event_loop_running = false;
    _async_event_thread.join();
}

::uhd::dict<std::string, std::string> usrp_sink_impl::get_usrp_info(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_usrp_tx_info(chan);
}

void usrp_sink_impl::set_subdev_spec(const std::string& spec, size_t mboard)
{
    return _dev->set_tx_subdev_spec(spec, mboard);
}

std::string usrp_sink_impl::get_subdev_spec(size_t mboard)
{
    return _dev->get_tx_subdev_spec(mboard).to_string();
}

void usrp_sink_impl::set_samp_rate(double rate)
{
    BOOST_FOREACH (const size_t chan, _stream_args.channels) {
        _dev->set_tx_rate(rate, chan);
    }
    _sample_rate = this->get_samp_rate();
}

double usrp_sink_impl::get_samp_rate(void)
{
    return _dev->get_tx_rate(_stream_args.channels[0]);
}

::uhd::meta_range_t usrp_sink_impl::get_samp_rates(void)
{
    return _dev->get_tx_rates(_stream_args.channels[0]);
}

::uhd::tune_result_t
usrp_sink_impl::set_center_freq(const ::uhd::tune_request_t tune_request, size_t chan)
{
    _curr_tune_req[chan] = tune_request;
    chan = _stream_args.channels[chan];
    return _dev->set_tx_freq(tune_request, chan);
}

::uhd::tune_result_t usrp_sink_impl::_set_center_freq_from_internals(size_t chan,
                                                                     pmt::pmt_t direction)
{
    _chans_to_tune.reset(chan);
    if (pmt::eqv(direction, gr::uhd::ant_direction_rx())) {
        // TODO: what happens if the RX device is not instantiated? Catch error?
        return _dev->set_rx_freq(_curr_tune_req[chan], _stream_args.channels[chan]);
    } else {
        return _dev->set_tx_freq(_curr_tune_req[chan], _stream_args.channels[chan]);
    }
}

double usrp_sink_impl::get_center_freq(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_freq(chan);
}

::uhd::freq_range_t usrp_sink_impl::get_freq_range(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_freq_range(chan);
}

void usrp_sink_impl::set_gain(double gain, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_tx_gain(gain, chan);
}

void usrp_sink_impl::set_gain(double gain, const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_tx_gain(gain, name, chan);
}

void usrp_sink_impl::set_normalized_gain(double norm_gain, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_NORMALIZED_GAIN
    _dev->set_normalized_tx_gain(norm_gain, chan);
#else
    if (norm_gain > 1.0 || norm_gain < 0.0) {
        throw std::runtime_error("Normalized gain out of range, must be in [0, 1].");
    }
    ::uhd::gain_range_t gain_range = get_gain_range(chan);
    double abs_gain =
        (norm_gain * (gain_range.stop() - gain_range.start())) + gain_range.start();
    set_gain(abs_gain, chan);
#endif
}

double usrp_sink_impl::get_gain(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_gain(chan);
}

double usrp_sink_impl::get_gain(const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_gain(name, chan);
}

double usrp_sink_impl::get_normalized_gain(size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_NORMALIZED_GAIN
    return _dev->get_normalized_tx_gain(chan);
#else
    ::uhd::gain_range_t gain_range = get_gain_range(chan);
    double norm_gain =
        (get_gain(chan) - gain_range.start()) / (gain_range.stop() - gain_range.start());
    // Avoid rounding errors:
    if (norm_gain > 1.0)
        return 1.0;
    if (norm_gain < 0.0)
        return 0.0;
    return norm_gain;
#endif
}

std::vector<std::string> usrp_sink_impl::get_gain_names(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_gain_names(chan);
}

::uhd::gain_range_t usrp_sink_impl::get_gain_range(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_gain_range(chan);
}

::uhd::gain_range_t usrp_sink_impl::get_gain_range(const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_gain_range(name, chan);
}

void usrp_sink_impl::set_antenna(const std::string& ant, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_tx_antenna(ant, chan);
}

std::string usrp_sink_impl::get_antenna(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_antenna(chan);
}

std::vector<std::string> usrp_sink_impl::get_antennas(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_antennas(chan);
}

void usrp_sink_impl::set_bandwidth(double bandwidth, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_tx_bandwidth(bandwidth, chan);
}

double usrp_sink_impl::get_bandwidth(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_bandwidth(chan);
}

::uhd::freq_range_t usrp_sink_impl::get_bandwidth_range(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_bandwidth_range(chan);
}
std::vector<std::string> usrp_sink_impl::get_lo_names(size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_tx_lo_names(chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

const std::string usrp_sink_impl::get_lo_source(const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_tx_lo_source(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

std::vector<std::string> usrp_sink_impl::get_lo_sources(const std::string& name,
                                                        size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_tx_lo_sources(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_sink_impl::set_lo_source(const std::string& src,
                                   const std::string& name,
                                   size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->set_tx_lo_source(src, name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

bool usrp_sink_impl::get_lo_export_enabled(const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_tx_lo_export_enabled(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_sink_impl::set_lo_export_enabled(bool enabled,
                                           const std::string& name,
                                           size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->set_tx_lo_export_enabled(enabled, name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

::uhd::freq_range_t usrp_sink_impl::get_lo_freq_range(const std::string& name,
                                                      size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_tx_lo_freq_range(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

double usrp_sink_impl::get_lo_freq(const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->get_tx_lo_freq(name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

double usrp_sink_impl::set_lo_freq(double freq, const std::string& name, size_t chan)
{
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
    chan = _stream_args.channels[chan];
    return _dev->set_tx_lo_freq(freq, name, chan);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_sink_impl::set_dc_offset(const std::complex<double>& offset, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_tx_dc_offset(offset, chan);
}

void usrp_sink_impl::set_iq_balance(const std::complex<double>& correction, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->set_tx_iq_balance(correction, chan);
}

::uhd::sensor_value_t usrp_sink_impl::get_sensor(const std::string& name, size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_sensor(name, chan);
}

std::vector<std::string> usrp_sink_impl::get_sensor_names(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_sensor_names(chan);
}

::uhd::usrp::dboard_iface::sptr usrp_sink_impl::get_dboard_iface(size_t chan)
{
    chan = _stream_args.channels[chan];
    return _dev->get_tx_dboard_iface(chan);
}

void usrp_sink_impl::set_stream_args(const ::uhd::stream_args_t& stream_args)
{
    _update_stream_args(stream_args);
    if (_tx_stream) {
        _tx_stream.reset();
    }
}

/***********************************************************************
 * Work
 **********************************************************************/
int usrp_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{

    //std::cerr << "Usrp_sink_impl::work" << std::endl;

    int ninput_items = noutput_items; // cuz it's a sync block

    // default to send a mid-burst packet
    _metadata.start_of_burst = false;
    _metadata.end_of_burst = false;

    // collect tags in this work()
    const uint64_t samp0_count = nitems_read(0);
    get_tags_in_range(_tags, 0, samp0_count, samp0_count + ninput_items);
    if (not _tags.empty())
        this->tag_work(ninput_items);

    if (not pmt::is_null(_length_tag_key)) {
        // check if there is data left to send from a burst tagged with length_tag
        // If a burst is started during this call to work(), tag_work() should have
        // been called and we should have _nitems_to_send > 0.
        if (_nitems_to_send > 0) {
            ninput_items = std::min<long>(_nitems_to_send, ninput_items);
            // if we run out of items to send, it's the end of the burst
            if (_nitems_to_send - long(ninput_items) == 0)
                _metadata.end_of_burst = true;
        } else {
            // There is a tag gap since no length_tag was found immediately following
            // the last sample of the previous burst. Drop samples until the next
            // length_tag is found. Notify the user of the tag gap.
            std::cerr << "tG" << std::flush;
            // increment the timespec by the number of samples dropped
            _metadata.time_spec += ::uhd::time_spec_t(0, ninput_items, _sample_rate);
            return ninput_items;
        }
    }

    // send all ninput_items with metadata
    boost::this_thread::disable_interruption disable_interrupt;
    const size_t num_sent = _tx_stream->send(input_items, ninput_items, _metadata, 1.0);
    boost::this_thread::restore_interruption restore_interrupt(disable_interrupt);

    // if using length_tags, decrement items left to send by the number of samples sent
    if (not pmt::is_null(_length_tag_key) && _nitems_to_send > 0) {
        _nitems_to_send -= long(num_sent);
    }

    // increment the timespec by the number of samples sent
    _metadata.time_spec += ::uhd::time_spec_t(0, num_sent, _sample_rate);

    // Some post-processing tasks if we actually transmitted the entire burst
    if (not _pending_cmds.empty() && num_sent == size_t(ninput_items)) {
        GR_LOG_DEBUG(d_debug_logger,
                     boost::format("Executing %d pending commands.") %
                         _pending_cmds.size());
        BOOST_FOREACH (const pmt::pmt_t& cmd_pmt, _pending_cmds) {
            msg_handler_command(cmd_pmt);
        }
        _pending_cmds.clear();
    }

    return num_sent;
}

/***********************************************************************
 * Tag Work
 **********************************************************************/
void usrp_sink_impl::tag_work(int& ninput_items)
{

    std::cerr << "usrp_sink_impl::tag_work" << std::endl;

    // the for loop below assumes tags sorted by count low -> high
    std::sort(_tags.begin(), _tags.end(), tag_t::offset_compare);

    // extract absolute sample counts
    const uint64_t samp0_count = this->nitems_read(0);
    uint64_t max_count = samp0_count + ninput_items;

    // Go through tag list until something indicates the end of a burst.
    bool found_time_tag = false;
    bool found_eob = false;
    // For commands that are in the middle of the burst:
    std::vector<pmt::pmt_t> commands_in_burst; // Store the command
    uint64_t in_burst_cmd_offset = 0;          // Store its position
    BOOST_FOREACH (const tag_t& my_tag, _tags) {
        const uint64_t my_tag_count = my_tag.offset;
        const pmt::pmt_t& key = my_tag.key;
        const pmt::pmt_t& value = my_tag.value;

        if (my_tag_count >= max_count) {
            break;
        }

        /* I. Tags that can only be on the first sample of a burst
         *
         * This includes:
         * - tx_time
         * - tx_command TODO should also work end-of-burst
         * - tx_sob
         * - length tags
         *
         * With these tags, we check if they're on the first item, otherwise,
         * we stop before that tag so they are on the first item the next time round.
         */
        else if (pmt::equal(key, COMMAND_KEY)) {
            if (my_tag_count != samp0_count) {
                max_count = my_tag_count;
                break;
            }
            // TODO set the command time from the sample time
            msg_handler_command(value);
        }

        // set the time specification in the metadata
        else if (pmt::equal(key, TIME_KEY)) {
            if (my_tag_count != samp0_count) {
                max_count = my_tag_count;
                break;
            }
            found_time_tag = true;
            _metadata.has_time_spec = true;
            _metadata.time_spec =
                ::uhd::time_spec_t(pmt::to_uint64(pmt::tuple_ref(value, 0)),
                                   pmt::to_double(pmt::tuple_ref(value, 1)));
        }

        // set the start of burst flag in the metadata; ignore if length_tag_key is not
        // null
        else if (pmt::is_null(_length_tag_key) && pmt::equal(key, SOB_KEY)) {
            if (my_tag.offset != samp0_count) {
                max_count = my_tag_count;
                break;
            }
            // Bursty tx will not use time specs, unless a tx_time tag is also given.
            _metadata.has_time_spec = false;
            _metadata.start_of_burst = pmt::to_bool(value);
        }

        // length_tag found; set the start of burst flag in the metadata
        else if (not pmt::is_null(_length_tag_key) && pmt::equal(key, _length_tag_key)) {
            if (my_tag_count != samp0_count) {
                max_count = my_tag_count;
                break;
            }
            // If there are still items left to send, the current burst has been
            // preempted. Set the items remaining counter to the new burst length. Notify
            // the user of the tag preemption.
            else if (_nitems_to_send > 0) {
                std::cerr << "tP" << std::flush;
            }
            _nitems_to_send = pmt::to_long(value);
            _metadata.start_of_burst = true;
        }

        /* II. Tags that can be on the first OR last sample of a burst
         *
         * This includes:
         * - tx_freq
         *
         * With these tags, we check if they're at the start of a burst, and do
         * the appropriate action. Otherwise, make sure the corresponding sample
         * is the last one.
         */
        else if (pmt::equal(key, FREQ_KEY) && my_tag_count == samp0_count) {
            // If it's on the first sample, immediately do the tune:
            GR_LOG_DEBUG(d_debug_logger,
                         boost::format("Received tx_freq on start of burst."));
            pmt::pmt_t freq_cmd = pmt::make_dict();
            freq_cmd = pmt::dict_add(freq_cmd, gr::uhd::cmd_freq_key(), value);
            msg_handler_command(freq_cmd);
        } else if (pmt::equal(key, FREQ_KEY)) {
            // If it's not on the first sample, queue this command and only tx until here:
            GR_LOG_DEBUG(d_debug_logger, boost::format("Received tx_freq mid-burst."));
            pmt::pmt_t freq_cmd = pmt::make_dict();
            freq_cmd = pmt::dict_add(freq_cmd, gr::uhd::cmd_freq_key(), value);
            commands_in_burst.push_back(freq_cmd);
            max_count = my_tag_count + 1;
            in_burst_cmd_offset = my_tag_count;
        }

        /* III. Tags that can only be on the last sample of a burst
         *
         * This includes:
         * - tx_eob
         *
         * Make sure that no more samples are allowed through.
         */
        else if (pmt::is_null(_length_tag_key) && pmt::equal(key, EOB_KEY)) {
            found_eob = true;
            max_count = my_tag_count + 1;
            _metadata.end_of_burst = pmt::to_bool(value);
        }
    } // end foreach

    if (not pmt::is_null(_length_tag_key) &&
        long(max_count - samp0_count) == _nitems_to_send) {
        found_eob = true;
    }

    // If a command was found in-burst that may appear at the end of burst,
    // there's two options:
    // 1) The command was actually on the last sample (eob). Then, stash the
    //    commands for running after work().
    // 2) The command was not on the last sample. In this case, only send()
    //    until before the tag, so it will be on the first sample of the next run.
    if (not commands_in_burst.empty()) {
        if (not found_eob) {
            // ...then it's in the middle of a burst, only send() until before the tag
            max_count = in_burst_cmd_offset;
        } else if (in_burst_cmd_offset < max_count) {
            BOOST_FOREACH (const pmt::pmt_t& cmd_pmt, commands_in_burst) {
                _pending_cmds.push_back(cmd_pmt);
            }
        }
    }

    if (found_time_tag) {
        _metadata.has_time_spec = true;
    }

    // Only transmit up to and including end of burst,
    // or everything if no burst boundaries are found.
    ninput_items = int(max_count - samp0_count);

} // end tag_work()

void usrp_sink_impl::set_start_time(const ::uhd::time_spec_t& time)
{
    _start_time = time;
    _start_time_set = true;
    _stream_now = false;
}

// Send an empty start-of-burst packet to begin streaming.
// Set at a time in the near future to avoid late packets.
bool usrp_sink_impl::start(void)
{

    if (not _tx_stream)
        _tx_stream = _dev->get_tx_stream(_stream_args);

    _metadata.start_of_burst = true;
    _metadata.end_of_burst = false;
    // Bursty tx will need to send a tx_time to activate time spec
    _metadata.has_time_spec = !_stream_now && pmt::is_null(_length_tag_key);
    _nitems_to_send = 0;

    if (pmt::is_null(_length_tag_key)) { // don't execute this part in burst mode
        _metadata.start_of_burst = true;
        _metadata.end_of_burst = false;
        _metadata.has_time_spec = false;

        if (!_stream_now) {
            _metadata.has_time_spec = true;
            if (_start_time_set) {
                _start_time_set = false; // cleared for next run
                _metadata.time_spec = _start_time;
            } else {
                _metadata.time_spec = get_time_now() + ::uhd::time_spec_t(0.15);
            }
        }

        _tx_stream->send(gr_vector_const_void_star(_nchan), 0, _metadata, 1.0);
    }
    return true;
}

// Send an empty end-of-burst packet to end streaming.
// Ending the burst avoids an underflow error on stop.
bool usrp_sink_impl::stop(void)
{
    _metadata.start_of_burst = false;
    _metadata.end_of_burst = true;
    _metadata.has_time_spec = false;
    _nitems_to_send = 0;

    if (_tx_stream) {
        _tx_stream->send(gr_vector_const_void_star(_nchan), 0, _metadata, 1.0);
    }
    return true;
}


void usrp_sink_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(new rpcbasic_register_handler<usrp_block>(
        alias(), "command", "", "UHD Commands", RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
}

void usrp_sink_impl::async_event_loop()
{
    typedef ::uhd::async_metadata_t md_t;
    md_t metadata;

    while (_async_event_loop_running) {
        while (!_dev->get_device()->recv_async_msg(metadata, 0.1)) {
            if (!_async_event_loop_running) {
                return;
            }
        }

        pmt::pmt_t event_list = pmt::PMT_NIL;

        if (metadata.event_code & md_t::EVENT_CODE_BURST_ACK) {
            event_list = pmt::list_add(event_list, BURST_ACK_KEY);
        }
        if (metadata.event_code & md_t::EVENT_CODE_UNDERFLOW) {
            event_list = pmt::list_add(event_list, UNDERFLOW_KEY);
        }
        if (metadata.event_code & md_t::EVENT_CODE_UNDERFLOW_IN_PACKET) {
            event_list = pmt::list_add(event_list, UNDERFLOW_IN_PACKET_KEY);
        }
        if (metadata.event_code & md_t::EVENT_CODE_SEQ_ERROR) {
            event_list = pmt::list_add(event_list, SEQ_ERROR_KEY);
        }
        if (metadata.event_code & md_t::EVENT_CODE_SEQ_ERROR_IN_BURST) {
            event_list = pmt::list_add(event_list, SEQ_ERROR_IN_BURST_KEY);
        }
        if (metadata.event_code & md_t::EVENT_CODE_TIME_ERROR) {
            event_list = pmt::list_add(event_list, TIME_ERROR_KEY);
        }

        if (!pmt::eq(event_list, pmt::PMT_NIL)) {
            pmt::pmt_t value =
                pmt::dict_add(pmt::make_dict(), EVENT_CODE_KEY, event_list);
            if (metadata.has_time_spec) {
                pmt::pmt_t time_spec =
                    pmt::cons(pmt::from_long(metadata.time_spec.get_full_secs()),
                              pmt::from_double(metadata.time_spec.get_frac_secs()));
                value = pmt::dict_add(value, TIME_SPEC_KEY, time_spec);
            }
            value = pmt::dict_add(value, CHANNEL_KEY, pmt::from_uint64(metadata.channel));
            pmt::pmt_t msg = pmt::cons(ASYNC_MSG_KEY, value);
            message_port_pub(ASYNC_MSGS_PORT_KEY, msg);
        }
    }
}

// GR_BLOCK_IMPL //////////////////////////////////////////////////////////////

/**********************************************************************
 * Helpers
 *********************************************************************/
void usrp_sink_impl::_update_stream_args(const ::uhd::stream_args_t& stream_args_)
{
    ::uhd::stream_args_t stream_args(stream_args_);
    if (stream_args.channels.empty()) {
        stream_args.channels = _stream_args.channels;
    }
    if (stream_args.cpu_format != _stream_args.cpu_format ||
        stream_args.channels.size() != _stream_args.channels.size()) {
        throw std::runtime_error(
            "Cannot change I/O signatures while updating stream args!");
    }
    _stream_args = stream_args;
}

bool usrp_sink_impl::_wait_for_locked_sensor(std::vector<std::string> sensor_names,
                                              const std::string& sensor_name,
                                              get_sensor_fn_t get_sensor_fn)
{
    if (std::find(sensor_names.begin(), sensor_names.end(), sensor_name) ==
        sensor_names.end())
        return true;

    boost::system_time start = boost::get_system_time();
    boost::system_time first_lock_time;

    while (true) {
        if ((not first_lock_time.is_not_a_date_time()) and
            (boost::get_system_time() >
             (first_lock_time +
              boost::posix_time::seconds(static_cast<long>(LOCK_TIMEOUT))))) {
            break;
        }

        if (get_sensor_fn(sensor_name).to_bool()) {
            if (first_lock_time.is_not_a_date_time())
                first_lock_time = boost::get_system_time();
        } else {
            first_lock_time = boost::system_time(); // reset to 'not a date time'

            if (boost::get_system_time() >
                (start + boost::posix_time::seconds(static_cast<long>(LOCK_TIMEOUT)))) {
                return false;
            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    return true;
}

bool usrp_sink_impl::_unpack_chan_command(std::string& command,
                                           pmt::pmt_t& cmd_val,
                                           int& chan,
                                           const pmt::pmt_t& cmd_pmt)
{
    try {
        chan = -1; // Default value
        if (pmt::is_tuple(cmd_pmt) and
            (pmt::length(cmd_pmt) == 2 or pmt::length(cmd_pmt) == 3)) {
            command = pmt::symbol_to_string(pmt::tuple_ref(cmd_pmt, 0));
            cmd_val = pmt::tuple_ref(cmd_pmt, 1);
            if (pmt::length(cmd_pmt) == 3) {
                chan = pmt::to_long(pmt::tuple_ref(cmd_pmt, 2));
            }
        } else if (pmt::is_pair(cmd_pmt)) {
            command = pmt::symbol_to_string(pmt::car(cmd_pmt));
            cmd_val = pmt::cdr(cmd_pmt);
            if (pmt::is_pair(cmd_val)) {
                chan = pmt::to_long(pmt::car(cmd_val));
                cmd_val = pmt::cdr(cmd_val);
            }
        } else {
            return false;
        }
    } catch (pmt::wrong_type& w) {
        return false;
    }
    return true;
}

bool usrp_sink_impl::_check_mboard_sensors_locked()
{
    bool clocks_locked = true;

    // Check ref lock for all mboards
    for (size_t mboard_index = 0; mboard_index < _dev->get_num_mboards();
         mboard_index++) {
        std::string sensor_name = "ref_locked";
        if (_dev->get_clock_source(mboard_index) == "internal") {
            continue;
        } else if (_dev->get_clock_source(mboard_index) == "mimo") {
            sensor_name = "mimo_locked";
        }
        if (not _wait_for_locked_sensor(
                get_mboard_sensor_names(mboard_index),
                sensor_name,
                boost::bind(
                    &usrp_sink_impl::get_mboard_sensor, this, _1, mboard_index))) {
            GR_LOG_WARN(
                d_logger,
                boost::format(
                    "Sensor '%s' failed to lock within timeout on motherboard %d.") %
                    sensor_name % mboard_index);
            clocks_locked = false;
        }
    }

    return clocks_locked;
}

void usrp_sink_impl::_set_center_freq_from_internals_allchans(pmt::pmt_t direction)
{
    while (_chans_to_tune.any()) {
        // This resets() bits, so this loop should not run indefinitely
        _set_center_freq_from_internals(_chans_to_tune.find_first(), direction);
    }
}


/**********************************************************************
 * Public API calls
 *********************************************************************/
::uhd::sensor_value_t usrp_sink_impl::get_mboard_sensor(const std::string& name,
                                                         size_t mboard)
{
    return _dev->get_mboard_sensor(name, mboard);
}

std::vector<std::string> usrp_sink_impl::get_mboard_sensor_names(size_t mboard)
{
    return _dev->get_mboard_sensor_names(mboard);
}

void usrp_sink_impl::set_time_source(const std::string& source, const size_t mboard)
{
    return _dev->set_time_source(source, mboard);
}

std::string usrp_sink_impl::get_time_source(const size_t mboard)
{
    return _dev->get_time_source(mboard);
}

std::vector<std::string> usrp_sink_impl::get_time_sources(const size_t mboard)
{
    return _dev->get_time_sources(mboard);
}

void usrp_sink_impl::set_clock_source(const std::string& source, const size_t mboard)
{
    return _dev->set_clock_source(source, mboard);
}

std::string usrp_sink_impl::get_clock_source(const size_t mboard)
{
    return _dev->get_clock_source(mboard);
}

std::vector<std::string> usrp_sink_impl::get_clock_sources(const size_t mboard)
{
    return _dev->get_clock_sources(mboard);
}

double usrp_sink_impl::get_clock_rate(size_t mboard)
{
    return _dev->get_master_clock_rate(mboard);
}

void usrp_sink_impl::set_clock_rate(double rate, size_t mboard)
{
    return _dev->set_master_clock_rate(rate, mboard);
}

::uhd::time_spec_t usrp_sink_impl::get_time_now(size_t mboard)
{
    return _dev->get_time_now(mboard);
}

::uhd::time_spec_t usrp_sink_impl::get_time_last_pps(size_t mboard)
{
    return _dev->get_time_last_pps(mboard);
}

std::vector<std::string> usrp_sink_impl::get_gpio_banks(const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_GPIO_API
    return _dev->get_gpio_banks(mboard);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

boost::uint32_t usrp_sink_impl::get_gpio_attr(const std::string& bank,
                                               const std::string& attr,
                                               const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_GPIO_API
    return _dev->get_gpio_attr(bank, attr, mboard);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

void usrp_sink_impl::set_time_now(const ::uhd::time_spec_t& time_spec, size_t mboard)
{
    return _dev->set_time_now(time_spec, mboard);
}

void usrp_sink_impl::set_time_next_pps(const ::uhd::time_spec_t& time_spec)
{
    return _dev->set_time_next_pps(time_spec);
}

void usrp_sink_impl::set_time_unknown_pps(const ::uhd::time_spec_t& time_spec)
{
    return _dev->set_time_unknown_pps(time_spec);
}

void usrp_sink_impl::set_command_time(const ::uhd::time_spec_t& time_spec, size_t mboard)
{
    return _dev->set_command_time(time_spec, mboard);
}

void usrp_sink_impl::clear_command_time(size_t mboard)
{
    return _dev->clear_command_time(mboard);
}

void usrp_sink_impl::set_user_register(const uint8_t addr,
                                        const uint32_t data,
                                        size_t mboard)
{
    _dev->set_user_register(addr, data, mboard);
}

void usrp_sink_impl::set_gpio_attr(const std::string& bank,
                                    const std::string& attr,
                                    const boost::uint32_t value,
                                    const boost::uint32_t mask,
                                    const size_t mboard)
{
#ifdef UHD_USRP_MULTI_USRP_GPIO_API
    return _dev->set_gpio_attr(bank, attr, value, mask, mboard);
#else
    throw std::runtime_error("not implemented in this version");
#endif
}

::uhd::usrp::multi_usrp::sptr usrp_sink_impl::get_device(void) { return _dev; }

size_t usrp_sink_impl::get_num_mboards() { return _dev->get_num_mboards(); }

void usrp_sink_impl::msg_handler_command(pmt::pmt_t msg)
{
    // Legacy code back compat: If we receive a tuple, we convert
    // it to a dict, and call the function again. Yep, this comes
    // at a slight performance hit. Sometime in the future, we can
    // hopefully remove this:
    if (pmt::is_tuple(msg)) {
        if (pmt::length(msg) != 2 && pmt::length(msg) != 3) {
            GR_LOG_ALERT(d_logger,
                         boost::format("Error while unpacking command PMT: %s") % msg);
            return;
        }
        pmt::pmt_t new_msg = pmt::make_dict();
        new_msg = pmt::dict_add(new_msg, pmt::tuple_ref(msg, 0), pmt::tuple_ref(msg, 1));
        if (pmt::length(msg) == 3) {
            new_msg = pmt::dict_add(new_msg, gr::uhd::cmd_chan_key(), pmt::tuple_ref(msg, 2));
        }
        GR_LOG_WARN(d_debug_logger,
                    boost::format("Using legacy message format (tuples): %s") % msg);
        return msg_handler_command(new_msg);
    }
    // End of legacy backward compat code.

    // Turn pair into dict
    if (!pmt::is_dict(msg)) {
        GR_LOG_ERROR(d_logger,
                     boost::format("Command message is neither dict nor pair: %s") % msg);
        return;
    }

    // OK, here comes the horrible part. Pairs pass is_dict(), but they're not dicts. Such
    // dicks.
    try {
        // This will fail if msg is a pair:
        pmt::pmt_t keys = pmt::dict_keys(msg);
    } catch (const pmt::wrong_type& e) {
        // So we fix it:
        GR_LOG_DEBUG(d_debug_logger, boost::format("Converting pair to dict: %s") % msg);
        msg = pmt::dict_add(pmt::make_dict(), pmt::car(msg), pmt::cdr(msg));
    }

    /*** Start the actual message processing *************************/
    /// 1) Check if there's a time stamp
    if (pmt::dict_has_key(msg, gr::uhd::cmd_time_key())) {
        size_t mboard_index = pmt::to_long(pmt::dict_ref(
            msg,
            gr::uhd::cmd_mboard_key(),
            pmt::from_long(::uhd::usrp::multi_usrp::ALL_MBOARDS) // Default to all mboards
            ));
        pmt::pmt_t timespec_p = pmt::dict_ref(msg, gr::uhd::cmd_time_key(), pmt::PMT_NIL);
        if (timespec_p == pmt::PMT_NIL) {
            clear_command_time(mboard_index);
        } else {
            ::uhd::time_spec_t timespec(
                time_t(pmt::to_uint64(pmt::car(timespec_p))), // Full secs
                pmt::to_double(pmt::cdr(timespec_p))          // Frac secs
            );
            GR_LOG_DEBUG(d_debug_logger,
                         boost::format("Setting command time on mboard %d") %
                             mboard_index);
            set_command_time(timespec, mboard_index);
        }
    }

    /// 2) Read chan value
    int chan = int(pmt::to_long(pmt::dict_ref(msg,
                                              gr::uhd::cmd_chan_key(),
                                              pmt::from_long(-1) // Default to all chans
                                              )));

    /// 3) Loop through all the values
    GR_LOG_DEBUG(d_debug_logger, boost::format("Processing command message %s") % msg);
    pmt::pmt_t msg_items = pmt::dict_items(msg);
    for (size_t i = 0; i < pmt::length(msg_items); i++) {
        try {
            dispatch_msg_cmd_handler(pmt::car(pmt::nth(i, msg_items)),
                                     pmt::cdr(pmt::nth(i, msg_items)),
                                     chan,
                                     msg);
        } catch (pmt::wrong_type& e) {
            GR_LOG_ALERT(d_logger,
                         boost::format("Invalid command value for key %s: %s") %
                             pmt::car(pmt::nth(i, msg_items)) %
                             pmt::cdr(pmt::nth(i, msg_items)));
            break;
        }
    }

    /// 4) See if a direction was specified
    pmt::pmt_t direction =
        pmt::dict_ref(msg,
                      gr::uhd::cmd_direction_key(),
                      pmt::PMT_NIL // Anything except "TX" or "RX will default to the
                                   // messaged block direction"
        );

    /// 5) Check if we need to re-tune
    _set_center_freq_from_internals_allchans(direction);
}


void usrp_sink_impl::dispatch_msg_cmd_handler(const pmt::pmt_t& cmd,
                                               const pmt::pmt_t& val,
                                               int chan,
                                               pmt::pmt_t& msg)
{
    if (_msg_cmd_handlers.has_key(cmd)) {
        _msg_cmd_handlers[cmd](val, chan, msg);
    }
}

void usrp_sink_impl::register_msg_cmd_handler(const pmt::pmt_t& cmd,
                                               cmd_handler_t handler)
{
    _msg_cmd_handlers[cmd] = handler;
}

void usrp_sink_impl::_update_curr_tune_req(::uhd::tune_request_t& tune_req, int chan)
{
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            _update_curr_tune_req(tune_req, int(i));
        }
        return;
    }

    if (tune_req.target_freq != _curr_tune_req[chan].target_freq ||
        tune_req.rf_freq_policy != _curr_tune_req[chan].rf_freq_policy ||
        tune_req.rf_freq != _curr_tune_req[chan].rf_freq ||
        tune_req.dsp_freq != _curr_tune_req[chan].dsp_freq ||
        tune_req.dsp_freq_policy != _curr_tune_req[chan].dsp_freq_policy) {
        _curr_tune_req[chan] = tune_req;
        _chans_to_tune.set(chan);
    }
}

// Default handlers:
void usrp_sink_impl::_cmd_handler_freq(const pmt::pmt_t& freq_,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    double freq = pmt::to_double(freq_);
    ::uhd::tune_request_t new_tune_reqest(freq);
    if (pmt::dict_has_key(msg, gr::uhd::cmd_lo_offset_key())) {
        double lo_offset =
            pmt::to_double(pmt::dict_ref(msg, gr::uhd::cmd_lo_offset_key(), pmt::PMT_NIL));
        new_tune_reqest = ::uhd::tune_request_t(freq, lo_offset);
    }

    _update_curr_tune_req(new_tune_reqest, chan);
}

void usrp_sink_impl::_cmd_handler_looffset(const pmt::pmt_t& lo_offset,
                                            int chan,
                                            const pmt::pmt_t& msg)
{
    if (pmt::dict_has_key(msg, gr::uhd::cmd_freq_key())) {
        // Then it's already taken care of
        return;
    }

    double lo_offs = pmt::to_double(lo_offset);
    ::uhd::tune_request_t new_tune_request = _curr_tune_req[chan];
    new_tune_request.rf_freq = new_tune_request.target_freq + lo_offs;
    new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
    new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_AUTO;

    _update_curr_tune_req(new_tune_request, chan);
}

void usrp_sink_impl::_cmd_handler_gain(const pmt::pmt_t& gain_,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    double gain = pmt::to_double(gain_);
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            set_gain(gain, i);
        }
        return;
    }

    set_gain(gain, chan);
}

void usrp_sink_impl::_cmd_handler_antenna(const pmt::pmt_t& ant,
                                           int chan,
                                           const pmt::pmt_t& msg)
{
    const std::string antenna(pmt::symbol_to_string(ant));
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            set_antenna(antenna, i);
        }
        return;
    }

    set_antenna(antenna, chan);
}

void usrp_sink_impl::_cmd_handler_rate(const pmt::pmt_t& rate_, int, const pmt::pmt_t&)
{
    const double rate = pmt::to_double(rate_);
    set_samp_rate(rate);
}

void usrp_sink_impl::_cmd_handler_tune(const pmt::pmt_t& tune,
                                        int chan,
                                        const pmt::pmt_t& msg)
{
    double freq = pmt::to_double(pmt::car(tune));
    double lo_offset = pmt::to_double(pmt::cdr(tune));
    ::uhd::tune_request_t new_tune_reqest(freq, lo_offset);
    _update_curr_tune_req(new_tune_reqest, chan);
}

void usrp_sink_impl::_cmd_handler_bw(const pmt::pmt_t& bw,
                                      int chan,
                                      const pmt::pmt_t& msg)
{
    double bandwidth = pmt::to_double(bw);
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            set_bandwidth(bandwidth, i);
        }
        return;
    }

    set_bandwidth(bandwidth, chan);
}

void usrp_sink_impl::_cmd_handler_lofreq(const pmt::pmt_t& lofreq,
                                          int chan,
                                          const pmt::pmt_t& msg)
{
    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            _cmd_handler_lofreq(lofreq, int(i), msg);
        }
        return;
    }

    ::uhd::tune_request_t new_tune_request = _curr_tune_req[chan];
    new_tune_request.rf_freq = pmt::to_double(lofreq);
    if (pmt::dict_has_key(msg, gr::uhd::cmd_dsp_freq_key())) {
        new_tune_request.dsp_freq =
            pmt::to_double(pmt::dict_ref(msg, gr::uhd::cmd_dsp_freq_key(), pmt::PMT_NIL));
    }
    new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
    new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;

    _update_curr_tune_req(new_tune_request, chan);
}

void usrp_sink_impl::_cmd_handler_dspfreq(const pmt::pmt_t& dspfreq,
                                           int chan,
                                           const pmt::pmt_t& msg)
{
    if (pmt::dict_has_key(msg, gr::uhd::cmd_lo_freq_key())) {
        // Then it's already dealt with
        return;
    }

    if (chan == -1) {
        for (size_t i = 0; i < _nchan; i++) {
            _cmd_handler_dspfreq(dspfreq, int(i), msg);
        }
        return;
    }

    ::uhd::tune_request_t new_tune_request = _curr_tune_req[chan];
    new_tune_request.dsp_freq = pmt::to_double(dspfreq);
    new_tune_request.rf_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;
    new_tune_request.dsp_freq_policy = ::uhd::tune_request_t::POLICY_MANUAL;

    _update_curr_tune_req(new_tune_request, chan);
}


} /* namespace uhd */
} /* namespace gr */
