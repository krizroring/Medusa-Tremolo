//arduino tremolo v1.0
//harmless machines
//
//feel free to use this code, but please credit if you do thanks

const float twoPi = 2.0 * 3.141592654;

const int outPin = 10;
const int ledPin = 13;
const int periodPin = A0;
const int depthPin = A1;
const int wavePin = A2;
const int exprPin = A3;
const int exprDetectPin = 11;

const int tapPin = 2; //interrupt 0 is pin 2

const int minPeriod = 10;
const int maxPeriod = 1000;

const int minDepth = 255; //255 = full off

const int blinkDivisor = 10;

const int tapDown = 1;
const int tapUp = 0;

const int potCount = 3;
const int waveCount = 7;

volatile int tapState = tapUp;

int waveform = -10; //must be negative enough for 0 to cross all change thresholds
int period = -1;
int rawPeriod = -10;
int depth = -10;

unsigned long firstPeriod = 0;
unsigned int blinkDuration = 0;
unsigned int halfPeriod = 0;

boolean exprPedal = false;
boolean firstChange = true;
int replacedByExpr = -1;

//waveform
//period
//depth
const int potPins[potCount] = { wavePin, periodPin, depthPin }; //hack for expr pedal
int* pots[potCount] = { &waveform, &rawPeriod, &depth };
int (*potReaders[potCount])(void) = { &readWaveform, &readRawPeriod, &readDepth };
void (*potChanged[potCount])(int,int,unsigned long,unsigned int) =  { &waveformChanged, &rawPeriodChanged, &nop };
const int changeThresholds[potCount] = { 0, 1, 1 }; //0 = no wiggle allowed

void nop(int oldVal, int newVal, unsigned long currentMillis, unsigned int offset) { }

int readAnalogPot(int pin) {
  int pinToRead = pin;
  if(exprPedal && (pin == replacedByExpr)) {
    pinToRead = exprPin;
  }
  return analogRead(pinToRead);
}

int readWaveform() { return map(readAnalogPot(wavePin), 0, 1023, 0, waveCount - 1); }
int readRawPeriod() { return map(readAnalogPot(periodPin), 0, 1023, minPeriod, maxPeriod); }
int readDepth() { return map(readAnalogPot(depthPin), 0, 1023, minDepth, 0);}

void waveformChanged(int oldVal, int newVal, unsigned long currentMillis, unsigned int offset) {
  if(newVal >= 5) {
    attachInterrupt(0, changeTapState, CHANGE);
  } else {
    attachInterrupt(0, tapped, RISING);
  }
}

void setPeriod(int newVal, unsigned long currentMillis, unsigned int offset) {
  int oldVal = period;  //can't use old raw value as current period may be tapped

  //adjust first period so that we are the same % of the way through the period
  //so there aren't any jarring transitions
  int effectiveNewOffset = map(offset, 0, oldVal - 1, 0, newVal - 1);
  firstPeriod = currentMillis - effectiveNewOffset;

  blinkDuration = newVal / blinkDivisor;
  halfPeriod = newVal / 2;

  period = newVal; //override any tapped period
}

void rawPeriodChanged(int oldVal, int newVal, unsigned long currentMillis, unsigned int offset) {
  //oldVal is old raw val and is only useful to detect that the raw period has changed
  //moving the knob means override tapped period and force set to knob value
  setPeriod(newVal, currentMillis, offset);
}

//sin, square, tri, ramp up, ramp down, play if down, play if up
int (*waveFns[waveCount])(unsigned int) = { &waveSin, &waveSquare, &waveTri, &waveRampUp, &waveRampDown, &waveIfDown, &waveIfUp };

//actually cos so we're on at period start
int waveSin(unsigned int offset) {
  float rads = ((float)offset / (float)period) * twoPi;
  float amplitude = (255.0 - depth);
  return constrain((cos(rads) + 1.0) * amplitude / 2.0 + depth, 0, 255); //cap at 255 instead of 256
}

int waveSquare(unsigned int offset) {
  if(offset < halfPeriod) return 255;
  else return depth;

}

int waveTri(unsigned int offset) {
  if(offset < halfPeriod) return map(offset, 0, halfPeriod, 255, depth);
  else return map(offset, halfPeriod + 1, period - 1, depth, 255);
}

int waveRampUp(unsigned int offset) {
  return map(offset, 0, period - 1, depth, 255);
}

