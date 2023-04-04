#include "DistortionPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "ProcessDistortion.h"

ProcessDistortion Processing;

DistortionPlugin::DistortionPlugin(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  // Set up filters
  Processing.SetFilters(GetSampleRate());

  // Bias Knob
  GetParam(kBias)->InitDouble("Bias", 0.0, -0.5, 0.5, 0.01, "%");

  // Gain Knob
  GetParam(kGain)->InitDouble("Gain", 1.0, 1.0, 36.0, 0.01, "%");

  // Bypass Switch
  GetParam(kButton)->InitEnum("On Off", 0, 1);

  // Mode Switch
  GetParam(kMode)->InitInt("Mode", 0, 0, 4);

  // Rectify button
  GetParam(kRectify)->InitEnum("Rectify", 0, 1);

  // Dynamics Knob
  GetParam(kDynamics)->InitDouble("Dynamics", 1.0, 0.0, 1.0, -0.01, "%");

  // Tone Knob
  GetParam(kTone)->InitDouble("Tone", 0.0, -8.0, 8.0, 0.01, "db");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {

    // Create Canvas
    pGraphics->AttachBackground(GUI_FN);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    // Load Images
    const IBitmap Bias = pGraphics->LoadBitmap(BIAS_FN, 128);
    const IBitmap Dynamics = pGraphics->LoadBitmap(DYNAMICS_FN, 128);
    const IBitmap Filter = pGraphics->LoadBitmap(FILTER_FN, 128);
    const IBitmap Gain = pGraphics->LoadBitmap(GAIN_FN, 128);
    const IBitmap Rectify = pGraphics->LoadBitmap(RECTIFY_FN, 2);
    const IBitmap Bypass = pGraphics->LoadBitmap(BYPASS_FN, 2);
    const IBitmap Mode = pGraphics->LoadBitmap(MODE_FN, 5);

    // Attach controls
    pGraphics->AttachControl(new IBKnobControl(204, 33, Bias, kBias));
    pGraphics->AttachControl(new IBKnobControl(55, 136, Dynamics, kDynamics));
    pGraphics->AttachControl(new IBKnobControl(204, 136, Filter, kTone));
    pGraphics->AttachControl(new IBKnobControl(55, 33, Gain, kGain));
    pGraphics->AttachControl(new IBSwitchControl(194, 277, Rectify, kRectify));
    pGraphics->AttachControl(new IBSwitchControl(46, 261, Mode, kMode));
    pGraphics->AttachControl(new IBSwitchControl(211, 392, Bypass, kButton));

  };
#endif
}

#if IPLUG_DSP
void DistortionPlugin::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double mBias = GetParam(kBias)->Value();
  const int mButton = GetParam(kButton)->Value();
  const double mGain = GetParam(kGain)->Value();
  const int mMode = GetParam(kMode)->Value();
  const int mRectify = GetParam(kRectify)->Value();
  const double mDynamics = GetParam(kDynamics)->Value();
  const double mTone = GetParam(kTone)->Value();

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  
  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    // Check if bypassed
    if (mButton == 1)
    {
      // Apply Bias
      *out1 = Processing.DCOffset(*in1, mBias);
      *out2 = Processing.DCOffset(*in2, mBias);

      // Apply Gain
      *out1 = Processing.Gain(*out1, mGain);
      *out2 = Processing.Gain(*out2, mGain);

      // Distortion modes
      *out1 = Processing.Distortion(*out1, mMode);
      *out2 = Processing.Distortion(*out2, mMode);

      // Filter
      *out1 = Processing.ToneFilter(*out1, mTone);
      *out2 = Processing.ToneFilter(*out2, mTone);

      // Dynamics
      *out1 = Processing.Dynamics(*out1, *in1, mDynamics);
      *out2 = Processing.Dynamics(*out2, *in2, mDynamics);

      // Rectify
      if (mRectify == 1)
      {
        *out1 = Processing.Rectify(*out1);
        *out2 = Processing.Rectify(*out2);
      }

      // DC Offset Correction Filter
      *out1 = Processing.DCCorrection(*out1);
      *out2 = Processing.DCCorrection(*out2);

    }
    else
    {
      *out1 = *in1;
      *out2 = *in2;
    }
  }
}
#endif