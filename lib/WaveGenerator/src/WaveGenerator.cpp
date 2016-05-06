/*
 * WaveGenerator.cpp - Wave generator for Arduino
 * 2016 Christian Roring
 *
 * Code is based on the Harmless tremolo with modifications for adding a modulation LFO
 * http://www.diystompboxes.com/smfforum/index.php?topic=95586.0
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "MedusaDisplay.h"

static const float multiplier_table[] PROGMEM =  { 0.25, 0.5, 1, 2, 4};
static const float LFO_OFFSET = PI / 8; //  tweak for best sound

WaveGenerator::WaveGenerator(int _bpm, int _depth, int _wave, int _multi, int _mod) {
    depth = _depth;
    wave = _wave;
    multi = _multi;
    mod = _mod;
    lfo = 8;

    multiplier = pgm_read_float(multiplier_table+multi);
    firstPeriod = millis();

    waveFn[0] = &WaveGenerator::waveSin;
    waveFn[1] = &WaveGenerator::waveSquare;
    waveFn[2] = &WaveGenerator::waveTri;
    waveFn[3] = &WaveGenerator::waveSaw;
    waveFn[4] = &WaveGenerator::waveReverseSaw;

    updateDepth(0);
    updateBPM(_bpm);
};

int WaveGenerator::generate() {
    currentMillis = millis();
    int offset = (currentMillis - firstPeriod) % periodMultiplied;

    float modulation = 0.0;
    if (mod > 0.0) modulation = generateLFO();

    return constrain((this->*waveFn[wave])(offset) + modulation, MAX_LDR_DEPTH, MIN_LDR_DEPTH);
};

void WaveGenerator::setBPM(int _bpm) {
    period = BPM_2_PERIOD(_bpm);
    newPeriodMultiplied = period / multiplier;

    updatePeriod();
};

int WaveGenerator::updateBPM(int _modifier) {
    bpm += _modifier;
    bpm = constrain(bpm, MIN_BPM, MAX_BPM);
    setBPM(bpm);
    return bpm;
}

int WaveGenerator::updateDepth(int _modifier) {
    depth += _modifier;
    depth = constrain(depth, MIN_DEPTH, MAX_DEPTH);

    ldrDepth = map(depth, MAX_DEPTH, MIN_DEPTH, MAX_LDR_DEPTH, MIN_LDR_DEPTH);
    // check if MAX LDR = 0 and MIN LDR = 255
    return depth;
}
int WaveGenerator::updateWave(int _modifier) {
    wave += _modifier;
    wave = constrain(wave, MIN_WAVE, MAX_WAVE);
    updatePeriod();

    return wave;
}
int WaveGenerator::updateMultiplier(int _modifier) {
    multi += _modifier;
    multi = constrain(multi, MIN_MULTI, MAX_MULTI);
    multiplier = pgm_read_float(multiplier_table+multi);

    setBPM(bpm);

    return multi;
}
int WaveGenerator::updateModulation(int _modifier) {
    mod += _modifier;
    mod = constrain(mod, MIN_MOD, MAX_MOD);

    return mod;
}

//Private functions

void WaveGenerator::updatePeriod() {
    currentMillis = millis();
    unsigned int _offset = (currentMillis - firstPeriod) % periodMultiplied;

    //adjust first period so that we are the same % of the way through the period
    //so there aren't any jarring transitions
    int effectiveNewOffset = map(_offset, 0, periodMultiplied - 1, 0, newPeriodMultiplied - 1);
    firstPeriod = currentMillis - effectiveNewOffset;

    halfMultipliedPeriod = newPeriodMultiplied / 2;

    periodMultiplied = newPeriodMultiplied;

    lfoPeriod = period * lfo;
}
// actually cos so we're on at period start
int WaveGenerator::waveSin(unsigned int _offset) {
    float rads = ((float)_offset / (float)periodMultiplied) * TWO_PI;

    return constrain((cos(rads) + 1.0) * (MIN_LDR_DEPTH - ldrDepth) / 2.0 + ldrDepth, MAX_LDR_DEPTH, MIN_LDR_DEPTH); //cap at 255 instead of 256
}

int WaveGenerator::waveSquare(unsigned int _offset) {
    if(_offset < halfMultipliedPeriod) {
        return MIN_LDR_DEPTH;
    } else {
        return ldrDepth;
    }
}

int WaveGenerator::waveTri(unsigned int _offset) {
    if(_offset < halfMultipliedPeriod) {
        return map(_offset, 0, halfMultipliedPeriod, MIN_LDR_DEPTH, ldrDepth);
    } else {
        return map(_offset, halfMultipliedPeriod + 1, periodMultiplied - 1, ldrDepth, MIN_LDR_DEPTH);
    }
}

int WaveGenerator::waveSaw(unsigned int _offset) {
    return map(_offset, 0, periodMultiplied - 1, ldrDepth, MIN_LDR_DEPTH);
}

int WaveGenerator::waveReverseSaw(unsigned int _offset) {
  return map(_offset, 0, periodMultiplied - 1, MIN_LDR_DEPTH, ldrDepth);
}

float WaveGenerator::generateLFO() {
    unsigned int modOffset = (currentMillis - firstPeriod) % lfoPeriod;
    // generate the mod rad
    float modRads = ((float)modOffset / (float)lfoPeriod ) * TWO_PI;

    return cos(modRads + LFO_OFFSET) * mod; // magic numbers :)
}
