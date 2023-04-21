#include "ProcessCompression.h"
#include "cmath"

ProcessDistortion D;

double ProcessCompression::Compression(double input, double threshold, double ratio, double attack, double release, double makeup, int samplerate)
{
  // Allow change of attack and release
  NAttack = samplerate * (attack / 1000);
  NRelease = samplerate * (release / 1000);

  Gain_Step_Attack = (1.0 - ratio) / NAttack;
  Gain_Step_Release = (1.0 - ratio) / NRelease;

  // Stage checker
  if (fabs(input) > threshold)
  {
    if (Gain >= ratio)
    {
      if (CompStates == 0 || CompStates == 3)
      {
        CompStates = 1;
        Timeout = NAttack;
      }
    }
    if (CompStates == 2)
    {
      Timeout = Hold;
    }
  }
  if (fabs(input) < threshold && Gain <= 1.0)
  {
    if (Timeout == 0 && CompStates == 2)
    {
      CompStates = 3;
      Timeout = NRelease;
    }
  }

  // Compression calculation
  switch (CompStates)
  {
  case 0:
    // No Gain Reduction
    if (Gain < 1.0)
    {
      Gain = 1.0;
    }
    break;

  case 1:
    // Attack
    if (Timeout > 0 && Gain > ratio)
    {
      Gain -= Gain_Step_Attack;
      Timeout--;
    }
    else
    {
      CompStates = 2;
      Timeout = Hold;
    }
    break;

  case 2:
    // Gain Reduction
    if (Timeout > 0)
    {
      Timeout--;
    }
    else
    {
      CompStates = 3;
      Timeout = NRelease;
    }
    break;

  case 3:
    // Release
    if (Timeout > 0 && Gain < 1.0)
    {
      Timeout--;
      Gain += Gain_Step_Release;
    }
    else
    {
      CompStates = 0;
    }
    break;
  }

  // Apply Compression and Makeup Gain
  input = input * Gain * makeup;
  // Apply Saturation
  input = D.Distortion(input, 1);

  return input;
}