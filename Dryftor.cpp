//-------------------------------------------------------------------------------------
//
//        Dryftor for Daisy pod platform
//        by Xavier Halgand
//			august 2020
//
//*************************************************************************************

#include "daisysp.h"
#include "daisy_pod.h"

#include <math.h>
#include <stdint.h>

#include "constants.h"
#include "random.h"
#include "delay.h"
#include "chorusFD.h"
#include "phaser.h"
#include "drifter.h"

//--------------------------------------------------------------------------
using namespace daisysp;
using namespace daisy;

enum FXstate
{
    NOFX,
    DELAYonly,
    DELAYandPHASER,
    DELAYandPHASERandCHORUS,
    DELAYandCHORUS,
    FXend
};

FXstate       fx_state;
bool delayON  _CCM_;
bool phaserON _CCM_;
bool chorusON _CCM_;

static DaisyPod pod;
static int32_t  inc;
float           sample_rate;
Parameter       p_knob1, p_knob2;

bool stopSound;
bool autoMode;

float oldk1, oldk2, k1, k2;
float frequency;
float drift;

Oscillator osc[5];
// Drifters for the oscillators :
Drifter dfo[5];

// Drifter for main frequency, in automatic mode :
Drifter dfmf;

//--------------------------------------------------------------------------

void ConditionalParameter(float  oldVal,
                          float  newVal,
                          float &param,
                          float  update);

//--------------------------------------------------------------------------

void Controls();

//--------------------------------------------------------------------------

static void AudioCallback(float *in, float *out, size_t size)
{
    Controls();

    float y, yL, yR;

    for(int i = 0; i < 5; i++)
        dfo[i].SetAmplitude(drift);

    if(autoMode)
        frequency = 30.0f + 440.0f * abs(dfmf.NextSample());

    for(size_t i = 0; i < size; i += 2)
    {
        y = 0.0f;

        if(!stopSound)
        {
            for(int j = 0; j < 5; j++)
            {
                osc[j].SetFreq(frequency * (1 + dfo[j].NextSample()));
                y += osc[j].Process();
            }
            y *= 1.0 / 5;
        }
        /*---  Apply delay effect ----*/
        if(delayON)
            y = Delay_compute(y);

        /*---  Apply phaser effect ----*/
        if(phaserON)
            y = Phaser_compute(y);

        /*--- Apply chorus/flanger effect ---*/
        if(chorusON)
            stereoChorus_compute(&yL, &yR, y);
        else
            yL = yR = y;

        /*--- clipping ---*/
        yL = (yL > 1.0f) ? 1.0f : yL; //clip too loud left samples
        yL = (yL < -1.0f) ? -1.0f : yL;

        yR = (yR > 1.0f) ? 1.0f : yR; //clip too loud right samples
        yR = (yR < -1.0f) ? -1.0f : yR;

        // left out
        out[i] = yL;

        // right out
        out[i + 1] = yR;
    }
}


//--------------------------------------------------------------------------

//Updates values if knob had changed
void ConditionalParameter(float  oldVal,
                          float  newVal,
                          float &param,
                          float  update)
{
    if(abs(oldVal - newVal) > 0.0005)
    {
        param = update;
    }
}

//--------------------------------------------------------------------------

//Controls Helpers
void UpdateEncoder()
{
    inc = pod.encoder.Increment();
    // Change the effects.
    if(inc > 0)
    {
        fx_state = FXstate(fx_state + 1);

        switch(fx_state)
        {
            case FXend:
            {
                fx_state = NOFX;
                delayON  = false;
                phaserON = false;
                chorusON = false;
                break;
            }
            case NOFX:
            {
                delayON  = false;
                phaserON = false;
                chorusON = false;
                break;
            }
            case DELAYonly:
            {
                delayON  = true;
                phaserON = false;
                chorusON = false;
                break;
            }
            case DELAYandPHASER:
            {
                delayON  = true;
                phaserON = true;
                chorusON = false;
                break;
            }
            case DELAYandPHASERandCHORUS:
            {
                delayON  = true;
                phaserON = true;
                chorusON = true;
                break;
            }
            case DELAYandCHORUS:
            {
                delayON  = true;
                phaserON = false;
                chorusON = true;
                break;
            }
        }
    }
}

//--------------------------------------------------------------------------

void UpdateKnobs()
{
    k1 = pod.knob1.Process();
    k2 = pod.knob2.Process();

    frequency = p_knob1.Process();
    drift     = p_knob2.Process();

    oldk1 = k1;
    oldk2 = k2;
}

//--------------------------------------------------------------------------

void UpdateLeds()
{
    pod.led1.Set(autoMode, autoMode, autoMode);
    pod.led2.Set(stopSound, !stopSound, 0);
    pod.UpdateLeds();
}

//--------------------------------------------------------------------------

void UpdateButtons()
{
    if(pod.button1.RisingEdge())
    {
        autoMode = !autoMode;
    }
    if(pod.button2.RisingEdge())
    {
        stopSound = !stopSound;
    }
}

//--------------------------------------------------------------------------
void Controls()
{
    pod.UpdateAnalogControls();
    pod.DebounceControls();
    UpdateEncoder();
    UpdateKnobs();
    UpdateButtons();
    UpdateLeds();
}
//**************************************************************************

int main(void)
{
    //Init everything
    pod.Init();
    sample_rate = pod.AudioSampleRate();
    oldk1 = oldk2 = 0;
    k1 = k2 = 0;
    //----- central frequency -------------------------------
    p_knob1.Init(pod.knob1, 40, 4000, Parameter::LOGARITHMIC);

    //-----  drift amount ------------------------------------
    p_knob2.Init(pod.knob2, 0, 1, Parameter::EXPONENTIAL);

    // Init the drifting oscillators
    for(int i = 0; i < 5; i++)
    {
        osc[i].Init(sample_rate);
        osc[i].SetAmp(1.f);
        osc[i].SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);

        dfo[i].Init(sample_rate);
    }
    // Init the main frequency drifter in autoMode
    dfmf.Init(sample_rate);
    dfmf.SetAmplitude(8.0f);
    dfmf.SetMinFreq(5.0f);
    dfmf.SetMaxFreq(15.0f);

    randomGen_init();
    Delay_init();
    Chorus_init();
    PhaserInit(sample_rate);

    autoMode = true;
    delayON  = true;
    phaserON = false;
    chorusON = false;
    fx_state = DELAYonly;
    frequency = 440.0f;

    // start callback
    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1) {}
}

/*--------------------------------END ------------------------------------*/
