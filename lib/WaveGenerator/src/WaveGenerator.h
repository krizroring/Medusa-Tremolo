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

#define BPM_2_PERIOD(bpm)(60000/bpm)

class WaveGenerator
{
public:
     /**
     Initialization function
     @param unsigned int _bpm The BPM value.
     @param unsigned int* _depth Pointer to the depth.
     @param int* _wave Pointer to the wave type.
     @param float* _multi Pointer to the multiplier.
     @param int* _mod Pointer to the modulation.
     */
     WaveGenerator(unsigned int, unsigned int*, int*, float*, int*);

     /**
     Updates the internal period to incorporate the old en new offset
     */
     void updatePeriod(int);
     /**
     Generates the waveform for the wave led (vactrol) and a status led
     */
     int generate();
private:
    unsigned int *depth;
    int *wave;
    float *multi;
    int *mod;
    unsigned int lfo;

    int period;
    int periodMultiplied;
    unsigned long firstPeriod;
    unsigned int halfMultipliedPeriod;
    unsigned long currentMillis;

    int waveSin(unsigned int);
    int waveSquare(unsigned int);
    int waveTri(unsigned int);
    int waveSaw(unsigned int);
    int waveReverseSaw(unsigned int);

    int (WaveGenerator::*waveFn[5])(unsigned int);

    float generateLFO();
};

#endif
