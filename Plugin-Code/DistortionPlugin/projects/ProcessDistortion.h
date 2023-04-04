#include "Biquad.h"

#pragma once
class ProcessDistortion
{
public:
  // Functions
  void SetFilters(double sampleRate);

  double DCOffset(double signal, double bias);
  double Gain(double signal, double gain);
  double HardClipping(double signal);
  double SoftClipping(double signal);
  double TriangleFold(double signal);
  double SineFold(double signal);
  double WeirdFold(double signal);
  double ToneFilter(double signal, double tone, double sampleRate);
  double Dynamics(double signal, double preSignal);
  double Rectify(double signal);
  double DCCorrection(double signal, double sampleRate);

private:
  //Variables
  double mBias;
  double mGain;
  int mButton;
  int mMode;
  int mRectify;
  double mDynamics;
  double mTone;

  // High Shelf Filter
  Biquad* hsFilter = new Biquad();

  // DC Correction Filter
  Biquad* hpFilter = new Biquad();
  

};
