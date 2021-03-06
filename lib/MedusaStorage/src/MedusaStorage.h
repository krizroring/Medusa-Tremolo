/*
 * MedusaStorage.h - Storage for the Medusa system
 * Copyright 2016 Christian Roring
 */

#ifndef MEDUSA_STORAGE_H
#define MEDUSA_STORAGE_H

#include "Arduino.h"

class MedusaStorage
{
public:
    MedusaStorage(int _deviceAddress);

    byte loadSetting(unsigned int _addr);
    void saveSetting(unsigned int _addr, byte _data);

    /**
    Loads the selected program
    @paran int the porgram number
    @param *byte _data The pointer to the data array
    */
    void loadSettings(int, byte** _data);

    /**
    Saves the selected program
    @paran int the porgram number
    @param *byte _data The pointer to the data array
    */
    void saveSettings(int, byte** _data);

    int programStart;
protected:
    int deviceAddress;
};

#endif
