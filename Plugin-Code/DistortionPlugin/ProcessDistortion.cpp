#include "ProcessDistortion.h"
#include "Biquad.h"
#include "cmath"

void ProcessDistortion::SetFilters(double sampleRate)
{
  hsFilter->setBiquad(bq_type_highshelf, 3500 / sampleRate, 0.707, 0);
  hpFilter->setBiquad(bq_type_highpass, 19 / sampleRate, 0.9, 0);
}

double ProcessDistortion::DCOffset(double signal, double bias)
{
  signal += bias;
  return signal;
}

double ProcessDistortion::Gain(double signal, double gain)
{
  signal *= gain;
  return signal;
}

double ProcessDistortion::Distortion(double signal, int mode)
{ 
  switch (mode)
    {
    // Hard Clipping
    case 0:
      if (signal >= 0)
      {
        signal = (fmin(signal, 1));
      }
      else
      {
        signal = (fmax(signal, -1));
      }
      break;

    // Soft Clipping
    case 1:
      if (signal > 0)
      {
  
        signal = 1 - exp(-1 * signal);
      }
      else
      {

        signal = -1 + exp(signal);
      }
      break;

    // Triangle Fold
    case 2:
      signal = 4 * (abs((0.25 * signal) + 0.25 - round((0.25 * signal) + 0.25)) - 0.25);
      break;

    // Sine Fold
    case 3:
      signal = sin(signal);
      break;

    // HRM Fold
    case 4:
      signal = (cos(signal)) * (sin(tan(signal)));
      break;
    }
  return signal;
}

double ProcessDistortion::ToneFilter(double signal, double tone)
{
  hsFilter->setPeakGain(tone);
  signal = hsFilter->process(signal);
  return signal;
}

double ProcessDistortion::Dynamics(double signal, double preSignal, double dynamics)
{
  signal = (dynamics * signal) + (1 - dynamics) * (signal * abs(preSignal));
  return signal;
}

double ProcessDistortion::Rectify(double signal)
{
  signal = abs(signal);
  return signal;
}

double ProcessDistortion::DCCorrection(double signal)
{
  signal = hpFilter->process(signal);
  return signal;
}