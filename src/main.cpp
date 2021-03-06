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
#include <i2c_t3.h>

// definitions
#define NEXT 1 // increase ident
#define PREV -1 // decrese ident
#define LDR_PIN 16 // ldr pin
#define BUTTON_PIN 12 // encoder button pin
#define EXP_PIN A0 // expression pedal pin
#define TAP_PIN 15
#define READ_INTERVAL 50 // expression pedal read interval
#define MAJOR 0 // major versioj
#define MINOR 1 // minor version

#define BRIGHTNESS_ADDR 0
#define EXP_MIN_ADDR 1
#define EXP_MAX_ADDR 2
#define CAL_TIME 3000

// library instantiation
Rotary r = Rotary(3, 4);
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

byte *params[] = {&bpm, &depth, &wave, &mult, &mod, &expression, &pedalMode};

byte brightness = 1;

int currentProgram = 0;

boolean calibratingMin = false;
boolean calibratingMax = false;
int expressionMin = 0; // default values for analog read
// max at 1020 because of / 4 to store the setting in a byte
int expressionMax = 1020; // default values for analog read
unsigned long calStartMillis;

int readThreshold = 10;
int expressionValue = -100;

// encoder select button
int buttonState = 0;
int debounce = 0;
elapsedMillis elapsedButtonMillis;

const int maxTaps = 4;
elapsedMillis elapsedTapMillis;
volatile int tapCount = 0;
volatile int tapIndex = 0;
volatile unsigned long taps[4];
volatile bool tapPending = false;
volatile int tapBPM;

// menu states
int selectedMenuItem;
boolean isMenu = true;

// the final output value to the vactrol / LDR
int output = 0;

//Pointer to the action functions
void (*buttonAction)(int);
void (*changeAction)(int);

void noop(int);
void displayMenu();

