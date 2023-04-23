#include "FlangerPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "CircularBuffer.h"
#include <projects/Oscillator.h>
#include "Biquad.h"

Oscillator mOscillator;
CircularBuffer BL;
CircularBuffer BR;
Biquad* hsFilter = new Biquad();

FlangerPlugin::FlangerPlugin(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kFrequency)->InitDouble("Frequency", 0.2, 0.01, 10.0, 0.01, "Hz", IParam::kFlagsNone, "", IParam::ShapePowCurve(3.));
  GetParam(kDepth)->InitDouble("Depth", 1., 0., 1., 0.01, "%");
  GetParam(kMix)->InitDouble("Mix", 100., 0., 100., 0.01, "%");
  GetParam(kFeedback)->InitDouble("Feedback", 0., 0., 100., 0.01, "%");
  GetParam(kTone)->InitDouble("Tone", 0.0, -4.0, 0.0, 0.01, "");
  GetParam(kWarmth)->InitDouble("Warmth", 1.0, 1.0, 3.0, 0.01, "");

  GetParam(kBypass)->InitInt("Bypass", 1, 0, 1);
  GetParam(kOscMode)->InitInt("mode", 0, 0, 3);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    
    pGraphics->AttachBackground(GUI_FN);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    // Load images
    const IBitmap depth = pGraphics->LoadBitmap(DEPTH_FN, 128);
    const IBitmap feedback = pGraphics->LoadBitmap(FEEDBACK_FN, 128);
    const IBitmap warmth = pGraphics->LoadBitmap(WARMTH_FN, 128);
    const IBitmap tone = pGraphics->LoadBitmap(TONE_FN, 128);
    const IBitmap frequency = pGraphics->LoadBitmap(FREQUENCY_FN, 128);
    const IBitmap mix = pGraphics->LoadBitmap(MIX_FN, 128);

    const IBitmap mode = pGraphics->LoadBitmap(MODE_FN, 4);
    const IBitmap bypass = pGraphics->LoadBitmap(BYPASS_FN, 2);

    // Attach controls
    
    pGraphics->AttachControl(new IBKnobControl(55, 33, depth, kDepth));
    pGraphics->AttachControl(new IBKnobControl(204, 33, feedback, kFeedback)); 
    pGraphics->AttachControl(new IBKnobControl(55, 136, warmth, kWarmth));
    pGraphics->AttachControl(new IBKnobControl(204, 136, tone, kTone));
    pGraphics->AttachControl(new IBKnobControl(55, 239, frequency, kFrequency));
    pGraphics->AttachControl(new IBKnobControl(204, 239, mix, kMix));

    pGraphics->AttachControl(new IBSwitchControl(199, 344, mode, kOscMode));
    pGraphics->AttachControl(new IBSwitchControl(211, 392, bypass, kBypass));
    
  };
#endif
}

#if IPLUG_DSP
void FlangerPlugin::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  mDelay = 1.0;
  
  for (int s = 0; s < nFrames; s++, ++in1, ++in2, ++out1, ++out2)
  {
    double LPS = BL.ProcessSample(*in1);
    double RPS = BR.ProcessSample(*in2);

    if (mBypass == 1)
    {
      mDelay = 1. + (mDepth * (0.5 * (1. + mOscillator.generate())));
      BL.cookVars(mDelay, mFeedback);
      BR.cookVars(mDelay, mFeedback);

      // Anti-Aliasting code
      LPS = (LPS + Lps1 + Lps2 + Lps3) / 4;
      Lps3 = Lps2;
      Lps2 = Lps1;
      Lps1 = RPS;

      RPS = (RPS + Rps1 + Rps2 + Rps3) / 4;
      Rps3 = Rps2;
      Rps2 = Rps1;
      Rps1 = RPS;


      hsFilter->setPeakGain(mTone);
      LPS = ToneWarmth(LPS, mWarmth);
      RPS = ToneWarmth(RPS, mWarmth);

      // Mix
      *out1 = (mMix * LPS + (1. - mMix) * *in1);
      *out2 = (mMix * RPS + (1. - mMix) * *in2);
    }
    else
    {
      *out1 = *in1;
      *out2 = *in2;
    }
  }
}

void FlangerPlugin::OnParamChange(int paramIdx)
{
  mFrequency = GetParam(kFrequency)->Value();
  mMix = (GetParam(kMix)->Value()) / 200.;
  mDepth = (GetParam(kDepth)->Value() * 19.);
  mTone = (GetParam(kTone)->Value());
  mWarmth = GetParam(kWarmth)->Value();
  mBypass = GetParam(kBypass)->Value();
  mMode = GetParam(kOscMode)->Value();
  mFeedback = GetParam(kFeedback)->Value();

  mOscillator.setMode(mMode);
  mOscillator.setFrequency(mFrequency);
}

void FlangerPlugin::OnReset()
{
  mOscillator.setSampleRate(GetSampleRate());
  BL.Reset(GetSampleRate(), 1.0, 0.0);
  BR.Reset(GetSampleRate(), 1.0, 0.0);
  hsFilter->setBiquad(bq_type_highshelf, 3500 / GetSampleRate(), 0.707, 0);
}

// Tone Warmth Processing
double FlangerPlugin::ToneWarmth(double signal, double warmth)
{

  // Filter Processing
  signal = hsFilter->process(signal);

  // Saturation
  if (warmth > 1.0)
  {
    if (signal > 0)
    {

      signal = 1 - exp(-1 * signal * warmth);
    }
    else
    {

      signal = -1 + exp(signal * warmth);
    }
  }

  return signal;
}
#endif

FlangerPlugin::~FlangerPlugin()
{
  BL.Destroy();
  BR.Destroy();
}