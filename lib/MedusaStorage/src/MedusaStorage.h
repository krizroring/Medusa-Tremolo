/*
 * MedusaStorage.h - Storage for the Medusa system
 * Copyright 2016 Christian Roring
 */

#ifndef MEDUSA_STORAGE_H
#define MEDUSA_STORAGE_H

#include "Arduino.h"

// use struct for settings object
// use struct for config object
// use offset for position

class MedusaStorage
{
public:
    MedusaStorage();

    void saveSettings();
    void loadSettings();

    void saveConfiguration();
    void loadConfiguration();
protected:
    int settingsOffset;
    int settingsLength;

    int configPosition;
    int configLength;
};

#endif
