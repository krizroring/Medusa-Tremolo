/*
 * WaveGenerator.cpp - Wave generator for Arduino
 * 2016 Christian Roring
 *
 * Code is based on the Harmless tremolo with modifications for adding a modulation LFO
 * http://www.diystompboxes.com/smfforum/index.php?topic=95586.0
 */

#include "Arduino.h"
#include "WaveGenerator.h"

WaveGenerator::WaveGenerator(unsigned int _bpm, unsigned int *_depth, int *_wave, float *_multi, int *_mod)
{
    depth = _depth;
    wave = _wave;
    multi = _multi;
    mod = _mod;
    lfo = 8;

    period = BPM_2_PERIOD(120);
    periodMultiplied = period * *multi;

    firstPeriod = millis();

    waveFn[0] = &WaveGenerator::waveSin;
    waveFn[1] = &WaveGenerator::waveSquare;
    waveFn[2] = &WaveGenerator::waveTri;
    waveFn[3] = &WaveGenerator::waveSaw;
    waveFn[4] = &WaveGenerator::waveReverseSaw;

    updatePeriod(BPM_2_PERIOD(_bpm));
};

void WaveGenerator::updatePeriod(int _newVal)
{
    period = _newVal;

    int newPeriodMultiplied = _newVal * *multi;

    currentMillis = millis();
    unsigned int _offset = (currentMillis - firstPeriod) % periodMultiplied;

    //adjust first period so that we are the same % of the way through the period
    //so there aren't any jarring transitions
    int effectiveNewOffset = map(_offset, 0, periodMultiplied - 1, 0, newPeriodMultiplied - 1);
    firstPeriod = currentMillis - effectiveNewOffset;

    halfMultipliedPeriod = newPeriodMultiplied / 2;

    periodMultiplied = newPeriodMultiplied;
};

int WaveGenerator::generate()
{
    currentMillis = millis();
    int offset = (currentMillis - firstPeriod) % periodMultiplied;

    float modulation = 0.0;
    if (*mod > 0.0) modulation = generateLFO();

    return constrain((this->*waveFn[*wave])(offset) + modulation, 0, 255);
};

//Private functions

// actually cos so we're on at period start
int WaveGenerator::waveSin(unsigned int _offset)
{
    float rads = ((float)_offset / (float)periodMultiplied) * TWO_PI;

    return constrain((cos(rads) + 1.0) * (255 - *depth) / 2.0 + *depth, 0, 255); //cap at 255 instead of 256
}

int WaveGenerator::waveSquare(unsigned int _offset)
{
    if(_offset < halfMultipliedPeriod) {
        return 255;
    }
    else {
        return *depth;
    }
}

int WaveGenerator::waveTri(unsigned int _offset)
{
    if(_offset < halfMultipliedPeriod)
    {
        return map(_offset, 0, halfMultipliedPeriod, 255, *depth);
    }
    else
    {
        return map(_offset, halfMultipliedPeriod + 1, periodMultiplied - 1, *depth, 255);
    }
}

int WaveGenerator::waveSaw(unsigned int _offset)
{
    return map(_offset, 0, periodMultiplied - 1, *depth, 255);
}

int WaveGenerator::waveReverseSaw(unsigned int _offset)
{
  return map(_offset, 0, periodMultiplied - 1, 255, *depth);
}

float WaveGenerator::generateLFO()
{
    unsigned int modOffset = (currentMillis - firstPeriod) % (period * lfo);
    // generate the mod rad
    float modRads = ((float)modOffset / (float)(period * lfo)) * TWO_PI;
    return cos(modRads + 11.125) * (*mod * 2) / 2.0; // magic numbers :)
}
