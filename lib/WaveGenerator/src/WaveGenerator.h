/*
 * MedusaDisplay.h - Wave generator for Arduino
 * Copyright 2016 Christian Roring
 *
 * Code is based on the Harmless tremolo with modifications for adding a modulation LFO
 * http://www.diystompboxes.com/smfforum/index.php?topic=95586.0
 */

#ifndef WAVE_GENERATOR_H
#define WAVE_GENERATOR_H

#include "Arduino.h"
#include "MedusaDisplay.h"

#define BPM_2_PERIOD(bpm)(60000/bpm)

#define MIN_BPM 60
#define MAX_BPM 180
#define MIN_DEPTH 0
#define MAX_DEPTH 100
#define MIN_WAVE 0
#define MAX_WAVE 5
#define MIN_MOD 0
#define MAX_MOD 10
#define MIN_MULTI 0
#define MAX_MULTI 4

#define MIN_LDR_DEPTH 255
#define MAX_LDR_DEPTH 0

// add the expression functions

class WaveGenerator
{
public:
     /**
     Contructor function
     */
     WaveGenerator();
     /**
     Sets the parameters
     @param int _bpm The BPM value.
     @param int _depth The depth value.
     @param int _wave Pointer to the wave type.
     @param int _multi The multiplier.
     @param int _mod The modulation value.
     @param int _tremOff Trem or Volume mode.
     */
     void setParams(byte, byte, byte, byte, byte, byte);
     /**
     Updates the bpm
     */
     void setBPM(byte);
     /**
     Generates the waveform for the wave led (vactrol) and a status led
     */
     int generate();
     /**
     Sets a new BPM and resets the first period
     */
     byte setTappedBPM(byte);
     /**
     Increases or decreases the BPM
     */
     byte updateBPM(int);
     /**
     Increases or decreases the Depth
     */
     byte updateDepth(int);
     /**
     Sets the Depth
     */
     byte setDepth(byte);
     /**
     Increases or decreases the wave type
     */
     byte updateWave(int);
     /**
     Increases or decreases the multiplier
     */
     byte updateMultiplier(int);
     /**
     Increases or decreases the modulation
     */
     byte updateModulation(int);
     /**
     Sets the tremolo in tremolo or volume mode
     */
     byte setPedalMode(int);
protected:
    byte bpm;
    byte depth;
    byte wave;
    byte multi;
    byte mod;
    byte pedalMode;

    unsigned int lfo;

    int ldrDepth;
    float multiplier;
    int period;
    int periodMultiplied;
    int newPeriodMultiplied;
    int lfoPeriod;
    unsigned long firstPeriod;
    unsigned int halfMultipliedPeriod;
    unsigned int threeQuarterMultipliedPeriod;
    unsigned long currentMillis;

    void updatePeriod();

    int waveSin(unsigned int);
    int waveSquare(unsigned int);
    int waveTri(unsigned int);
    int waveSaw(unsigned int);
    int waveReverseSaw(unsigned int);
    int waveTriSaw(unsigned int);

    int (WaveGenerator::*waveFn[6])(unsigned int);

    float generateLFO();
};

#endif
