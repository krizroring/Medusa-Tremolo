/*
 * MedusaStorage.h - Storage for the Medusa system
 * Copyright 2016 Christian Roring
 */
#include "MedusaStorage.h"
#include "Wire.h"

MedusaStorage::MedusaStorage(int _deviceAddress) {
    deviceAddress = _deviceAddress;

    // 6 bytes for each programm + (4 char for name?) bpm, depth, wave, multi, mod, exp
    // 4 bytes for config cal low, cal high, brightness, mode
    // 7 bytes for running program?
}

void MedusaStorage::saveSetting(unsigned int _addr, byte _data) {
    Serial.print("WRITE: ");
    Serial.println(_data);
    Wire.beginTransmission(deviceAddress);
    Wire.write((int)(_addr >> 8)); // MSB
    Wire.write((int)(_addr & 0xFF)); // LSB
    Wire.write(_data);
    Wire.endTransmission();
}

byte MedusaStorage::loadSetting(unsigned int _addr) {
    byte data;

    Wire.beginTransmission(deviceAddress);
    Wire.write((int)(_addr >> 8)); // MSB
    Wire.write((int)(_addr & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, 1);

    while (Wire.available())    // slave may send less than requested
    {
        data = Wire.read();
    }

    Serial.print("DATA: ");
    Serial.println(data);

    return data;
}
