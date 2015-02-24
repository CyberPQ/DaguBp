#! /usr/bin/python
# -*- coding: utf-8 -*-

import smbus
import time
	
class DaguRover(object):

	MotorSpeedSet   = 0x82
	PWMFrequenceSet = 0x84
	DirectionSet    = 0xaa
	MotorSetA       = 0xa1
	MotorSetB       = 0xa5
	Nothing         = 0x01
	
	STOP    = 0
	FORWARD = 1
	REVERSE = 2
	
	def __init__(self, bus_nr, addr):
		""" Sensor class constructor
		Params:
			bus_nr .. I2C bus number
			addr   .. BMA180 device address
		"""
		##self.bus = smbus.SMBus(bus_nr)
		##self.addr = addr

	def set_speed(self, speed_a, speed_b):
		#self.bus.write_word_data(self.addr, DaguRover.MotorSpeedSet, (speed_b<<8)+speed_a)
		print "set_speed"

	def set_dir(self, direction):
		#self.bus.write_word_data(self.addr, DaguRover.DirectionSet, (direction<<8)+direction)
		print "set_dir"
		
	def set_speed_a(self, direction, speed):
		#self.bus.write_word_data(self.addr, DaguRover.MotorSetA, (speed<<8)+direction)
		print "set_speed_a"

	def set_speed_b(self, direction, speed):
		#self.bus.write_word_data(self.addr, DaguRover.MotorSetB, (speed<<8)+direction)
		print "set_speed_b"

if __name__ == '__main__':

	rover = DaguRover(0, 0x0f) # update with your bus number and address
	time.sleep(0.1)

	rover.set_speed_a(DaguRover.FORWARD, 255)
	rover.set_speed_b(DaguRover.FORWARD, 255)
	time.sleep(2)
	rover.set_speed_a(DaguRover.REVERSE, 127)
	rover.set_speed_b(DaguRover.REVERSE, 127)
	time.sleep(2)
	rover.set_speed_a(DaguRover.FORWARD, 50)
	rover.set_speed_b(DaguRover.FORWARD, 50)
	time.sleep(2)
	rover.set_speed_a(DaguRover.STOP, 0)
	rover.set_speed_b(DaguRover.STOP, 0)
