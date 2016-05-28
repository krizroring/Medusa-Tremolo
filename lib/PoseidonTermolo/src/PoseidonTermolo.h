/*
 * PoseidonTermolo.h - Menu for the Poseidon Tremolo
 * Copyright 2016 Christian Roring
 */

#ifndef POSEIDON_TREMOLO_H
#define POSEIDON_TREMOLO_H

#include "Arduino.h"
#include "MedusaDisplay.h"

class PoseidonMenu
{
public:
    // add the boot animation?

    PoseidonMenu(MedusaDisplay *);
    void next();
    void prev();
    void displayCurrentMenu();
    int getSelectedMenu();

    void displayWave(byte);
    void displayMultiplier(byte);
    void displayExpression(byte);
    void displayCalibration(int);
    void displayPedalMode(byte);
    void displayProgram(int);

    int expressionMenuLength;
    int numProgramLength;
protected:
    int menuIndex;
    char buffer[5];
    MedusaDisplay *display;
    void writeDisplay();
};

#endif
