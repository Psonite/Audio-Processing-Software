#include "Oscillator.h"

void Oscillator::setMode(int mode)
{
  switch (mode)
  {
  case 0:
    mOscillatorMode = OscillatorMode::SINE;
    break;

  case 1:
    mOscillatorMode = OscillatorMode::SAW;
    break;

  case 2:
    mOscillatorMode = OscillatorMode::TRIANGLE;
    break;

  case 3:
    mOscillatorMode = OscillatorMode::SQUARE;
    break;
  }
}

void Oscillator::setFrequency(double frequency)
{
  mFrequency = frequency;
  updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate)
{
  mSampleRate = sampleRate;
  updateIncrement();
}

double Oscillator::generate()
{
  const double twoPI = 2 * mPI;
  switch (mOscillatorMode)
  {
  case (OscillatorMode::SINE):
    output = sin(mPhase);
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI)
    {
      mPhase -= twoPI;
    }
    break;

  case (OscillatorMode::SAW):
    output = 1.0 - (2.0 * mPhase / twoPI);
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI)
    {
      mPhase -= twoPI;
    }
    break;

  case (OscillatorMode::SQUARE):
    if (mPhase <= mPI)
    {
      output = 1.0;
    }
    else
    {
      output = -1.0;
    }
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI)
    {
      mPhase -= twoPI;
    }
    break;

  case (OscillatorMode::TRIANGLE):
    double value = -1.0 + (2.0 * mPhase / twoPI);
    output = 2.0 * (fabs(value) - 0.5);
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI)
    {
      mPhase -= twoPI;
    }
    break;
  }
  return output;
}

void Oscillator::updateIncrement() { mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate; }