int waveRampDown(unsigned int offset) {
  return map(offset, 0, period - 1, 255, depth);
}

int waveIf(int tapIf) {
  if(tapState == tapIf) return 255;
  else return 0;
}

int waveIfDown(unsigned int offset) {
  return waveIf(tapDown);
}

int waveIfUp(unsigned int offset) {
  return waveIf(tapUp);
}

volatile boolean tapPending = false;
volatile int tappedPeriod = -1;
const int maxTaps = 5;
int tapCount = 0;
int nextTap = 0;
int taps[maxTaps] = { 0, 0, 0, 0, 0 };
unsigned long lastTap = -maxPeriod;

void tapped() {
  unsigned long current = millis();
  if(current - lastTap > (2 * maxPeriod)) {
    tapCount = 0;
    nextTap = 0;
  } else {
    taps[nextTap] = current - lastTap;
    nextTap = (nextTap + 1) % maxTaps;
    tapCount = constrain(tapCount + 1, 0, maxTaps);
  }
  lastTap = current;

  if(tapCount > 0) {
    int total = 0;
    for(int i = 0; i < tapCount; i++) {
      int tapIndex = nextTap - 1 - i;
      if(tapIndex < 0) tapIndex += maxTaps;
      total += taps[tapIndex];
    }
    tapPending = true;
    tappedPeriod = total / tapCount;
  }
}

void changeTapState() {
  if(tapState == tapUp) tapState = tapDown;
  else tapState = tapUp;
}

void setup(){
//  Serial.begin(9600);
  pinMode(outPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(periodPin, INPUT);
  pinMode(depthPin, INPUT);
  pinMode(wavePin, INPUT);
  pinMode(exprDetectPin, INPUT);
  pinMode(exprPin, INPUT);

  //set prescaler for pin 10 to 1 for fastest pwm freq
  TCCR1B = _BV(CS10);

  unsigned long current = millis();
  firstPeriod = current;
  for(int i=0;i<potCount;i++) updatePot(i, current, 0);

  exprPedal = digitalRead(exprDetectPin) == LOW;
}

int statusLedValue(unsigned int offset) {
  if(offset < blinkDuration) return HIGH;
  else return LOW;
}

void digitalWriteIfChanged(int pin, int *lastValue, int newValue) {
  if(*lastValue != newValue) {
    digitalWrite(pin, newValue);
    *lastValue = newValue;
  }
}

void analogWriteIfChanged(int pin, int *lastValue, int newValue) {
  if(*lastValue != newValue) {
    analogWrite(pin, newValue);
    *lastValue = newValue;
  }
}

int nextPot = 0;
unsigned long lastPotRead = millis();
unsigned int readEvery = 200;

boolean updatePot(int pot, unsigned long currentMillis, unsigned int offset) {
  int newVal = (*potReaders[pot])();
  int oldVal = *pots[pot];
  int delta = oldVal - newVal;
  if(abs(delta) > changeThresholds[pot]) {
    (*potChanged[pot])(oldVal, newVal, currentMillis, offset);
//    Serial.print(pot);Serial.print(" changed from ");Serial.print(oldVal);Serial.print(" to ");Serial.println(newVal);
    *pots[pot] = newVal;
    return true;
  }else return false;
}

void updatePots(unsigned long currentMillis, unsigned int offset) {
  if(currentMillis > lastPotRead + readEvery) {
    boolean updated = updatePot(nextPot, currentMillis, offset);
    if(exprPedal && firstChange && updated) {
      firstChange = false;
      replacedByExpr = potPins[nextPot];
//      Serial.print("Expr pedal is assigned to pot ");Serial.println(nextPot);
    }
    nextPot = (nextPot + 1) % potCount; //don't keep reading the same pot if it's changing - expression pedals wobble and thus other pots wont ever get polled
    lastPotRead = currentMillis;
  }
}

int lastLedValue = LOW;
int lastOutputValue = -1;

void loop() {
  unsigned long currentMillis = millis();
  unsigned int offset = (currentMillis - firstPeriod) % period;

  digitalWriteIfChanged(ledPin, &lastLedValue, statusLedValue(offset));
  analogWriteIfChanged(outPin, &lastOutputValue, (*waveFns[waveform])(offset));
  updatePots(currentMillis, offset);

  if(tapPending) {
    setPeriod(tappedPeriod, currentMillis, offset);
    tapPending = false;
  }
}
