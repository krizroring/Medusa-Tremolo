/*
 * Poseidon tremolo
 * Copyright 2016 - Christian Roring
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "Rotary.h"
#include "MedusaDisplay.h"
#include "PoseidonMenu.h"

// definitions for the encoder modifiers
#define NEXT 1
#define PREV -1

// display names for functions
const char WAVE_SINE[] PROGMEM = "SINE";
const char WAVE_SQR[] PROGMEM = "SQR ";
const char WAVE_TRI[] PROGMEM = "TRI ";
const char WAVE_SAW[] PROGMEM = "SAW ";
const char WAVE_RSAW[] PROGMEM = "RSAW";
const char WAVE_TSAW[] PROGMEM = "TSAW";
const char MULT_0[] PROGMEM = "1/4X";
const char MULT_1[] PROGMEM = "1/2X";
const char MULT_2[] PROGMEM = "  1X";
const char MULT_3[] PROGMEM = "  2X";
const char MULT_4[] PROGMEM = "  4X";

const char* const WAVE_NAMES[] PROGMEM = { WAVE_SINE, WAVE_SQR, WAVE_TRI, WAVE_SAW, WAVE_RSAW, WAVE_TSAW };
const char* const MULTI_NAMES[] PROGMEM = { MULT_0, MULT_1, MULT_2, MULT_3, MULT_4 };

// initial values
int bpm = 120;
int depth = 100; // maximum depth
int wave = 0; // sine wave
int mult = 2; // 1x multiplier
int mod = 0; // no modulation

// encoder select button
int button_pin = 3;
int buttonState = 0;
int debounce = 0;

// library instantiation
Rotary r = Rotary(2, 4);
WaveGenerator waveGenerator = WaveGenerator(bpm, depth, wave, mult, mod);
MedusaDisplay medusaDisplay;
PoseidonMenu poseidonMenu = PoseidonMenu(&medusaDisplay);

//TODO move to poseidon menu
char pedalName[] = "   POSEIDON   ";

// menu states
boolean isMenu = true;
boolean isActive = true;

// the final outpu value to the vactrol / LDR
int output = 0;

void (*changeAction)(int);

void bootAnimation();

void changeBPM(int _direction) {
    bpm = waveGenerator.updateBPM(_direction);
    medusaDisplay.writeDisplay(bpm);
}

void changeDepth(int _direction) {
    depth = waveGenerator.updateDepth(_direction);
    medusaDisplay.writeDisplay(depth);
}

void changeWave(int _direction) {
    wave = waveGenerator.updateWave(_direction);

    char _buffer[5];
    strcpy_P(_buffer, (char*)pgm_read_word(&(WAVE_NAMES[wave])));
    medusaDisplay.writeDisplay(_buffer);
}

void changeMultiplier(int _direction) {
    mult = waveGenerator.updateMultiplier(_direction);

    char _buffer[5];
    strcpy_P(_buffer, (char*)pgm_read_word(&(MULTI_NAMES[mult])));
    medusaDisplay.writeDisplay(_buffer);
}

void changeModulation(int _direction) {
    mod = waveGenerator.updateModulation(_direction);
    medusaDisplay.writeDisplay(mod);
}

void (*actionFN[5])(int) = {&changeBPM, &changeDepth, &changeWave, &changeMultiplier,&changeModulation};

void menuItemSelected(int _selectedMenuItem)
{
    changeAction = actionFN[_selectedMenuItem];
    (*changeAction)(0);
}

void setup()
{
    Serial.begin(9600);

    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);

    medusaDisplay.begin(0x70, 1);
    medusaDisplay.clear();

    pinMode(button_pin, INPUT_PULLUP);

    // bootAnimation();
    // delay(1000);
    poseidonMenu.displayCurrentMenu();

    // set the prescaler for the PWN output (~30 kHz)
    TCCR1B = _BV(CS10);
}

void loop() {
    buttonState = digitalRead(button_pin);

    if (buttonState == LOW && debounce == 0) {
        if (isMenu) {
            isMenu = false;
            menuItemSelected(poseidonMenu.getSelectedMenu());
        } else {
            medusaDisplay.blinkRate(HT16K33_BLINK_2HZ);
            delay(1000);
            medusaDisplay.blinkRate(HT16K33_BLINK_OFF);
            poseidonMenu.displayCurrentMenu();
            isMenu = true;
        }

        debounce = 1;
    }
    if (buttonState == HIGH && debounce == 1) {
        debounce = 0;
    }

    unsigned char result = r.process();
    if(result != DIR_NONE){
        if(isMenu) {
            if (result == DIR_CW) {
                poseidonMenu.next();
            } else {
                poseidonMenu.prev();
            }
        } else {
            if (result == DIR_CW) {
                (*changeAction)(NEXT);
            } else {
                (*changeAction)(PREV);
            }
        }
    }

    int _output = waveGenerator.generate();

    // if (output != _output){
        output = _output;
        analogWrite(9, output);
        analogWrite(10, output);
        Serial.println(output);
    // }

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
