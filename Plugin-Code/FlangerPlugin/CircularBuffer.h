#include "cmath"
#pragma once
class CircularBuffer
{
public:
  void Destroy();
  void cookVars(double delay, double feedback);
  void resetDelay();
  void Reset(double samplerate, double delay, double feedback);
  double ProcessSample(double input);

private:
  double mDelaySam;
  double mFeedback;
  double SampleRate;

  double* mpBuffer = NULL;
  int mReadIndex = 0;
  int mWriteIndex = 0;
  int mBufferSize = 0;
};
