# Copyright (c) 2014 Adafruit Industries
# Author: Tony DiCola
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
from __future__ import division
import logging
import time

import Adafruit_GPIO as GPIO
import Adafruit_GPIO.SPI as SPI


# Constants
SSD1332_I2C_ADDRESS = 0x3C    # 011110+SA0+RW - 0x3C or 0x3D
#SSD1332_SETCONTRAST = 0x81
SSD1332_DISPLAYALLON_RESUME = 0xA4
#SSD1332_DISPLAYALLON = 0xA5
#SSD1332_NORMALDISPLAY = 0xA6
#SSD1332_INVERTDISPLAY = 0xA7
#SSD1332_DISPLAYOFF = 0xAE
#SSD1332_DISPLAYON = 0xAF
#SSD1332_SETDISPLAYOFFSET = 0xD3
#SSD1332_SETCOMPINS = 0xDA
#SSD1332_SETVCOMDETECT = 0xDB
#SSD1332_SETDISPLAYCLOCKDIV = 0xB3
#SSD1332_SETPRECHARGE = 0xD9
#SSD1332_SETMULTIPLEX = 0xA8
#SSD1332_SETLOWCOLUMN = 0x00
#SSD1332_SETHIGHCOLUMN = 0x10
#SSD1332_SETSTARTLINE = 0x40
#SSD1332_MEMORYMODE = 0x20
SSD1332_COLUMNADDR = 0x15
SSD1332_PAGEADDR = 0x75
#SSD1332_COMSCANINC = 0xC0
#SSD1332_COMSCANDEC = 0xC8
#SSD1332_SEGREMAP = 0xA0
#SSD1332_CHARGEPUMP = 0x8D
#SSD1332_EXTERNALVCC = 0x1
#SSD1332_SWITCHCAPVCC = 0x2

# Scrolling constants
#SSD1332_ACTIVATE_SCROLL = 0x2F
#SSD1332_DEACTIVATE_SCROLL = 0x2E
#SSD1332_SET_VERTICAL_SCROLL_AREA = 0xA3
#SSD1332_RIGHT_HORIZONTAL_SCROLL = 0x26
#SSD1332_LEFT_HORIZONTAL_SCROLL = 0x27
#SSD1332_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29
#SSD1332_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A


class SSD1332Base(object):
    """Base class for SSD1332-based OLED displays.  Implementors should subclass
    and provide an implementation for the _initialize function.
    """

    def __init__(self, width, height, rst, dc=None, sclk=None, din=None, cs=None,
                 gpio=None, spi=None, i2c_bus=None, i2c_address=SSD1332_I2C_ADDRESS,
                 i2c=None):
        self._log = logging.getLogger('Adafruit_SSD1332.SSD1332Base')
        self._spi = None
        self._i2c = None
        self.width = width
        self.height = height
        self._pages = height#8
        self._buffer = [0]*(width*self._pages)
        # Default to platform GPIO if not provided.
        self._gpio = gpio
        if self._gpio is None:
            self._gpio = GPIO.get_platform_gpio()
        # Setup reset pin.
        self._rst = rst
        self._gpio.setup(self._rst, GPIO.OUT)
        # Handle hardware SPI
        if spi is not None:
            self._log.debug('Using hardware SPI')
            self._spi = spi
            self._spi.set_clock_hz(8000000)
        # Handle software SPI
        elif sclk is not None and din is not None and cs is not None:
            self._log.debug('Using software SPI')
            self._spi = SPI.BitBang(self._gpio, sclk, din, None, cs)
        # Handle hardware I2C
        elif i2c is not None:
            self._log.debug('Using hardware I2C with custom I2C provider.')
            self._i2c = i2c.get_i2c_device(i2c_address)
        else:
            self._log.debug('Using hardware I2C with platform I2C provider.')
            import Adafruit_GPIO.I2C as I2C
            if i2c_bus is None:
                self._i2c = I2C.get_i2c_device(i2c_address)
            else:
                self._i2c = I2C.get_i2c_device(i2c_address, busnum=i2c_bus)
        # Initialize DC pin if using SPI.
        if self._spi is not None:
            if dc is None:
                raise ValueError('DC pin must be provided when using SPI.')
            self._dc = dc
            self._gpio.setup(self._dc, GPIO.OUT)

    def _initialize(self):
        raise NotImplementedError

    def command(self, c):
        """Send command byte to display."""
        if self._spi is not None:
            # SPI write.
            self._gpio.set_low(self._dc)
            self._spi.write([c])
        else:
            # I2C write.
            control = 0x00   # Co = 0, DC = 0
            self._i2c.write8(control, c)

    def data(self, c):
        """Send byte of data to display."""
        if self._spi is not None:
            # SPI write.
            self._gpio.set_high(self._dc)
            self._spi.write([c])
        else:
            # I2C write.
            control = 0x40   # Co = 0, DC = 0
            self._i2c.write8(control, c)

    def begin(self):
        """Initialize display."""
        # Save vcc state.
        # self._vccstate = vccstate
        # Reset and initialize display.
        self.reset()
        self._initialize()
        # Turn on the display.
        self.command(SSD1332_DISPLAYON)

    def reset(self):
        """Reset the display."""
        # Set reset high for a millisecond.
        self._gpio.set_high(self._rst)
        time.sleep(0.001)
        # Set reset low for 10 milliseconds.
        self._gpio.set_low(self._rst)
        time.sleep(0.010)
        # Set reset high again.
        self._gpio.set_high(self._rst)

    def display(self):
        """Write display buffer to physical display."""
        self.command(SSD1332_COLUMNADDR)
        self.command(0x1C+ 0)              # Column start address. (0 = reset)
        self.command(0x1C + 0x3F)   # Column end address.
        self.command(SSD1332_PAGEADDR)
        self.command(0)              # Page start address. (0 = reset)
        self.command(self._pages-1)  # Page end address.
        # Write buffer data.
        if self._spi is not None:
            # Set DC high for data.
            self._gpio.set_high(self._dc)
            # Write buffer.
            self._spi.write(self._buffer)
        else:
            for i in range(0, len(self._buffer), 16):
                control = 0x40   # Co = 0, DC = 0
                self._i2c.writeList(control, self._buffer[i:i+16])

    def image(self, image):
        """Set buffer to value of Python Imaging Library image.  The image should
        be in 1 bit mode and a size equal to the display size.
        """
        if image.mode != '1':
            raise ValueError('Image must be in mode 1.')
        imwidth, imheight = image.size
        if imwidth != self.width or imheight != self.height:
            raise ValueError('Image must be same dimensions as display ({0}x{1}).' \
                .format(self.width, self.height))
        # Grab all the pixels from the image, faster than getpixel.
        pix = image.load()
        # Iterate through the memory pages
        index = 0
        for page in range(self._pages):
            # Iterate through all x axis columns.
            for x in range(self.width):
                # Set the bits for the column of pixels at the current position.
                bits = 0
                # Don't use range here as it's a bit slow
                for bit in [0, 1, 2, 3, 4, 5, 6, 7]:
                    bits = bits << 1
                    bits |= 0 if pix[(x, page*8+7-bit)] == 0 else 1
                # Update buffer byte and increment to next byte.
                self._buffer[index] = bits
                index += 1

    def clear(self):
        """Clear contents of image buffer."""
        self._buffer = [0]*(self.width*self._pages)


