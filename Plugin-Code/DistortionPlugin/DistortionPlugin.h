#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kBias,
  kButton,
  kMode,
  kRectify,
  kDynamics,
  kTone,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class DistortionPlugin final : public Plugin
{
public:
  DistortionPlugin(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif

private:
  double mBias;
  double mGain;
  int mButton;
  int mMode;
  int mRectify;
  double mDynamics;
  double mTone;
};
