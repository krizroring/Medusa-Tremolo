/*
 * MedusaDisplay.cpp - Wave generator for Arduino
 * Copyright 2016 Christian Roring
 */

 #include "Arduino.h"
 #include "Wire.h"
 #include "Adafruit_LEDBackpack.h"
 #include "Adafruit_GFX.h"

 #include "MedusaDisplay.h"

static const uint16_t numfonttable[] PROGMEM =  {
    0b0000110000111111, // 0
    0b0000000000000110, // 1
    0b0000000011011011, // 2
    0b0000000010001111, // 3
    0b0000000011100110, // 4
    0b0010000001101001, // 5
    0b0000000011111101, // 6
    0b0000000000000111, // 7
    0b0000000011111111, // 8
    0b0000000011101111, // 9
    0b0000000000000000, //
    0b0000110000000000, // /
};

MedusaDisplay::MedusaDisplay()
{
    display = Adafruit_AlphaNum4();
};

void MedusaDisplay::setBrightness(uint8_t b)
{
    display.setBrightness(b);
}

void MedusaDisplay::writeWord(char word)
{
    display.writeDigitAscii(0, word[0]);
    display.writeDigitAscii(1, word[1]);
    display.writeDigitAscii(2, word[2]);
    display.writeDigitAscii(3, word[3]);
    display.writeDisplay();
}
