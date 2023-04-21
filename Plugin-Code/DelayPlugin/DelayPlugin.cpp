#include "DelayPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "ProcessDelay.h"
#include "Biquad.h"

// Instance imported classes
ProcessDelay BL;
ProcessDelay BR;
Biquad* hsFilter = new Biquad();
ProcessDistortion D;

DelayPlugin::DelayPlugin(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kDelayMs)->InitDouble("Delay", 10., 0.1, 2000., 0.01, "Ms");
  GetParam(kFeedback)->InitDouble("Feedback", 50., 0., 100., 0.01, "%");
  GetParam(kMix)->InitDouble("Mix", 100., 0., 100., 0.01, "%");
  GetParam(kTone)->InitDouble("Tone", 0.0, -4.0, 0.0, 0.01, "");
  GetParam(kWarmth)->InitDouble("Warmth", 1.0, 1.0, 3.0, 0.01, "");
  GetParam(kBypass)->InitInt("Bypass", 1, 0, 1, "");
  GetParam(kSync)->InitInt("Sync", 0, 0, 1, "");
  GetParam(kPingPong)->InitInt("PingPong", 0, 0, 1, "");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachBackground(GUI_FN);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IBitmap time = pGraphics->LoadBitmap(TIME_FN, 128);
    const IBitmap feedback = pGraphics->LoadBitmap(FEEDBACK_FN, 128);
    const IBitmap warmth = pGraphics->LoadBitmap(WARMTH_FN, 128);
    const IBitmap tone = pGraphics->LoadBitmap(TONE_FN, 128);
    const IBitmap mix = pGraphics->LoadBitmap(MIX_FN, 128);
    const IBitmap bypass = pGraphics->LoadBitmap(BYPASS_FN, 2);
    const IBitmap mode = pGraphics->LoadBitmap(MODE_FN, 2);
    const IBitmap sync = pGraphics->LoadBitmap(SYNC_FN, 2);

    pGraphics->AttachControl(new IBKnobControl(55, 33, time, kDelayMs));
    pGraphics->AttachControl(new IBKnobControl(204, 33, feedback, kFeedback));
    pGraphics->AttachControl(new IBKnobControl(204, 239, mix, kMix));
    pGraphics->AttachControl(new IBKnobControl(204, 136, tone, kTone));
    pGraphics->AttachControl(new IBKnobControl(55, 136, warmth, kWarmth));

    pGraphics->AttachControl(new IBSwitchControl(211, 392, bypass, kBypass));
    pGraphics->AttachControl(new IBSwitchControl(104, 254, sync, kSync));
    pGraphics->AttachControl(new IBSwitchControl(35, 254, mode, kPingPong));

  };
#endif
}

#if IPLUG_DSP
void DelayPlugin::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double mix = GetParam(kMix)->Value() / 100.0;
  const bool bypass = GetParam(kBypass)->Value();
  const double tone = GetParam(kTone)->Value();
  const double warmth = GetParam(kWarmth)->Value();

  const int nChans = NOutChansConnected();

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  
  for (int s = 0; s < nFrames; s++, ++in1, ++in2, ++out1, ++out2)
  {
    double LPS = BL.ProcessSample(*in1);
    double RPS = BR.ProcessSample(*in2);

    if (bypass == 1)
    {
      // Tone
      hsFilter->setPeakGain(tone);
      LPS = ToneWarmth(LPS, warmth);
      RPS = ToneWarmth(RPS, warmth);

      // Mix
      *out1 = (mix * LPS + (1 - mix) * *in1);
      *out2 = (mix * RPS + (1 - mix) * *in2);
    }
    else
    {
      *out1 = *in1;
      *out2 = *in2;
    }
  }
}

// Tone Warmth Processing
double DelayPlugin::ToneWarmth(double signal, double warmth)
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

void DelayPlugin::OnReset()
{

  // Calls buffer's reset function
  BL.Reset(GetSampleRate());
  BR.Reset(GetSampleRate());
  hsFilter->setBiquad(bq_type_highshelf, 3500 / GetSampleRate(), 0.707, 0);
}

void DelayPlugin::OnParamChange(int paramIdx)
{
  double delayMs = GetParam(kDelayMs)->Value();
  double feedback = GetParam(kFeedback)->Value();
  int sync = GetParam(kSync)->Value();
  int pingPong = GetParam(kPingPong)->Value();

  // Cook vars for buffers
  BL.SetVars(delayMs, feedback, sync, pingPong, GetTempo(), 0);
  BR.SetVars(delayMs, feedback, sync, pingPong, GetTempo(), 1);

  BL.cookVars();
  BR.cookVars();
}
#endif

DelayPlugin::~DelayPlugin()
{
  // Delete both buffers by calling their destructors
  BL.Destroy();
  BR.Destroy();
}