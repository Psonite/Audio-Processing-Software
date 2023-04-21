#include "ProcessCompression2.h"
#include "cmath"

void ProcessCompression2::Compressor()
{
  // Create buffer of size 150 and delay of 20
  mpBuffer = new double[150];

  tav = 0.01;
  rms = 0;
  gain = 1;
}

double ProcessCompression2::CompressSample(double data, double thresh, int ratio, double attack, double release)
{
  // Ratio Switch statement
  switch (ratio)
  {
  case 0:
    //RatioC = 1;
    RatioC = 1;
    break;

  case 1:
    //RatioC = 0.5;
    RatioC = 2;
    break;

  case 2:
    //RatioC = 0.3333;
    RatioC = 3;
    break;

  case 3:
    //RatioC = 0.2;
    RatioC = 5;
    break;

  case 4:
    //RatioC = 0.1;
    RatioC = 10;
    break;

  case 5:
    //RatioC = 0.05;
    RatioC = 20;
    break;

  case 6:
    //RatioC = 0.0333;
    RatioC = 30;
    break;
  }

  rms = (1 - tav) * rms + tav * pow(data, 2.0);
  double dbRMS = 10 * log10(rms);

  double slope = 1 - (1 / RatioC);
  double dbGain = fmin(0.0, (slope * (thresh - dbRMS)));
  double newGain = pow(10, dbGain / 20);

  double coeff;
  if (newGain < gain)
    coeff = attack;
  else
    coeff = release;
  gain = (1 - coeff) * gain + coeff * newGain;

  double compressedSample = gain * GetData();
  SetData(data);
  NextSample();
  return compressedSample;
}

double ProcessCompression2::GetData() { return mpBuffer[mReadindex]; }

void ProcessCompression2::SetData(double data) { mpBuffer[mWriteIndex] = data; }

void ProcessCompression2::NextSample()
{
  mReadindex = ((150 + mWriteIndex) - 20) % 150;
  mWriteIndex = (mWriteIndex + 1) % 150;
}
