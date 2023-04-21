#include "ProcessDistortion.h"
#include "IPlugUtilities.h"

#pragma once
class ProcessCompression
{
public:
  
  double Compression(double input, double threshold, double ratio, double attack, double release, double makeup, int samplerate);

private:
  int Hold = 1000;
  int Timeout;
  int NAttack;
  int NRelease;
  double Gain = 1.0;
  double Gain_Step_Attack;
  double Gain_Step_Release;
  int CompStates = 0;
};