#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kDelayMs = 0,
  kFeedback,
  kMix,
  kTone,
  kWarmth,
  kBypass,
  kSync,
  kPingPong,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class DelayPlugin final : public Plugin
{
public:
  DelayPlugin(const InstanceInfo& info);
  ~DelayPlugin();

private:


#if IPLUG_DSP // http://bit.ly/2S64BDd
  void OnParamChange(int paramIdx) override;
  void OnReset() override;
  double ToneWarmth(double signal, double warmth);
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