class SSD1332_256_64(SSD1332Base):
    def __init__(self, rst, dc=None, sclk=None, din=None, cs=None, gpio=None,
                 spi=None, i2c_bus=None, i2c_address=SSD1332_I2C_ADDRESS,
                 i2c=None):
        # Call base class constructor.
        super(SSD1332_256_64, self).__init__(256, 64, rst, dc, sclk, din, cs,
                                             gpio, spi, i2c_bus, i2c_address, i2c)

    def _initialize(self):
           
        self.command(0xAE);     # Display Off
    
        self.command(0xFD);     # Set Command Lock
        self.data(0x12);        #
        
        self.command(0xB3);     # Set Clock as 80 Frames/Sec
        self.data(0x91);        #  

        self.command(0xCA);     # Set Multiplex Ratio
        self.data(0x3F);        # 1/64 Duty (0x0F~0x5F)
       
        self.command(0xA2);     # Shift Mapping RAM Counter (0x00~0x3F)
        self.data(0x00);        #    
        
        self.command(0xA1);     # Set Mapping RAM Display Start Line (0x00~0x7F)
        self.data(0x00);        #    

        
        self.command(0xA0);     #Set Column Address 0 Mapped to SEG0 
        self.data(0x14);        #   Default => 0x40
                                #     Horizontal Address Increment
                                #     Column Address 0 Mapped to SEG0
                                #     Disable Nibble Remap
                                #     Scan from COM0 to COM[N-1]
                                #     Disable COM Split Odd Even
        self.data(0x11);        #    Default => 0x01 (Disable Dual COM Mode)

        
        self.command(0xB5);     #  Disable GPIO Pins Input
        self.data(0x00);        #    
        
        self.command(0xAB);     #   Enable Internal VDD Regulator
        self.data(0x01);        #

        self.command(0xB4);     #  Display Enhancement  
        self.data(0xA0);        # Enable External VSL
        self.data(0xF8);        # Enhance Low Gray Scale Display Quality

        self.command(0xC1);     #  Set Contrast Current 
        self.data(0xEF);        #  Default => 0x7F

        self.command(0xC7);     #  Master Contrast Current Control 
        self.data(0xFF);  #  Default => 0x0f (Maximum)

        self.command(0xB8);     #  Set Gray Scale Table 
        self.data(0x0C); 
        self.data(0x18); 
        self.data(0x24); 
        self.data(0x30); 
        self.data(0x3C); 
        self.data(0x48); 
        self.data(0x54); 
        self.data(0x60); 
        self.data(0x6C); 
        self.data(0x78); 
        self.data(0x84); 
        self.data(0x90); 
        self.data(0x9C); 
        self.data(0xA8); 
        self.data(0xB4); 
        self.data(0x00);        #  Enable Gray Scale Table

        
        self.command(0xB1);     #  Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
        self.data(0xE2);        #  Default => 0x74 (7 Display Clocks [Phase 2] / 9 Display Clocks [Phase 1])

        self.command(0xD1);     #  Enhance Driving Scheme Capability (0x00/0x20)
        self.data(0xA2);        #  Normal
        self.data(0x20);        #

       
        self.command(0xBB);     #  Set Pre-Charge Voltage Level as 0.60*VCC
        self.data(0x1F);        #

        self.command(0xB6);     # Set Second Pre-Charge Period as 8 Clocks 
        self.data(0x08);        #

        self.command(0xBE);     # Set Common Pins Deselect Voltage Level as 0.86*VCC 
        self.data(0x07);        # Default => 0x04 (0.80*VCC)

        self.command(0xA6);     # Normal Display Mode (0x00/0x01/0x02/0x03)
                                # Default => 0xA4
                                #     0xA4  => Entire Display Off, All Pixels Turn Off
                                #     0xA5  => Entire Display On, All Pixels Turn On at GS Level 15
                                #     0xA6  => Normal Display
                                #     0xA7  => Inverse Display
         
        self.command(0xA9);     #   Disable Partial Display
        
        self.command(0xAF);     # Display On

     