// Tremolo parameters
void changeParam(int _direction) {
    switch(selectedMenuItem) {
        case 0:
            bpm = waveGenerator.updateBPM(_direction);
            medusaDisplay.writeDisplay(bpm);
            break;
        case 1:
            depth = waveGenerator.updateDepth(_direction);
            medusaDisplay.writeDisplay(depth);
            break;
        case 2:
            wave = waveGenerator.updateWave(_direction);
            poseidonMenu.displayWave(wave);
            break;
        case 3:
            mult = waveGenerator.updateMultiplier(_direction);
            poseidonMenu.displayMultiplier(mult);
            break;
        case 4:
            mod = waveGenerator.updateModulation(_direction);
            medusaDisplay.writeDisplay(mod);
            break;
    }
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

void saveParam(int _index) {
    medusaStorage.saveSetting(medusaStorage.programStart + _index, *params[_index]);
    displayMenu();
}

//Load and save functions
void changeProgram(int _direction) {
    int _tmp = currentProgram + _direction;

    if(_tmp >= 0 && _tmp <= poseidonMenu.numProgramLength) {
        currentProgram = _tmp;
    }

    if (currentProgram == 0) {
        medusaDisplay.writeBack();
    } else {
        poseidonMenu.displayProgram(currentProgram);
    }
}

void loadProgram(int _index) {
    if(currentProgram > 0) {
        medusaStorage.loadSettings(currentProgram, params);
        // store the loaded program in the current running program spot
        medusaStorage.saveSettings(0, params);
        waveGenerator.setParams(bpm, depth, wave, mult, mod, pedalMode);

    } else {
        currentProgram = 1;
    }

    displayMenu();
}

void saveProgram(int _index) {
    if(currentProgram > 0) {
        medusaStorage.saveSettings(currentProgram, params);
    } else {
        currentProgram = 1;
    }

    displayMenu();
}

// System functions

// changes the brightness of the display 1-4
void changeBrightness(int _direction) {
    brightness = medusaDisplay.updateBrightness(_direction);
    medusaDisplay.writeDisplay(brightness);
}
// changes the brightness of the display 1-4
void saveBrightness(int _index) {
    medusaStorage.saveSetting(BRIGHTNESS_ADDR, brightness);
    displayMenu();
}

void calibrateExpression(int _direction) {
    if(_direction == 0) {
        poseidonMenu.displayCalibration(0);
    }
    if (_direction == 2) {
        poseidonMenu.displayCalibration(1);
    }
}

void startCalMin(int _index) {
    medusaDisplay.blinkRate(1);
    calStartMillis = millis();
    expressionMin = 0;
    calibratingMin = true;
}

void startCalMax(int _index) {
    medusaDisplay.blinkRate(1);
    calStartMillis = millis();
    // max at 1020 because of / 4 to store the setting in a byte
    expressionMax = 1020;
    calibratingMax = true;
}

void displayVersion(int _direction) {
    medusaDisplay.displayVersion(MAJOR, MINOR);
}

// noop function
void noop(int _index) {
    displayMenu();
}

static void (*changeFN[])(int) = {&changeParam, &changeParam, &changeParam, &changeParam, &changeParam,
    &changeExpression, &changePedalMode, &changeProgram, &changeProgram, &changeBrightness, &calibrateExpression, &displayVersion};
static void (*buttonFN[])(int) = {&saveParam, &saveParam, &saveParam, &saveParam, &saveParam,
    &saveParam, &saveParam, &loadProgram, &saveProgram, &saveBrightness, &startCalMin, &noop};

void menuItemSelected(int _selectedMenuItem)
{
    selectedMenuItem = _selectedMenuItem;
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

void tapped() {
    cli();

    if(elapsedTapMillis > 200) {
        if(elapsedTapMillis > 1200) { // 2 second reset
            tapCount = 0;
            tapIndex = 0;
            waveGenerator.restart(); // resets the start period
        } else {
            taps[tapIndex] = elapsedTapMillis;
            tapIndex = (tapIndex + 1) % maxTaps;
            tapCount = constrain(tapCount + 1, 0, maxTaps);
        }
    }

    elapsedTapMillis = 0;

    if(tapCount > 1) {
        int total = 0;
        for(int i = 0; i < tapCount; i++) {
            total += taps[i];
        }

        tapBPM = 60000 / (total / tapCount);
        Serial.print("tapBPM ");
        Serial.println(tapBPM);
        tapPending = true;
    }
  sei();
}

unsigned long lastPotRead = millis();

void calibration() {
    unsigned long currentMillis = millis();

    if(currentMillis > lastPotRead + READ_INTERVAL) {
        int temp = analogRead(EXP_PIN);

        if(calibratingMin && (temp > expressionMin)) {
            expressionMin = temp;
        }

        if(calibratingMax && (temp < expressionMax)) {
            expressionMax = temp;
        }

        lastPotRead = currentMillis;
    }
    if (currentMillis > (calStartMillis + CAL_TIME)) {
        if (calibratingMin) {
            medusaStorage.saveSetting(EXP_MIN_ADDR, (byte) (expressionMin / 4));

            calibratingMin = false;

            medusaDisplay.blinkRate(0);
            calibrateExpression(2);

            Serial.print("CAL MIN: ");
            Serial.println(expressionMin);

            buttonAction = &startCalMax;
        }
        if (calibratingMax) {
            medusaStorage.saveSetting(EXP_MAX_ADDR, (byte) (expressionMax / 4));

            calibratingMax = false;

            medusaDisplay.blinkRate(0);

            Serial.print("CAL MAX: ");
            Serial.println(expressionMax);

            displayMenu();
        }
    }
}

void setup()
{
    Serial.begin(9600);
    // delay for teensyLC not having soft reset on serial
    delay(2000);
    Serial.println("START");

    Wire1.begin();

    analogWriteFrequency(16, 46875); // Pin 16 & 17 on FTM1 - ideal for 10 bits
    analogWriteResolution(10); // ideal for 46875 Hz

    pinMode(LDR_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(EXP_PIN, INPUT);

    pinMode(TAP_PIN, INPUT); // sets the digital pin as output
    attachInterrupt(TAP_PIN, tapped, FALLING); // interrrupt 1 is data ready

    //set the button action to be the menu
    buttonAction = &menuItemSelected;

    // temp function for writing the clean eeprom
    // medusaStorage.saveSetting(BRIGHTNESS_ADDR, brightness);
    // medusaStorage.saveSetting(EXP_MIN_ADDR, (byte) (expressionMin / 4));
    // medusaStorage.saveSetting(EXP_MAX_ADDR, (byte) (expressionMax / 4));
    // for (int i = 0; i< 100 ; i++) {
    //     medusaStorage.saveSettings(i, params);
    //     delay(50);
    // }

    medusaStorage.loadSettings(currentProgram, params);
    waveGenerator.setParams(bpm, depth, wave, mult, mod, pedalMode);

    brightness = medusaStorage.loadSetting(BRIGHTNESS_ADDR);

    // maybe bitshift >> 2 to lose 4 points of resolution but save because no rounding is occuring?
    expressionMin = (medusaStorage.loadSetting(EXP_MIN_ADDR) * 4);
    expressionMax = (medusaStorage.loadSetting(EXP_MAX_ADDR) * 4);

    medusaDisplay.begin(0x70, brightness);
    poseidonMenu.displayCurrentMenu();
}

void loop() {
    if (calibratingMin || calibratingMax) {
        calibration();
    } else {
        if (elapsedButtonMillis > 50) {
            buttonState = digitalRead(BUTTON_PIN);

            if (buttonState == LOW && debounce == 0) {
                (*buttonAction)(poseidonMenu.getSelectedMenu());

                debounce = 1;
            }
            if (buttonState == HIGH && debounce == 1) {
                debounce = 0;
            }

            elapsedButtonMillis = 0;
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
        //
        if (output != _output){
            output = _output;
            analogWrite(LDR_PIN, output);
            // Serial.println(output);
        }

        if(tapPending) {
            cli();
            bpm = waveGenerator.setTappedBPM((byte)tapBPM);
            tapPending = false;
            sei();
        }
    }
}
