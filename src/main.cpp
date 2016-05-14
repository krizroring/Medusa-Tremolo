/*
 * Poseidon tremolo
 * Copyright 2016 - Christian Roring
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "Rotary.h"
#include "MedusaDisplay.h"
#include "PoseidonMenu.h"

// definitions
#define NEXT 1
#define PREV -1
#define EXP 3

#define LDR_PIN 9
#define STATUS_PIN 10
#define BUTTON_PIN 6
#define EXP_PIN A0

#define READ_INTERVAL 50

// initial values
int bpm = 60;
int depth = 100; // maximum depth
int wave = 2; // sine wave
int mult = 2; // 1x multiplier
int mod = 0; // no modulation

int expression = 0;

int brightness = 1;

int readThreshold = 10;
int expressionValue = -100;

// encoder select button
int buttonState = 0;
int debounce = 0;

// library instantiation
Rotary r = Rotary(5, 7);
WaveGenerator waveGenerator = WaveGenerator(bpm, depth, wave, mult, mod, 0);
MedusaDisplay medusaDisplay;
PoseidonMenu poseidonMenu = PoseidonMenu(&medusaDisplay);

// menu states
boolean isMenu = true;
boolean isActive = true;

// the final outpu value to the vactrol / LDR
int output = 0;

void (*buttonAction)();
void (*changeAction)(int);

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
    poseidonMenu.displayWave(wave);
}

void changeMultiplier(int _direction) {
    mult = waveGenerator.updateMultiplier(_direction);
    poseidonMenu.displayMultiplier(mult);
}

void changeModulation(int _direction) {
    mod = waveGenerator.updateModulation(_direction);
    medusaDisplay.writeDisplay(mod);
}

void changeExpression(int _direction) {
    expression += _direction;

    expression = constrain(expression, 0, EXP);
    poseidonMenu.displayExpression(expression);
}

void loadSettings(int _direction) {


}

void saveSettings(int _direction) {


}
// changes the brightness of the display 1-4
void changeBrightness(int _direction) {
    brightness = medusaDisplay.updateBrightness(_direction);
    medusaDisplay.writeDisplay(brightness);
}


void (*actionFN[9])(int) = {&changeBPM, &changeDepth, &changeWave, &changeMultiplier,&changeModulation,
    &changeExpression, &loadSettings, &saveSettings, &changeBrightness};

void menuItemSelected(int _selectedMenuItem)
{
    changeAction = actionFN[_selectedMenuItem];
    (*changeAction)(0);
}

void menuSelectAction();
void valueSetAction();

void menuSelectAction() {
    menuItemSelected(poseidonMenu.getSelectedMenu());

    // TODO find better way for state
    isMenu = false;
    buttonAction = &valueSetAction;
    // add custom handler for config
}

void valueSetAction() {
    //add save option here

    poseidonMenu.displayCurrentMenu();
    isMenu = true;
    buttonAction = &menuSelectAction;
}



void setup()
{
    Serial.begin(9600);

    pinMode(LDR_PIN, OUTPUT);
    pinMode(STATUS_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXP_PIN, INPUT);

    medusaDisplay.begin(0x70, 1);

    expressionValue = analogRead(EXP_PIN);

    //set the button action to be the menu
    buttonAction = &menuSelectAction;

    poseidonMenu.displayName();


    // set the prescaler for the PWN output (~30 kHz)
    TCCR1B = _BV(CS10);
}

unsigned long lastPotRead = millis();

void loop() {
    unsigned long currentMillis = millis();

    buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW && debounce == 0) {
        (*buttonAction)();

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


    if(currentMillis > lastPotRead + READ_INTERVAL) {
        int temp =  analogRead(EXP_PIN);

        temp = constrain(temp, 30, 900);
        // Serial.println(temp);
        if ((temp > expressionValue + readThreshold) || (temp < expressionValue - readThreshold)) {
            expressionValue = map(temp, 30, 900, 0, 100);

        }

        Serial.println(expressionValue);

        lastPotRead = currentMillis;
    }

    int _output = waveGenerator.generate();

    // if (output != _output){
        output = _output;
        analogWrite(LDR_PIN, output);
        analogWrite(STATUS_PIN, output);
        // Serial.println(output);
    // }

}
