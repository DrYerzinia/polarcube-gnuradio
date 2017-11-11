#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import numpy
from gnuradio import gr

import threading
import time
import math
import ephem
from datetime import datetime

class doppler_corrector(threading.Thread):
  def __init__(self, callback, sat_freq, tle1, tle2, longitude, latitude, elevation):
    threading.Thread.__init__(self)

    self.callback = callback
    self.sat_freq = sat_freq
    self.tle1 = tle1
    self.tle2 = tle2
    self.longitude = longitude
    self.latitude = latitude
    self.elevation = elevation

  def run(self):

    sat = ephem.readtle("sat", self.tle1, self.tle2)

    ob = ephem.Observer()
    ob.lon = self.longitude
    ob.lat = self.latitude
    ob.elevation = self.elevation

    c = 299792458.0
    f0 = self.sat_freq

    while(True):

      ob.date = ephem.now()
      sat.compute(ob)
      d_frequency = (c/(c+sat.range_velocity))*f0	

      print 'Alt: ',  sat.alt, '/Az: ',  sat.az

      freq_rounded = math.floor(d_frequency)
      self.callback(freq_rounded)

      time.sleep(0.5)


class sat_doppler_correction(gr.sync_block):
    """
    docstring for block sat_doppler_correction
    """
    def __init__(self, callback, sat_freq, tle1, tle2, longitude, latitude, elevation):
        gr.sync_block.__init__(self,
            name="sat_doppler_correction",
            in_sig=None,
            out_sig=None)
        self.dc = doppler_corrector(callback, sat_freq, tle1, tle2, longitude, latitude, elevation)
        self.dc.start()

