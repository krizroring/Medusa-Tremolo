/*
 * Poseidon tremolo
 * Copyright 2016 - Christian Roring
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "Rotary.h"
#include "MedusaDisplay.h"
#include "PoseidonTermolo.h"
#include "MedusaStorage.h"

// definitions
#define NEXT 1
#define PREV -1
#define EXP 3

#define LDR_PIN 9
#define STATUS_PIN 10
#define BUTTON_PIN 6
#define EXP_PIN A0

#define READ_INTERVAL 50

// library instantiation
Rotary r = Rotary(5, 7);
WaveGenerator waveGenerator = WaveGenerator();
MedusaDisplay medusaDisplay;
PoseidonMenu poseidonMenu = PoseidonMenu(&medusaDisplay);
MedusaStorage medusaStorage = MedusaStorage(0x50);


// initial values
byte bpm = 80; //the bpm
byte depth = 100; // maximum depth
byte wave = 0; // sine wave
byte mult = 2; // 1x multiplier
byte mod = 0; // no modulation
byte expression = 0; // the expression pedal assignment
byte pedalMode = 0;

static byte *params[] = {&bpm, &depth, &wave, &mult, &mod, &expression, &pedalMode};

byte brightness = 1;
byte noopVal;


int currentProgram = 1;

int readThreshold = 10;
int expressionValue = -100;

// encoder select button
int buttonState = 0;
int debounce = 0;

// menu states
boolean isMenu = true;
boolean isActive = true;

// the final output value to the vactrol / LDR
int output = 0;

//Pointer to the action functions
void (*buttonAction)(int);
void (*changeAction)(int);

void noop(int);
void displayMenu();

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
    int _tmp = expression + _direction;

    if(_tmp >= 0 && _tmp < poseidonMenu.expressionMenuLength) {
        expression = _tmp;
    }

    poseidonMenu.displayExpression(expression);
}

void changePedalMode(int _direction) {
    pedalMode = waveGenerator.setPedalMode(_direction);
    poseidonMenu.displayPedalMode(pedalMode);
}

void changeProgram(int _direction) {
    int _tmp = currentProgram + _direction;

    if(_tmp >= 0 && _tmp <= poseidonMenu.numProgramLength) {
        currentProgram = _tmp;
    }

    if (currentProgram == 0) {
        medusaDisplay.writeBack();
    } else {
        medusaDisplay.writeDisplay(currentProgram);
    }
}

// changes the brightness of the display 1-4
void changeBrightness(int _direction) {
    brightness = medusaDisplay.updateBrightness(_direction);
    medusaDisplay.writeDisplay(brightness);
}
// changes the brightness of the display 1-4
void saveBrightness(int _index) {
    medusaStorage.saveBrightness(brightness);
    displayMenu();
}

void calibrateExpression(int _direction) {

}

void saveParam(int _index) {
    medusaStorage.saveSetting(medusaStorage.programStart + _index, *params[_index]);
    displayMenu();
}

void loadProgram(int _index) {
    if(currentProgram > 0) {
        medusaStorage.loadSettings(currentProgram, *params);
    } else {
        currentProgram = 1;
    }

    // store the loaded program in the current running program spot
    medusaStorage.saveSettings(0, *params);

    waveGenerator.setParams(bpm, depth, wave, mult, mod, pedalMode);

    displayMenu();
}

void saveProgram(int _index) {
    if(currentProgram > 0) {
        medusaStorage.saveSettings(currentProgram, *params);
    } else {
        currentProgram = 1;
    }

    displayMenu();
}

// temp function
void noop(int _index) {
    displayMenu();
}

static void (*changeFN[])(int) = {&changeBPM, &changeDepth, &changeWave, &changeMultiplier, &changeModulation,
    &changeExpression, &changePedalMode, &changeProgram, &changeProgram, &changeBrightness, &calibrateExpression};
static void (*buttonFN[])(int) = {&saveParam, &saveParam, &saveParam, &saveParam, &saveParam,
    &saveParam, &saveParam, &loadProgram, &saveProgram, &saveBrightness, &noop};

void menuItemSelected(int _selectedMenuItem)
{
    changeAction = changeFN[_selectedMenuItem];
    buttonAction = buttonFN[_selectedMenuItem];
    (*changeAction)(0);

    isMenu = false;
}

void displayMenu() {
    poseidonMenu.displayCurrentMenu();
    isMenu = true;
    buttonAction = &menuItemSelected;
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    pinMode(LDR_PIN, OUTPUT);
    pinMode(STATUS_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXP_PIN, INPUT);

    // expressionValue = analogRead(EXP_PIN);

    //set the button action to be the menu
    buttonAction = &menuItemSelected;

    medusaStorage.loadSettings(0, *params);
    waveGenerator.setParams(bpm, depth, wave, mult, mod, pedalMode);
    brightness = medusaStorage.loadBrightness();
    medusaDisplay.begin(0x70, brightness);
    poseidonMenu.displayCurrentMenu();

    // set the prescaler for the PWN output (~30 kHz)
    TCCR1B = _BV(CS10);
}

unsigned long lastPotRead = millis();

void loop() {
    // unsigned long currentMillis = millis();

    buttonState = digitalRead(BUTTON_PIN);

    if (buttonState == LOW && debounce == 0) {
        (*buttonAction)(poseidonMenu.getSelectedMenu());

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


    // if(currentMillis > lastPotRead + READ_INTERVAL) {
    //     int temp =  analogRead(EXP_PIN);
    //
    //     temp = constrain(temp, 30, 900);
    //     // Serial.println(temp);
    //     if ((temp > expressionValue + readThreshold) || (temp < expressionValue - readThreshold)) {
    //         expressionValue = map(temp, 30, 900, 0, 100);
    //
    //     }
    //
    //     Serial.println(expressionValue);
    //
    //     lastPotRead = currentMillis;
    // }

    int _output = waveGenerator.generate();

    // if (output != _output){
        output = _output;
        analogWrite(LDR_PIN, output);
        analogWrite(STATUS_PIN, output);
        // Serial.println(output);
    // }

}
