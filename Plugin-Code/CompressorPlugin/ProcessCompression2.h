#include "ProcessDistortion.h"
#include <string>
#pragma once

class ProcessCompression2
{
public:
  void Compressor();
  double CompressSample(double data, double thresh, int ratio, double attack, double release);

private:
  // Create the circular buffer
  double tav, rms, gain;
  double RatioC;

  // Circular Buffer Code
  double* mpBuffer = NULL;
  int mReadindex = 0;
  int mWriteIndex = 0;

  double GetData();
  void SetData(double data);
  void NextSample();
};