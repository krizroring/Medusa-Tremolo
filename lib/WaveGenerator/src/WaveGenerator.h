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
     @param char wavePin The output pin for the vactrol.
     @param char ratePin The output pin for rate led.
     */
     WaveGenerator(char, char);
     /**
     Links the parameters in the generation
     @param unsigned int* *_bpm Pointer to the bpm.
     @param unsigned int* _depth Pointer to the depth.
     @param int* _wave Pointer to the wave type.
     @param float* _mult Pointer to the multiplier.
     @param int* _mod Pointer to the modulation.
     @param unsigned int* _lfo Pointer to the lfo devision of the main period.
     */
     void setParams(unsigned int*, unsigned int*, int*, float*, int*, unsigned int*);
     /**
    Generates the waveform for the wave led (vactrol) and a status led
     */
     void generate();
private:
    char wavePin;
    char ratePin;

    unsigned int *bpm;
    unsigned int *depth;
    int *wave;
    float *mult;
    int *mod;
    unsigned int *lfo;

    int orgPeriod;
    int period;
    unsigned long firstPeriod;
    unsigned int halfPeriod;
    unsigned long currentMillis;

    int lastLDRValue;

    void updatePeriod(int);

    int waveSin(unsigned int);
    int waveSquare(unsigned int);
    int waveTri(unsigned int);
    int waveRampUp(unsigned int);
    int waveRampDown(unsigned int);

    int (WaveGenerator::*waveFn[5])(unsigned int);

    float generateLFO();
    void ldrWriteIfChanged(int);
};

#endif
