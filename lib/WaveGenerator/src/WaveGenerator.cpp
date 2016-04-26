/*
 * WaveGenerator.cpp - Wave generator for Arduino
 * 2016 Christian Roring
 *
 * Code is based on the Harmless tremolo with modifications for adding a modulation LFO
 * http://www.diystompboxes.com/smfforum/index.php?topic=95586.0
 */

#include "Arduino.h"
#include "WaveGenerator.h"

WaveGenerator::WaveGenerator(char _wavePin, char _ratePin)
{
    wavePin = _wavePin;
    ratePin = _ratePin;

    pinMode(wavePin, OUTPUT);
    pinMode(ratePin, OUTPUT);

    orgPeriod = BPM_2_PERIOD(120);
    period = orgPeriod;

    firstPeriod = millis();
    halfPeriod = firstPeriod / 2;

    waveFn[0] = &WaveGenerator::waveSin;
    waveFn[1] = &WaveGenerator::waveSquare;
    waveFn[2] = &WaveGenerator::waveTri;
    waveFn[3] = &WaveGenerator::waveRampUp;
    waveFn[4] = &WaveGenerator::waveRampDown;
};

void WaveGenerator::setParams(unsigned int *_bpm, unsigned int *_depth, int *_wave, float *_mult, int *_mod, unsigned int *_lfo)
{
    bpm = _bpm;
    depth = _depth;
    wave = _wave;
    mult = _mult;
    mod = _mod;
    lfo = _lfo;

    updatePeriod(BPM_2_PERIOD(*bpm));
};

void WaveGenerator::generate()
{
    currentMillis = millis();
    int offset = (currentMillis - firstPeriod) % period;

    ldrWriteIfChanged((this->*waveFn[*wave])(offset));
};

void WaveGenerator::updatePeriod(int _newVal)
{
    orgPeriod = period;
    int periodMultiplied = _newVal * *mult;

    currentMillis = millis();
    unsigned int _offset = (currentMillis - firstPeriod) % periodMultiplied;

    //adjust first period so that we are the same % of the way through the period
    //so there aren't any jarring transitions
    int effectiveNewOffset = map(_offset, 0, orgPeriod - 1, 0, periodMultiplied - 1);
    firstPeriod = currentMillis - effectiveNewOffset;

    halfPeriod = periodMultiplied / 2;

    period = periodMultiplied;
};

// //actually cos so we're on at period start
int WaveGenerator::waveSin(unsigned int _offset)
{
    float rads = ((float)_offset / (float)period) * TWO_PI;

    int temp = constrain((cos(rads) + 1.0) * (255 - *depth) / 2.0 + *depth, 0, 255); //cap at 255 instead of 256

    return temp;
}

int WaveGenerator::waveSquare(unsigned int _offset)
{
    if(_offset < halfPeriod) {
        return 255;
    }
    else {
        return *depth;
    }
}

int WaveGenerator::waveTri(unsigned int _offset)
{
    if(_offset < halfPeriod)
    {
        return map(_offset, 0, halfPeriod, 255, *depth);
    }
    else
    {
        return map(_offset, halfPeriod + 1, period - 1, *depth, 255);
    }
}

int WaveGenerator::waveRampUp(unsigned int _offset)
{
    return map(_offset, 0, period - 1, *depth, 255);
}

int WaveGenerator::waveRampDown(unsigned int _offset)
{
  return map(_offset, 0, period - 1, 255, *depth);
}

float WaveGenerator::generateLFO()
{
    unsigned int modOffset = (currentMillis - firstPeriod) % (orgPeriod * *lfo);
    // generate the mod rad
    float modRads = ((float)modOffset / (float)(orgPeriod * *lfo)) * TWO_PI;
    return cos(modRads + 11.125) * (*mod * 2) / 2.0; // magic numbers :)
}

// writing the ldr
void WaveGenerator::ldrWriteIfChanged(int _newValue)
{
    float modulation = 0.0;
    if (*mod > 0) modulation = generateLFO();

    int newLDRValue = constrain(_newValue + modulation, 0, 255);

    // if(lastLDRValue != newLDRValue)
    // {
        analogWrite(wavePin, newLDRValue);
        analogWrite(ratePin, newLDRValue);
        lastLDRValue = newLDRValue;
    // }
}
