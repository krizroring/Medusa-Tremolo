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
#define MAX_BPM 240
#define MIN_DEPTH 100
#define MAX_DEPTH 0
#define MIN_WAVE 0
#define MAX_WAVE 4
#define MIN_MOD 0
#define MAX_MOD 10
#define MIN_MULTI 0
#define MAX_MULTI 5

#define MIN_LDR_DEPTH 255
#define MAX_LDR_DEPTH 0

class WaveGenerator
{
public:
     /**
     Initialization function
     @param int _bpm The BPM value.
     @param int _depth The depth value.
     @param int _wave Pointer to the wave type.
     @param int _multi The multiplier.
     @param int _mod The modulation value.
     */
     WaveGenerator(int, int, int, int, int);

     /**
     Updates the bpm
     */
     void setBPM(int);
     /**
     Generates the waveform for the wave led (vactrol) and a status led
     */
     int generate();
     /**
     Increases or decreases the BPM
     */
     int updateBPM(int);
     /**
     Increases or decreases the Depth
     */
     int updateDepth(int);
     /**
     Increases or decreases the wave type
     */
     int updateWave(int);
     /**
     Increases or decreases the multiplier
     */
     int updateMultiplier(int);
     /**
     Increases or decreases the modulation
     */
     int updateModulation(int);
protected:
    int bpm;
    int depth;
    int wave;
    int multi;
    int mod;
    unsigned int lfo;

    int ldrDepth;
    float multiplier;
    int period;
    int periodMultiplied;
    int newPeriodMultiplied;
    unsigned long firstPeriod;
    unsigned int halfMultipliedPeriod;
    unsigned long currentMillis;

    void updatePeriod();

    int waveSin(unsigned int);
    int waveSquare(unsigned int);
    int waveTri(unsigned int);
    int waveSaw(unsigned int);
    int waveReverseSaw(unsigned int);

    int (WaveGenerator::*waveFn[5])(unsigned int);

    float generateLFO();
};

#endif
