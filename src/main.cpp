/*
 * Medusa tremolo
 * Copyright 2016 - Christian Roring
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "Rotary.h"
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

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
};

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Rotary r = Rotary(2, 4);

unsigned int bpm = 240;
unsigned int depth = 0; // set depth
int wave = 0;
float mult = 1;
int mod = 10; // mod depth
unsigned int lfo = 6; // set depth

int displayDepth = 99;
int maxDisplayDepth = 99;
int minDisplayDepth = 0;

WaveGenerator waveGenerator(9, 10);

char name[] = "TREM";

void setup(){
    //set prescaler for pin 10 to 1 for fastest pwm freq
    TCCR1B = _BV(CS10);

    alpha4.begin(0x70);  // pass in the address
    alpha4.setBrightness(2);
    alpha4.clear();

    alpha4.writeDigitAscii(0, name[0]);
    alpha4.writeDigitAscii(1, name[1]);
    alpha4.writeDigitAscii(2, name[2]);
    alpha4.writeDigitAscii(3, name[3]);
    alpha4.writeDisplay();

    waveGenerator.setParams(&bpm, &depth, &wave, &mult, &mod, &lfo);
}

void loop() {
    unsigned char result = r.process();
    int temp = displayDepth;

    if (result == DIR_CW)
    {
        temp = constrain(temp + 1, minDisplayDepth, maxDisplayDepth);
    }
    else if (result == DIR_CCW)
    {
        temp = constrain(temp - 1, minDisplayDepth, maxDisplayDepth);
    }

    if (displayDepth != temp)
    {
        displayDepth = temp;
        depth = map(displayDepth, minDisplayDepth, maxDisplayDepth, 255, 0);

        int tempDepth = displayDepth;
        int arr[2];
        int i = 2;
        while(i >= 0)
        {
            arr[--i] = tempDepth % 10;
            tempDepth /= 10;
        }

        alpha4.clear();
        alpha4.writeDigitRaw(2, pgm_read_word(numfonttable + arr[0]));
        alpha4.writeDigitRaw(3, pgm_read_word(numfonttable + arr[1]));
        alpha4.writeDisplay();
    }

    waveGenerator.generate();
}
