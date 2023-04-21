#include "cmath"
#pragma once
class ProcessDelay
{
public:
  void Destroy();
  void cookVars();
  void resetDelay();
  void Reset(double samplerate);
  void SetVars(double delay, double feedback, int sync, int pingPong, double tempo, int pt);
  double ProcessSample(double input);

private:
  double mDelaySam;
  double mDelayMs;
  double mFeedback;
  double SampleRate;
  int mSync;
  int mPingPong;
  double mTempo;
  int mPT;

  double* mpBuffer = NULL;
  int mReadIndex = 0;
  int mWriteIndex = 0;
  int mBufferSize = 0;
};
