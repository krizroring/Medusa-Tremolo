/*
 * MedusaDisplay.h - Wave generator for Arduino
 * Copyright 2016 Christian Roring
 */

#ifndef MEDUSA_DISPLAY_H
#define MEDUSA_DISPLAY_H

#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

class MedusaDisplay
{
public:
    /**
    Initialization function
    */
    MedusaDisplay();

    void setBrightness(uint8_t);
    void writeWord(char);
    void writeInt(unsigned int);
    void writeDivision(float);
private:
    Adafruit_AlphaNum4 display;


};

#endif
