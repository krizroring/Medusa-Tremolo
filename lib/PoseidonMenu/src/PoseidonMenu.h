/*
 * Poseidon.h - Menu for the Poseidon Tremolo
 * Copyright 2016 Christian Roring
 */

#ifndef POSEIDON_MENU_H
#define POSEIDON_MENU_H

#include "Arduino.h"
#include "MedusaDisplay.h"

class PoseidonMenu
{
public:
    // add the boot animation?

    PoseidonMenu(MedusaDisplay *);
    void displayName();
    void next();
    void prev();
    void displayCurrentMenu();
    int getSelectedMenu();

    void displayWave(int);
    void displayMultiplier(int);
    void displayExpression(int);
protected:
    int menuIndex;
    char buffer[5];
    MedusaDisplay *display;
    void writeDisplay();
};

#endif
