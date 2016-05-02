/*
 * Poseidon tremolo
 * Copyright 2016 - Christian Roring
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "Rotary.h"
#include "MedusaDisplay.h"
#include "PoseidonMenu.h"

unsigned int bpm = 60;
unsigned int depth = 100; // set depth
int wave = 0;
float mult = 1;
int mod = 10; // mod depth
unsigned int lfo = 8;

int maxDisplayDepth = 99;
int minDisplayDepth = 0;
int displayDepth = map(depth, 255, 0, minDisplayDepth, maxDisplayDepth);

int button_pin = 3;
int buttonState = 0;
int debounce = 0;

void menuItemSelected(int);

Rotary r = Rotary(2, 4);
WaveGenerator waveGenerator = WaveGenerator(bpm, &depth, wave, mult, &mod);
MedusaDisplay medusaDisplay;
PoseidonMenu poseidonMenu = PoseidonMenu(&menuItemSelected, &medusaDisplay);

char pedalName[] = "   POSEIDON   ";

boolean isMemu = true;

void bootAnimation();
void setupMenu();
void toDisplay(char*);

void menuItemSelected(int _selectedMenuItem)
{
    Serial.print("Selected: ");
    Serial.println(_selectedMenuItem);
}

void setup()
{
    medusaDisplay.begin(0x70, 1);
    medusaDisplay.clear();

    pinMode(button_pin, INPUT_PULLUP);

    bootAnimation();
    delay(1000);

    poseidonMenu.displayCurrentMenu();

    // set the prescaler
    TCCR1B = _BV(CS10);
}

void loop() {
    buttonState = digitalRead(button_pin);

    if (buttonState == LOW && debounce == 0) {
        if (isMemu)
        {
            poseidonMenu.displayCurrentMenu();
        }

        debounce = 1;
    }
    if (buttonState == HIGH && debounce == 1) {
        debounce = 0;
    }

    unsigned char result = r.process();
    if (result == DIR_NONE) {
      // do nothing
    }
    // use dynamic allocation of function
    else if (result == DIR_CW) {
        poseidonMenu.next();
    }
    else if (result == DIR_CCW) {
        poseidonMenu.prev();
    }
}

void bootAnimation()
{
    delay(1000);
    int counter =0;
    char buf[4];
    while (counter < 11) {
        buf[0] = pedalName[counter];
        buf[1] = pedalName[counter + 1];
        buf[2] = pedalName[counter + 2];
        buf[3] = pedalName[counter + 3];

        medusaDisplay.writeDisplay(buf);
        counter ++;

        delay(200);
    }

    medusaDisplay.clear();
}
