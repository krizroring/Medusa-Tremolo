/*
 * MedusaStorage.h - Storage for the Medusa system
 * Copyright 2016 Christian Roring
 */

#ifndef MEDUSA_STORAGE_H
#define MEDUSA_STORAGE_H

#include "Arduino.h"
#include "Wire.h"

// use struct for settings object
// use struct for config object
// use offset for position

class MedusaStorage
{
public:
    MedusaStorage(int _deviceAddress);

    void saveSetting(unsigned int _addr, byte _data);
    byte loadSetting(unsigned int _addr);
protected:
    int deviceAddress;
};

#endif
