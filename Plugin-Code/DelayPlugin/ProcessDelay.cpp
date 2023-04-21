#include "ProcessDelay.h"
#include <string.h>

void ProcessDelay::Destroy()
{
  if (mpBuffer)
  {
    delete[] mpBuffer;
  }
}

void ProcessDelay::cookVars()
{
  // Sync Calculation
  mTempo = 150.0;
  if (mSync == 0)
  {
    mDelaySam = mDelayMs * SampleRate / 1000.0;
  }
  else
  {
    if (mDelayMs < 250.0)
    {
      mDelaySam = 0.25 * (SampleRate / (mTempo / 60.0));
    }
    else if (mDelayMs < 500.0)
    {
      mDelaySam = 0.5 * (SampleRate / (mTempo / 60.0));
    }
    else if (mDelayMs < 750.0)
    {
      mDelaySam = 0.75 * (SampleRate / (mTempo / 60.0));
    }
    else if (mDelayMs < 1000.0)
    {
      mDelaySam = 1.0 * (SampleRate / (mTempo / 60.0));
    }
    else if (mDelayMs < 1250.0)
    {
      mDelaySam = 1.25 * (SampleRate / (mTempo / 60.0));
    }
    else if (mDelayMs < 1500.0)
    {
      mDelaySam = 1.5 * (SampleRate / (mTempo / 60.0));
    }
    else if (mDelayMs < 1750.0)
    {
      mDelaySam = 2.0 * (SampleRate / (mTempo / 60.0));
    }
    else
    {
      mDelaySam = 4.0 * (SampleRate / (mTempo / 60.0));
    }
  }

  // Ping Pong
  if (mPingPong == 1 && mDelayMs != 0.)
  {
    if (mPT == 0)
    {
      mReadIndex = mWriteIndex - (2 * (int)mDelaySam) + (int)mDelaySam;
    }
    else
    {
      mReadIndex = mWriteIndex - (2 * (int)mDelaySam);
    }
  }
  else
  {
    mReadIndex = mWriteIndex - (int)mDelaySam;
  }
  

  if (mReadIndex < 0)
  {
    mReadIndex += mBufferSize;
  }
}

void ProcessDelay::resetDelay()
{
  if (mpBuffer)
  {
    memset(mpBuffer, 0, mBufferSize * sizeof(double));
  }

  mWriteIndex = 0;
  mReadIndex = 0;
}

void ProcessDelay::Reset(double samplerate)
{
  SampleRate = samplerate;
  mBufferSize = 2 * SampleRate;
  if (mpBuffer)
  {
    delete[] mpBuffer;
  }

  mpBuffer = new double[mBufferSize];
  resetDelay();
  cookVars();
}

void ProcessDelay::SetVars(double delay, double feedback, int sync, int pingPong, double tempo, int pt)
{
  mDelayMs = delay;
  mFeedback = feedback / 100.;
  mSync = sync;
  mPingPong = pingPong;
  mTempo = tempo;
  mPT = pt;
}

double ProcessDelay::ProcessSample(double input)
{
  // first we read our delayed output
  double yn = mpBuffer[mReadIndex];

  // if the delay is 0 samples we just feed it the input
  if (mDelaySam == 0)
  {
    yn = input;
  }

  // now write to our delay buffer
  mpBuffer[mWriteIndex] = input + mFeedback * yn;

  // increment the write index, wrapping if it goes out of bounds.
  mWriteIndex++;
  if (mWriteIndex >= mBufferSize)
  {
    mWriteIndex = 0;
  }

  // same with the read index
  mReadIndex++;
  if (mReadIndex >= mBufferSize)
  {
    mReadIndex = 0;
  }

  // because we are working in mono we'll just copy the left output to the right output.
  return yn;
}
