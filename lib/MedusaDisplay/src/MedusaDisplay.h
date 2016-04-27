/*
 * MedusaDisplay.h - Display driver for the Medusa menu system
 * Copyright 2016 Christian Roring
 *
 * This library is based on the Adafruit_LEDBackpack library, with only the
 * sections needed for a AlphaNumeric display, and some helper functions.
 *
 * Adafruit_LEDBackpack licence:
 ***************************************************
   This is a library for our I2C LED Backpacks

   Designed specifically to work with the Adafruit LED Matrix backpacks
   ----> http://www.adafruit.com/products/
   ----> http://www.adafruit.com/products/

   These displays use I2C to communicate, 2 pins are required to
   interface. There are multiple selectable I2C addresses. For backpacks
   with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
   with 3 Address Select pins: 0x70 thru 0x77

   Adafruit invests time and resources providing this open source code,
   please support Adafruit and open-source hardware by purchasing
   products from Adafruit!

   Written by Limor Fried/Ladyada for Adafruit Industries.
   MIT license, all text above must be included in any redistribution
  ****************************************************
 */

#ifndef MEDUSA_DISPLAY_H
#define MEDUSA_DISPLAY_H

#include "Arduino.h"
#include "Wire.h"

#define HT16K33_CMD_BRIGHTNESS 0x02
#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

class MedusaDisplay
{
public:
    MedusaDisplay(void);
    void begin(uint8_t _addr, uint8_t _b);
    void setBrightness(uint8_t _b);
    void blinkRate(uint8_t _b);
    void writeDisplay(void);
    void clear(void);

    uint16_t displaybuffer[4];
private:
    void writeDigitRaw(uint8_t _n, uint16_t _bitmask);
    void writeDigitAscii(uint8_t _n, uint8_t _ascii, boolean _dot = false);
    uint8_t i2c_addr;
};

#endif
