#!/usr/bin/python
# vim: ai:ts=4:sw=4:sts=4:et:fileencoding=utf-8
#
# BMA180 accelerometer control class
#
# Copyright 2014 Baptiste PIERARD
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.	If not, see http://www.gnu.org/licenses/.
#

import smbus

def int(msb, lsb):
	"""Interpret integer as signed word with bytes swapped"""
	xx = (msb << 8) + lsb
	return xx - 0xffff if xx > 0x7fff else xx

class SensorBMA180(object):
	"""BMA180 digital accelerometer control class.
	Supports data polling at the moment.
	"""
	REG_RESET = 0x10 
	REG_PWR	  = 0x0D
	REG_BW	  = 0X20
	REG_RANGE = 0X35
	REG_DATA  = 0x02

	def __init__(self, bus_nr, addr):
		""" Sensor class constructor
		Params:
			bus_nr .. I2C bus number
			addr   .. BMA180 device address
		"""
		self.bus = smbus.SMBus(bus_nr)
		self.addr = addr

	def default_init(self):
		"""Initialization with default values:
		8kHz internal sample rate, 256Hz low pass filter, sample rate divider 8, scale +/- 2g.
		"""
		#reset the chip
		self.bus.write_byte_data(self.addr, SensorBMA180.REG_RESET, 0xB6)
		
		#wake up mode
		self.bus.write_byte_data(self.addr, SensorBMA180.REG_PWR,0x10);
		
		# low pass filter,
		temp=self.bus.read_byte_data(self.addr, SensorBMA180.REG_BW)
		temp=temp & 0x0F;
		self.bus.write_byte_data(self.addr, SensorBMA180.REG_BW, temp);	  
		
		# range +/- 2g 
		temp=self.bus.read_byte_data(self.addr, SensorBMA180.REG_RANGE)
		temp = (temp & 0xF1) | 0x04;
		self.bus.write_byte_data(self.addr, SensorBMA180.REG_RANGE,temp);

	def read_data(self):
		"""Read and return data tuple for x, y and z axis as signed float in 'g' unit
		"""
		
		offx = 0
		offy = 0
		offz = 0

		# read in the 3 axis data, each one is 14 bits (2 bytes) --> 6 bytes
		raw = self.bus.read_i2c_block_data(self.addr, SensorBMA180.REG_DATA, 6)
		
		x  = ( int(raw[1],raw[0] ) >> 2 ) + offx
		ax = float(x) / 4096.0

		y = ( int(raw[3],raw[2] ) >> 2 ) + offy
		ay = float(y) / 4096.0

		z = ( int(raw[5],raw[4] ) >> 2 ) + offz
		az = float(z) / 4096.0
		
		return (ax, ay, az)

if __name__ == '__main__':
	import time
	sensor = SensorBMA180(1, 0x40) # update with your bus number and address
	sensor.default_init()
	time.sleep(0.1)
	
	for i in range(0,1000) :
		ax, ay, az = sensor.read_data()
		print "{:+10.4f} {:+10.4f} {:+10.4f}".format(ax, ay, az)  # angle en unité 'g'

