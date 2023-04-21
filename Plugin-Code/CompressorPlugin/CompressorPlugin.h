#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kMakeup = 0,
  kThreshold,
  kRatio,
  kAttack,
  kRelease,
  kMix,
  kHold,
  kBypass,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class CompressorPlugin final : public Plugin
{
public:
  CompressorPlugin(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
