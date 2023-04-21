#include "CompressorPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "ProcessCompression.h"

ProcessCompression C;

CompressorPlugin::CompressorPlugin(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  // Set Params
  GetParam(kMakeup)->InitGain("Makeup Gain", 0., 0., 12.0, 0.01);
  GetParam(kThreshold)->InitGain("Threshold", 0., -30., 0., 0.01);
  GetParam(kRatio)->InitDouble("Ratio", 1., -30., 1., 0.01, "dBV");

  GetParam(kAttack)->InitDouble("Attack", 20.0, 1., 500.0, 0.01, "Ms");
  GetParam(kRelease)->InitDouble("Release", 50.0, 1., 2000.0, 0.01, "Ms");
  GetParam(kMix)->InitDouble("Mix", 100.0, 0., 100.0, 0.01, "%");
  GetParam(kBypass)->InitBool("Bypass", 1);

  
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachBackground(GUI_FN);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    // Load Images
    const IBitmap threshold = pGraphics->LoadBitmap(THRESHOLD_FN, 128);
    const IBitmap ratio = pGraphics->LoadBitmap(RATIO_FN, 128);
    const IBitmap attack = pGraphics->LoadBitmap(ATTACK_FN, 128);
    const IBitmap release = pGraphics->LoadBitmap(RELEASE_FN, 128);
    const IBitmap makeup = pGraphics->LoadBitmap(MAKEUP_FN, 128);
    const IBitmap mix = pGraphics->LoadBitmap(MIX_FN, 128);
    const IBitmap bypass = pGraphics->LoadBitmap(BYPASS_FN, 2);

    // Attach controls
    pGraphics->AttachControl(new IBKnobControl(55, 33, threshold, kThreshold));
    pGraphics->AttachControl(new IBKnobControl(204, 33, ratio, kRatio));
    pGraphics->AttachControl(new IBKnobControl(55, 136, attack, kAttack));
    pGraphics->AttachControl(new IBKnobControl(204, 136, release, kRelease));
    pGraphics->AttachControl(new IBKnobControl(55, 239, makeup, kMakeup));
    pGraphics->AttachControl(new IBKnobControl(204, 239, mix, kMix));
    pGraphics->AttachControl(new IBSwitchControl(211, 392, bypass, kBypass));
    
  };
#endif
}

#if IPLUG_DSP
void CompressorPlugin::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  // Update params
  const int nChans = NOutChansConnected();

  double* in1 = inputs[0];
  double* in2 = inputs[1];

  double* out1 = outputs[0];
  double* out2 = outputs[1];
  double Threshold = DBToAmp(GetParam(kThreshold)->Value());
  double Ratio = DBToAmp(GetParam(kRatio)->Value());
  double Attack = GetParam(kAttack)->Value();
  double Release = GetParam(kRelease)->Value();
  double Makeup = DBToAmp(GetParam(kMakeup)->Value());
  double Samplerate = GetSampleRate();

  double mix = GetParam(kMix)->Value() / 100;
  bool bypass = GetParam(kBypass)->Value();

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    if (bypass)
    {
      // Mix
      *out1 = ((mix * (C.Compression(*in1, Threshold, Ratio, Attack, Release, Makeup, Samplerate))) + (1 - mix) * *in1);
      *out2 = ((mix * (C.Compression(*in2, Threshold, Ratio, Attack, Release, Makeup, Samplerate))) + (1 - mix) * *in2);
    }
    else
    {
      *out1 = *in1;
      *out2 = *in2;
    }
    
  }
}
#endif
