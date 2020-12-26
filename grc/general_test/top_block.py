#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Tue Dec 15 13:21:28 2020
##################################################


if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import uhd
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import waterfallsink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import pmt
import spacegrant
import time
import wx


class top_block(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.sat_freq = sat_freq = int(401.365e6)
        self.doppler_freq = doppler_freq = sat_freq
        self.variable_text_box_0 = variable_text_box_0 = doppler_freq-sat_freq
        self.tx_gain = tx_gain = 86
        self.samp_rate = samp_rate = 460800
        self.rx_gain = rx_gain = 40
        self.lo_offset = lo_offset = 150e3
        self.freq_xlation = freq_xlation = 120000
        self.freq_corr = freq_corr = 0
        self.baud_rate = baud_rate = 9600

        ##################################################
        # Blocks
        ##################################################
        _freq_corr_sizer = wx.BoxSizer(wx.VERTICAL)
        self._freq_corr_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_freq_corr_sizer,
        	value=self.freq_corr,
        	callback=self.set_freq_corr,
        	label='Frequency Correction',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._freq_corr_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_freq_corr_sizer,
        	value=self.freq_corr,
        	callback=self.set_freq_corr,
        	minimum=-10000,
        	maximum=10000,
        	num_steps=500,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.GridAdd(_freq_corr_sizer, 0, 2, 1, 1)
        self._doppler_freq_text_box = forms.text_box(
        	parent=self.GetWin(),
        	value=self.doppler_freq,
        	callback=self.set_doppler_freq,
        	label='Doppler Frequency',
        	converter=forms.float_converter(),
        )
        self.GridAdd(self._doppler_freq_text_box, 0, 0, 1, 1)
        self._baud_rate_chooser = forms.drop_down(
        	parent=self.GetWin(),
        	value=self.baud_rate,
        	callback=self.set_baud_rate,
        	label='Baud Rate',
        	choices=[115200, 76800, 38400,19200,9600,4800],
        	labels=["115K2","76K8", "38K4", "19K2","9K6","4K8"],
        )
        self.Add(self._baud_rate_chooser)
        self.wxgui_waterfallsink2_0_1_1 = waterfallsink2.waterfall_sink_c(
        	self.GetWin(),
        	baseband_freq=sat_freq+lo_offset,
        	dynamic_range=50,
        	ref_level=-50,
        	ref_scale=2.0,
        	sample_rate=460800,
        	fft_size=512,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title='Waterfall Plot',
        )
        self.GridAdd(self.wxgui_waterfallsink2_0_1_1.win, 1, 0, 1, 4)
        self.wxgui_waterfallsink2_0_0_0 = waterfallsink2.waterfall_sink_c(
        	self.GetWin(),
        	baseband_freq=doppler_freq+freq_corr,
        	dynamic_range=50,
        	ref_level=-70,
        	ref_scale=2.0,
        	sample_rate=baud_rate*2,
        	fft_size=512,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title='Waterfall Plot',
        )
        self.GridAdd(self.wxgui_waterfallsink2_0_0_0.win, 3, 0, 1, 2)
        self.wxgui_fftsink2_0_0 = fftsink2.fft_sink_c(
        	self.GetWin(),
        	baseband_freq=doppler_freq+freq_corr,
        	y_per_div=10,
        	y_divs=10,
        	ref_level=-50,
        	ref_scale=2.0,
        	sample_rate=baud_rate*2,
        	fft_size=1024,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title='FFT Plot',
        	peak_hold=False,
        )
        self.GridAdd(self.wxgui_fftsink2_0_0.win, 3, 2, 1, 2)
        self._variable_text_box_0_text_box = forms.text_box(
        	parent=self.GetWin(),
        	value=self.variable_text_box_0,
        	callback=self.set_variable_text_box_0,
        	label='Doppler Shift',
        	converter=forms.float_converter(),
        )
        self.GridAdd(self._variable_text_box_0_text_box, 0, 1, 1, 1)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "master_clock_rate=46080000")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(sat_freq+lo_offset, 0)
        self.uhd_usrp_source_0.set_gain(rx_gain, 0)
        self.uhd_usrp_source_0.set_antenna('RX2', 0)
        self.spacegrant_spirit1_basic_framer_0 = spacegrant.spirit1_basic_framer(pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(4, [0xAA, 0xAA, 0xAA, 0xAA])), pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(4, [0x88, 0x88, 0x88, 0x88])), 8, 0, 0, 0)
        self.spacegrant_spirit1_basic_deframer_0 = spacegrant.spirit1_basic_deframer(pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(4, [0xAA, 0xAA, 0xAA, 0xAA])), pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(4, [0x88, 0x88, 0x88, 0x88])), 8, 0, 0, 0)
        self.spacegrant_sg_uhd_usrp_sink_0 = spacegrant.sg_uhd_usrp_sink(
        	",".join(("", "")),
         	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        	''
        )
        self.spacegrant_sg_uhd_usrp_sink_0.set_center_freq(sat_freq+lo_offset, 0)
        self.spacegrant_sg_uhd_usrp_sink_0.set_samp_rate(samp_rate)
        self.spacegrant_sg_uhd_usrp_sink_0.set_gain(tx_gain, 0)

        self.spacegrant_message_debug_0 = spacegrant.message_debug()
        self.interp_fir_filter_xxx_0 = filter.interp_fir_filter_ccc(samp_rate/(baud_rate*2), (firdes.low_pass(1,samp_rate,baud_rate,5000)))
        self.interp_fir_filter_xxx_0.declare_sample_delay(0)
        self.freq_xlating_fir_filter_xxx_0_0 = filter.freq_xlating_fir_filter_ccc(1, ([1]), freq_corr+lo_offset, samp_rate)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(samp_rate/(baud_rate*2), (firdes.low_pass(1,samp_rate,baud_rate/2,baud_rate/10)), -freq_corr-lo_offset, samp_rate)
        self.digital_gmsk_mod_0 = digital.gmsk_mod(
        	samples_per_symbol=4,
        	bt=0.5,
        	verbose=True,
        	log=True,
        )
        self.digital_gmsk_demod_0 = digital.gmsk_demod(
        	samples_per_symbol=4,
        	gain_mu=0.175,
        	mu=0.5,
        	omega_relative_limit=0.005,
        	freq_error=0.0,
        	verbose=False,
        	log=False,
        )
        self.blocks_unpacked_to_packed_xx_1 = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        self.blocks_tuntap_pdu_0 = blocks.tuntap_pdu('tun0', 256, True)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_tuntap_pdu_0, 'pdus'), (self.spacegrant_spirit1_basic_framer_0, 'in'))
        self.msg_connect((self.spacegrant_spirit1_basic_deframer_0, 'out'), (self.blocks_tuntap_pdu_0, 'pdus'))
        self.msg_connect((self.spacegrant_spirit1_basic_deframer_0, 'out'), (self.spacegrant_message_debug_0, 'pdu'))
        self.connect((self.blocks_unpacked_to_packed_xx_1, 0), (self.digital_gmsk_mod_0, 0))
        self.connect((self.digital_gmsk_demod_0, 0), (self.spacegrant_spirit1_basic_deframer_0, 0))
        self.connect((self.digital_gmsk_mod_0, 0), (self.interp_fir_filter_xxx_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.digital_gmsk_demod_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.wxgui_fftsink2_0_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.wxgui_waterfallsink2_0_0_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0_0, 0), (self.spacegrant_sg_uhd_usrp_sink_0, 0))
        self.connect((self.interp_fir_filter_xxx_0, 0), (self.freq_xlating_fir_filter_xxx_0_0, 0))
        self.connect((self.spacegrant_spirit1_basic_framer_0, 0), (self.blocks_unpacked_to_packed_xx_1, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.wxgui_waterfallsink2_0_1_1, 0))

    def get_sat_freq(self):
        return self.sat_freq

    def set_sat_freq(self, sat_freq):
        self.sat_freq = sat_freq
        self.set_doppler_freq(self.sat_freq)
        self.wxgui_waterfallsink2_0_1_1.set_baseband_freq(self.sat_freq+self.lo_offset)
        self.set_variable_text_box_0(self.doppler_freq-self.sat_freq)
        self.uhd_usrp_source_0.set_center_freq(self.sat_freq+self.lo_offset, 0)
        self.spacegrant_sg_uhd_usrp_sink_0.set_center_freq(self.sat_freq+self.lo_offset, 0)

    def get_doppler_freq(self):
        return self.doppler_freq

    def set_doppler_freq(self, doppler_freq):
        self.doppler_freq = doppler_freq
        self._doppler_freq_text_box.set_value(self.doppler_freq)
        self.wxgui_waterfallsink2_0_0_0.set_baseband_freq(self.doppler_freq+self.freq_corr)
        self.wxgui_fftsink2_0_0.set_baseband_freq(self.doppler_freq+self.freq_corr)
        self.set_variable_text_box_0(self.doppler_freq-self.sat_freq)

    def get_variable_text_box_0(self):
        return self.variable_text_box_0

    def set_variable_text_box_0(self, variable_text_box_0):
        self.variable_text_box_0 = variable_text_box_0
        self._variable_text_box_0_text_box.set_value(self.variable_text_box_0)

    def get_tx_gain(self):
        return self.tx_gain

    def set_tx_gain(self, tx_gain):
        self.tx_gain = tx_gain

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.spacegrant_sg_uhd_usrp_sink_0.set_samp_rate(self.samp_rate)
        self.interp_fir_filter_xxx_0.set_taps((firdes.low_pass(1,self.samp_rate,self.baud_rate,5000)))
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1,self.samp_rate,self.baud_rate/2,self.baud_rate/10)))

    def get_rx_gain(self):
        return self.rx_gain

    def set_rx_gain(self, rx_gain):
        self.rx_gain = rx_gain
        self.uhd_usrp_source_0.set_gain(self.rx_gain, 0)


    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset
        self.wxgui_waterfallsink2_0_1_1.set_baseband_freq(self.sat_freq+self.lo_offset)
        self.uhd_usrp_source_0.set_center_freq(self.sat_freq+self.lo_offset, 0)
        self.spacegrant_sg_uhd_usrp_sink_0.set_center_freq(self.sat_freq+self.lo_offset, 0)
        self.freq_xlating_fir_filter_xxx_0_0.set_center_freq(self.freq_corr+self.lo_offset)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(-self.freq_corr-self.lo_offset)

    def get_freq_xlation(self):
        return self.freq_xlation

    def set_freq_xlation(self, freq_xlation):
        self.freq_xlation = freq_xlation

    def get_freq_corr(self):
        return self.freq_corr

    def set_freq_corr(self, freq_corr):
        self.freq_corr = freq_corr
        self._freq_corr_slider.set_value(self.freq_corr)
        self._freq_corr_text_box.set_value(self.freq_corr)
        self.wxgui_waterfallsink2_0_0_0.set_baseband_freq(self.doppler_freq+self.freq_corr)
        self.wxgui_fftsink2_0_0.set_baseband_freq(self.doppler_freq+self.freq_corr)
        self.freq_xlating_fir_filter_xxx_0_0.set_center_freq(self.freq_corr+self.lo_offset)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(-self.freq_corr-self.lo_offset)

    def get_baud_rate(self):
        return self.baud_rate

    def set_baud_rate(self, baud_rate):
        self.baud_rate = baud_rate
        self._baud_rate_chooser.set_value(self.baud_rate)
        self.wxgui_waterfallsink2_0_0_0.set_sample_rate(self.baud_rate*2)
        self.wxgui_fftsink2_0_0.set_sample_rate(self.baud_rate*2)
        self.interp_fir_filter_xxx_0.set_taps((firdes.low_pass(1,self.samp_rate,self.baud_rate,5000)))
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1,self.samp_rate,self.baud_rate/2,self.baud_rate/10)))


def main(top_block_cls=top_block, options=None):

    tb = top_block_cls()
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
