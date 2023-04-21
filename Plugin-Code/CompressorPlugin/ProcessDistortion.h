#include "Biquad.h"

#pragma once
class ProcessDistortion
{
public:
  // Functions
  void SetFilters(double sampleRate);

  double DCOffset(double signal, double bias);
  double Gain(double signal, double gain);
  double Distortion(double signal, int mode);
  double ToneFilter(double signal, double tone);
  double Dynamics(double signal, double preSignal, double dynamics);
  double Rectify(double signal);
  double DCCorrection(double signal);

private:
  // High Shelf Filter
  Biquad* hsFilter = new Biquad();

  // DC Correction Filter
  Biquad* hpFilter = new Biquad();
};
