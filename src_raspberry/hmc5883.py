#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :

# HMC5888L Magnetometer (Digital Compass) wrapper class
# Based on https://bitbucket.org/thinkbowl/i2clibraries/src/14683feb0f96,
# but uses smbus rather than quick2wire and sets some different init
# params.

import smbus
import math
import time
import sys

class hmc5883l:

    __scales = {
        0.88: [0, 0.73],
        1.30: [1, 0.92],
        1.90: [2, 1.22],
        2.50: [3, 1.52],
        4.00: [4, 2.27],
        4.70: [5, 2.56],
        5.60: [6, 3.03],
        8.10: [7, 4.35],
    }

    def __init__(self, port=1, address=0x1E, gauss=1.3, declination=(0,0)):
        self.bus = smbus.SMBus(port)
        self.address = address

        (degrees, minutes) = declination
        self.__declDegrees = degrees
        self.__declMinutes = minutes
        self.__declination = (degrees + minutes / 60) * math.pi / 180

        (reg, self.__scale) = self.__scales[gauss]
        self.bus.write_byte_data(self.address, 0x00, 0x70) # 8 Average, 15 Hz, normal measurement
        self.bus.write_byte_data(self.address, 0x01, reg << 5) # Scale
        self.bus.write_byte_data(self.address, 0x02, 0x00) # Continuous measurement

    def declination(self):
        return (self.__declDegrees, self.__declMinutes)

    def twos_complement(self, val, len):
        # Convert twos compliment to integer
        if (val & (1 << len - 1)):
            val = val - (1<<len)
        return val

    def __convert(self, data, offset):
        val = self.twos_complement(data[offset] << 8 | data[offset+1], 16)
        if val == -4096: return None
        return round(val * self.__scale, 4)

    def axes(self):
        data = self.bus.read_i2c_block_data(self.address, 0x00)
        #print map(hex, data)
        x = self.__convert(data, 3)
        y = self.__convert(data, 7)
        z = self.__convert(data, 5)
        return (x,y,z)

    def heading(self):
        (x, y, z) = self.axes()
        headingRad = math.atan2(y, x)
        headingRad += self.__declination

        # Correct for reversed heading
        if headingRad < 0:
            headingRad += 2 * math.pi

        # Check for wrap and compensate
        elif headingRad > 2 * math.pi:
            headingRad -= 2 * math.pi

        # Convert to degrees from radians
        headingDeg = headingRad * 180 / math.pi
        return headingDeg

    def degrees(self, headingDeg):
        degrees = math.floor(headingDeg)
        minutes = round((headingDeg - degrees) * 60)
        return (degrees, minutes)

    def __str__(self):
        (x, y, z) = self.axes()
        return "Axis X: " + str(x) + "\n" \
               "Axis Y: " + str(y) + "\n" \
               "Axis Z: " + str(z) + "\n" #\
               #"Declination: " + str(self.degrees(self.declination())) + "\n" \
               #"Heading: " + str(self.degrees(self.heading())) + "\n"

if __name__ == "__main__":
    # http://magnetic-declination.com/Great%20Britain%20(UK)/Harrogate#
    min_x = 0.0
    max_x = 0.0
    min_y = 0.0
    max_y = 0.0
    min_z = 0.0
    max_z = 0.0
    compass = hmc5883l(gauss = 4.7, declination = (-1,20))
    while True:

        #sys.stdout.write("\rHeading: " + str(compass.degrees(compass.heading())) + "     ")
        #sys.stdout.write("\rHeading: " + str(compass.heading()) + "     ")
        #print "{:+.2f}\t{:+.2f}\t{:+.2f}".format(compass.axes()[0], compass.axes()[1], compass.axes()[2])

        if(compass.axes()[0]>max_x):
            max_x = compass.axes()[0]

        if(compass.axes()[0]<min_x):
            min_x = compass.axes()[0]
            
        sys.stdout.write("x : [ " + str(min_x)+" , "+str(max_x)+" ]    ")

        if(compass.axes()[1]>max_y):
            max_y = compass.axes()[1]

        if(compass.axes()[1]<min_y):
            min_y = compass.axes()[1]
            
        sys.stdout.write("y : [ " + str(min_y)+" , "+str(max_y)+" ]    ")

        if(compass.axes()[2]>max_z):
            max_z = compass.axes()[2]

        if(compass.axes()[2]<min_z):
            min_z = compass.axes()[2]
            
        sys.stdout.write("z : [ " + str(min_z)+" , "+str(max_z)+" ]                                               \r")

        sys.stdout.flush()
        #time.sleep(0.5)