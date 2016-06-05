/*
 * MedusaStorage.h - Storage for the Medusa system
 * Copyright 2016 Christian Roring
 */
#include "MedusaStorage.h"
#include <i2c_t3.h>

#define PROG_START 10
#define PROG_LENGTH 7

MedusaStorage::MedusaStorage(int _deviceAddress) {
    deviceAddress = _deviceAddress;
    programStart = PROG_START;
}

byte MedusaStorage::loadSetting(unsigned int _addr) {
    byte data;

    Wire1.beginTransmission(deviceAddress);
    Wire1.write((int)(_addr >> 8)); // MSB
    Wire1.write((int)(_addr & 0xFF)); // LSB
    Wire1.endTransmission();
    Wire1.requestFrom(deviceAddress, 1);

    while (Wire1.available())    // slave may send less than requested
    {
        data = Wire1.read();
    }

    Serial.print("LOAD: ");
    Serial.print(data);
    Serial.print(" FROM: ");
    Serial.println(_addr);

    return data;
}

void MedusaStorage::saveSetting(unsigned int _addr, byte _data) {
    Serial.print("SAVE: ");
    Serial.print(_data);
    Serial.print(" TO: ");
    Serial.println(_addr);

    Wire1.beginTransmission(deviceAddress);
    Wire1.write((int)(_addr >> 8)); // MSB
    Wire1.write((int)(_addr & 0xFF)); // LSB
    Wire1.write(_data);
    Wire1.endTransmission();
}

void MedusaStorage::loadSettings(int _prog, byte** _data) {
    unsigned int _addr = PROG_START + (_prog * PROG_LENGTH);

    Wire1.beginTransmission(deviceAddress);
    Wire1.write((int)(_addr >> 8)); // MSB
    Wire1.write((int)(_addr & 0xFF)); // LSB
    Wire1.endTransmission();
    Wire1.requestFrom(deviceAddress, PROG_LENGTH);

    for ( int i = 0; i < PROG_LENGTH; i++ ) {
        if (Wire1.available()) {
          *_data[i] = (byte)Wire1.read();
        };
    };

    Serial.print("LOAD: ");
    Serial.print("BPM after: ");
    Serial.print(*_data[0]);
    Serial.print(" DEPTH: ");
    Serial.print(*_data[1]);
    Serial.print(" WAVE: ");
    Serial.print(*_data[2]);
    Serial.print(" MULT: ");
    Serial.print(*_data[3]);
    Serial.print(" MOD: ");
    Serial.print(*_data[4]);
    Serial.print(" EXP: ");
    Serial.print(*_data[5]);
    Serial.print(" FROM: ");
    Serial.println(_addr);
};

void MedusaStorage::saveSettings(int _prog, byte** _data) {
    unsigned int _addr = PROG_START + (_prog * PROG_LENGTH);

    Serial.print("SAVE: ");
    Serial.print("BPM: ");
    Serial.print(*_data[0]);
    Serial.print(" DEPTH: ");
    Serial.print(*_data[1]);
    Serial.print(" WAVE: ");
    Serial.print(*_data[2]);
    Serial.print(" MULT: ");
    Serial.print(*_data[3]);
    Serial.print(" MOD: ");
    Serial.print(*_data[4]);
    Serial.print(" EXP: ");
    Serial.print(*_data[5]);
    Serial.print(" TO: ");
    Serial.println(_addr);

    Wire1.beginTransmission(deviceAddress);
    Wire1.write((int)(_addr >> 8)); // MSB
    Wire1.write((int)(_addr & 0xFF)); // LSB

    for ( int i = 0; i < PROG_LENGTH; i++ ) {
        Wire1.write(*_data[i]);
    };

    Wire1.endTransmission();
};
