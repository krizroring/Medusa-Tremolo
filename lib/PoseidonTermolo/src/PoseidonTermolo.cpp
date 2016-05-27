/*
 * PoseidonTermolo.cpp - Menu for the Poseidon Tremolo
 * 2016 Christian Roring
 */

#include "Arduino.h"
#include "PoseidonTermolo.h"
#include "MedusaDisplay.h"

const char MENU_1[] PROGMEM = "BPM ";
const char MENU_2[] PROGMEM = "DEPT";
const char MENU_3[] PROGMEM = "WAVE";
const char MENU_4[] PROGMEM = "MULT";
const char MENU_5[] PROGMEM = "MOD ";
const char MENU_6[] PROGMEM = "EXP ";
const char MENU_7[] PROGMEM = "MODE";
const char MENU_8[] PROGMEM = "LOAD";
const char MENU_9[] PROGMEM = "SAVE";
const char MENU_10[] PROGMEM = "DISP";
const char MENU_11[] PROGMEM = "CAL ";

const char* const MENU_NAMES[] PROGMEM = {
    MENU_1, MENU_2, MENU_3, MENU_4, MENU_5, MENU_6, MENU_7, MENU_8, MENU_9, MENU_10, MENU_11};

const char WAVE_SINE[] PROGMEM = "SINE";
const char WAVE_SQR[] PROGMEM = "SQR ";
const char WAVE_TRI[] PROGMEM = "TRI ";
const char WAVE_SAW[] PROGMEM = "SAW ";
const char WAVE_RSAW[] PROGMEM = "RSAW";
const char WAVE_TSAW[] PROGMEM = "TSAW";
const char* const WAVE_NAMES[] PROGMEM = {
    WAVE_SINE, WAVE_SQR, WAVE_TRI, WAVE_SAW, WAVE_RSAW, WAVE_TSAW };

const char MULT_0[] PROGMEM = "1/4X";
const char MULT_1[] PROGMEM = "1/2X";
const char MULT_2[] PROGMEM = "  1X";
const char MULT_3[] PROGMEM = "  2X";
const char MULT_4[] PROGMEM = "  4X";
const char* const MULTI_NAMES[] PROGMEM = {
    MULT_0, MULT_1, MULT_2, MULT_3, MULT_4 };

const char EXP_0[] PROGMEM = "NONE";
const char EXP_1[] PROGMEM = "DEPT";
const char EXP_2[] PROGMEM = "MOD ";
const char EXP_3[] PROGMEM = "MULT";
// const char EXP_4[] PROGMEM = "WAVE";
const char* const EXP_NAMES[] PROGMEM = {
    EXP_0, EXP_1, EXP_2, EXP_3};

static const int EXP_LENGTH = 4;

static const int PROG_LENGTH = 10;

const char CAL_0[] PROGMEM = "MIN ";
const char CAL_1[] PROGMEM = "MAX ";
const char* const CAL_NAMES[] PROGMEM = {
    CAL_0, CAL_1};

const char MODE_0[] PROGMEM = "TREM";
const char MODE_1[] PROGMEM = "VOL ";
const char* const MODE_NAMES[] PROGMEM = {
    MODE_0, MODE_1};

static const int MENU_LENGTH = 11;

static const char PEDAL_NAME[] = "   POSEIDON   ";

PoseidonMenu::PoseidonMenu(MedusaDisplay *_display) {
    menuIndex = 0;
    display = _display;

    expressionMenuLength = EXP_LENGTH;
    numProgramLength = PROG_LENGTH;
};

void PoseidonMenu::next() {
    if((menuIndex + 1) < MENU_LENGTH) {
        menuIndex++;
        writeDisplay();
    }
}
void PoseidonMenu::prev() {
    if((menuIndex - 1) >= 0) {
        menuIndex--;
        writeDisplay();
    }
}

void PoseidonMenu::displayCurrentMenu() {
    writeDisplay();
}

int PoseidonMenu::getSelectedMenu() {
    return menuIndex;
}

void PoseidonMenu::writeDisplay() {
    strcpy_P(buffer, (char*)pgm_read_word(&(MENU_NAMES[menuIndex])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayWave(byte _wave) {
    strcpy_P(buffer, (char*)pgm_read_word(&(WAVE_NAMES[_wave])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayMultiplier(byte _multi) {
    strcpy_P(buffer, (char*)pgm_read_word(&(MULTI_NAMES[_multi])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayExpression(byte _exp) {
    strcpy_P(buffer, (char*)pgm_read_word(&(EXP_NAMES[_exp])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayCalibration(int _cal) {
    strcpy_P(buffer, (char*)pgm_read_word(&(CAL_NAMES[_cal])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayPedalMode(byte _mode) {
    strcpy_P(buffer, (char*)pgm_read_word(&(MODE_NAMES[_mode])));
    (*display).writeDisplay(buffer);
}
