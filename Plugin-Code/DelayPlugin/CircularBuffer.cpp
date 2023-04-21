#include "CircularBuffer.h"
#include <string.h>

void CircularBuffer::Destroy()
{
  if (mpBuffer)
  {
    delete[] mpBuffer;
  }
}

void CircularBuffer::cookVars(double delay, double feedback) {
  mDelaySam = delay * SampleRate / 1000.0;
  mFeedback = feedback / 100.0;

  mReadIndex = mWriteIndex - (int)mDelaySam;
  if (mReadIndex < 0)
  {
    mReadIndex += mBufferSize;
  }
}

void CircularBuffer::resetDelay()
{
  if (mpBuffer)
  {
    memset(mpBuffer, 0, mBufferSize * sizeof(double));
  }

  mWriteIndex = 0;
  mReadIndex = 0;
}

void CircularBuffer::Reset(double samplerate, double delay, double feedback)
{
  SampleRate = samplerate;
  mBufferSize = 2 * SampleRate;
  if (mpBuffer)
  {
    delete[] mpBuffer;
  }

  mpBuffer = new double[mBufferSize];
  resetDelay();
  cookVars(delay, feedback);
}

double CircularBuffer::ProcessSample(double input)
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
  ++mWriteIndex;
  if (mWriteIndex >= mBufferSize)
  {
    mWriteIndex = 0;
  }

  // same with the read index
  ++mReadIndex;
  if (mReadIndex >= mBufferSize)
  {
    mReadIndex = 0;
  }

  // because we are working in mono we'll just copy the left output to the right output.
  return yn;
}
