#pragma once
#include <math.h>

enum class OscillatorMode
{
  SINE,
  SAW,
  TRIANGLE,
  SQUARE
};

class Oscillator
{
public: 
  void setMode(int mode);
  void setFrequency(double frequency);
  void setSampleRate(double sampleRate);
  double generate();

private:
  OscillatorMode mOscillatorMode = (OscillatorMode::SINE);
  const double mPI = (2 * acos(0.0));
  double mFrequency;
  double mPhase = 0.0;
  double mSampleRate;
  double mPhaseIncrement;
  void updateIncrement();
  double output;
};
