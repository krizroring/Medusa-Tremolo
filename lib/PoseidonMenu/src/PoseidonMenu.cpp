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
const char* const MENU_NAMES[] PROGMEM = {
    item0,
    item1,
    item2,
    item3,
    item4,
    item5,
};

static const int MENU_LENGTH = 6;

PoseidonMenu::PoseidonMenu(MedusaDisplay *_display)
{
    menuIndex = 0;
    display = _display;
};

void PoseidonMenu::next()
{
    if((menuIndex + 1) < MENU_LENGTH)
    {
        menuIndex++;
        writeDisplay();
    }
}
void PoseidonMenu::prev()
{
    if((menuIndex - 1) >= 0)
    {
        menuIndex--;
        writeDisplay();
    }
}

void PoseidonMenu::displayCurrentMenu()
{
    writeDisplay();
}

int PoseidonMenu::getSelectedMenu()
{
    return menuIndex;
}

void PoseidonMenu::writeDisplay()
{
    strcpy_P(buffer, (char*)pgm_read_word(&(MENU_NAMES[menuIndex])));
    (*display).writeDisplay(buffer);
}
