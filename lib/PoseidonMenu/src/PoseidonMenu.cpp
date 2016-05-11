/*
 * PoseidonMenu.cpp - Menu for the Poseidon Tremolo
 * 2016 Christian Roring
 */

#include "Arduino.h"
#include "PoseidonMenu.h"
#include "MedusaDisplay.h"

const char item0[5] PROGMEM = "BPM ";
const char item1[5] PROGMEM = "DEPT";
const char item2[5] PROGMEM = "WAVE";
const char item3[5] PROGMEM = "MULT";
const char item4[5] PROGMEM = "MOD ";
const char item5[5] PROGMEM = "EXP ";
const char item6[5] PROGMEM = "P  1";
const char item7[5] PROGMEM = "P  2";
const char item8[5] PROGMEM = "P  3";
const char item9[5] PROGMEM = "P  4";
const char item10[5] PROGMEM = "DISP";
const char item11[5] PROGMEM = "CAL ";
const char item12[5] PROGMEM = "MODE";
const char* const MENU_NAMES[] PROGMEM = {
    item0, item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, item12 };

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

static const int MENU_LENGTH = 13;

static const char PEDAL_NAME[] = "   POSEIDON   ";

PoseidonMenu::PoseidonMenu(MedusaDisplay *_display) {
    menuIndex = 0;
    display = _display;
};

void PoseidonMenu::displayName() {
    // delay(500);
    int counter =0;
    while (counter < 11) {
        buffer[0] = PEDAL_NAME[counter];
        buffer[1] = PEDAL_NAME[counter + 1];
        buffer[2] = PEDAL_NAME[counter + 2];
        buffer[3] = PEDAL_NAME[counter + 3];

        (*display).writeDisplay(buffer);
        counter ++;

        delay(200);
    }

    displayCurrentMenu();
}

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

void PoseidonMenu::displayWave(int _wave) {
    strcpy_P(buffer, (char*)pgm_read_word(&(WAVE_NAMES[_wave])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayMultiplier(int _multi) {
    strcpy_P(buffer, (char*)pgm_read_word(&(MULTI_NAMES[_multi])));
    (*display).writeDisplay(buffer);
}

void PoseidonMenu::displayExpression(int _exp) {
    strcpy_P(buffer, (char*)pgm_read_word(&(EXP_NAMES[_exp])));
    (*display).writeDisplay(buffer);
}
