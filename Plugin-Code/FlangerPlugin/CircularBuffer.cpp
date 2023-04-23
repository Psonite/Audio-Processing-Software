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
  // Read from buffer
  double yn = mpBuffer[mReadIndex];

  if (mDelaySam == 0)
  {
    yn = input;
  }

  // Write to buffer
  mpBuffer[mWriteIndex] = input + mFeedback * yn;

  // Increment buffers
  ++mWriteIndex;
  if (mWriteIndex >= mBufferSize)
  {
    mWriteIndex = 0;
  }
  ++mReadIndex;
  if (mReadIndex >= mBufferSize)
  {
    mReadIndex = 0;
  }

  // Return output
  return yn;
}
