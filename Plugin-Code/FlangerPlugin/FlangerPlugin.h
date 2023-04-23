#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kDepth,     // How much lfo affects delay time
  kFeedback,  // Feedback
  kWarmth, // Warmth
  kTone,      // Tone
  kFrequency, // Frequency of Oscillator
  kMix,       // Mix
  kOscMode,   // LFO mode
  kBypass,    // Bypass
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class FlangerPlugin final : public Plugin
{
public:
  FlangerPlugin(const InstanceInfo& info);
  ~FlangerPlugin();

private:
  double mFrequency;
  double mDelay;
  double mDepth;
  double mFeedback = 0.0;
  double mMix;

  int mBypass;
  int mMode;

  double mTone;
  double mWarmth;

  double Lps1 = 0.;
  double Lps2 = 0.;
  double Lps3 = 0.;

  double Rps1 = 0.;
  double Rps2 = 0.;
  double Rps3 = 0.;
  

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnParamChange(int paramIdx) override;
  void OnReset() override;
  double ToneWarmth(double signal, double warmth);
#endif
};
