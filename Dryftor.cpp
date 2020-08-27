//-------------------------------------------------------------------------------------
//
//        Dryftor for Daisy pod platform
//        by Xavier Halgand
//			august 2020
//
//*************************************************************************************
#include "Dryftor.h"

//--------------------------------------------------------------------------
using namespace daisysp;
using namespace daisy;


//--------------------------------------------------------------------------

DaisyPod      pod;
PodController podUI; //interface for Dryftor

bool delayON;
bool phaserON;
bool chorusON;
bool stopSound;
bool autoMode;

float sample_rate;
float mainFreq;
float driftAmount;
float state1_k1val = 0.5f;
float state1_k2val = 0.5f;
float state2_k1val = 0.5f;
float state2_k2val = 0.5f;
bool  caught       = true;

// Main oscillators
Oscillator osc[OSC_NUMBER];

// Drifters for the oscillators :
Drifter dfo[OSC_NUMBER];

// Drifter for main frequency, in automatic mode :
Drifter dfmf;

//--------------------------------------------------------------------------

enum Curve
{
    LINEAR,      /**< Linear curve */
    EXPONENTIAL, /**< Exponential curve */
    LOGARITHMIC, /**<  Logarithmic curve */
    CUBE,        /**< Cubic curve */
    LAST,        /**< Final enum element. */
};
//--------------------------------------------------------------------------
float curveFunction(float input, float min, float max, Curve curve)
{
    float val_;
    float lmin_ = logf(min < 0.0000001f ? 0.0000001f : min);
    float lmax_ = logf(max);

    switch(curve)
    {
        case LINEAR: val_ = (input * (max - min)) + min; break;
        case EXPONENTIAL:
            val_ = input;
            val_ = ((val_ * val_) * (max - min)) + min;
            break;
        case LOGARITHMIC: val_ = expf((input * (lmax_ - lmin_)) + lmin_); break;
        case CUBE:
            val_ = input;
            val_ = ((val_ * (val_ * val_)) * (max - min)) + min;
            break;
        default: break;
    }
    return val_;
}

//--------------------------------------------------------------------------

static void AudioCallback(float *in, float *out, size_t size)
{
    //Controls();

    podUI.Process();

    float y, yL, yR;

    for(int i = 0; i < OSC_NUMBER; i++)
        dfo[i].SetAmplitude(driftAmount);

    if(autoMode)
        mainFreq = 30.0f + 440.0f * abs(dfmf.NextSample());

    for(size_t i = 0; i < size; i += 2)
    {
        y = 0.0f;

        if(!stopSound)
        {
            for(int j = 0; j < OSC_NUMBER; j++)
            {
                osc[j].SetFreq(mainFreq * (1 + dfo[j].NextSample()));
                y += osc[j].Process();
            }
            y /= OSC_NUMBER;
        }

        /*---  Apply phaser effect ----*/
        if(phaserON)
            y = Phaser_compute(y);

        /*---  Apply delay effect ----*/
        if(delayON)
            y = Delay_compute(y);

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
//----------------------------------------------------------------------------------------
void s1k1Action(float k1val) //user function for knob1 / state 1
{
    mainFreq
        = curveFunction(k1val, 40, 4000, LOGARITHMIC); // This is the action
}
//---------------------------------------------------------------------------------------
void s2k1Action(float k1val) //user function for knob1 / state 2
{
    for(int i = 0; i < OSC_NUMBER; i++)
    {
        dfo[i].SetMinFreq(2 * k1val + 0.1f);
        dfo[i].SetMaxFreq(4 * k1val + 0.1f);
    }
}

//----------------Write a function "snkn(float knval)" for each state n et knob n ------------------

void s1k1smooth(float k1val) // Smooth control (no value jump) for knob1 / state 1
{
    if(podUI.stateHasChanged())
        caught = false;
    if(!caught)
    {
        if(abs(k1val - state1_k1val) < 0.005f)
        {
            s1k1Action(k1val);
            state1_k1val = k1val;
            caught       = true;
        }
        else
            caught = false;
    }
    else
    {
        s1k1Action(k1val);
        state1_k1val = k1val;
    }
}
//-----------------------------------------------------------------------------------------
void s2k1smooth(float k1val) // Smooth control (no value jump) for knob1 / state 2
{
    if(podUI.stateHasChanged())
        caught = false;
    if(!caught)
    {
        if(abs(k1val - state2_k1val) < 0.005f)
        {
            s2k1Action(k1val);
            state2_k1val = k1val;
            caught       = true;
        }
        else
            caught = false;
    }
    else
    {
        s2k1Action(k1val);
        state2_k1val = k1val;
    }
}
//--------------------------------------------------------------------------

void Knob1Do(float k1val, float oldk1val, int state) 
{
    switch(state)
    {
        case STATE1: // knob1 controls main frequency
        {
            s1k1smooth(k1val);
            break;
        }
        case STATE2: // knob1 controls drift speed of all oscillators
        {
            s2k1smooth(k1val);
            break;
        }
        default: break;
    }
}
//--------------------------------------------------------------------------

void Knob2Do(float k2val, float oldk2val, int state) // No smooth action implemented
{
    switch(state)
    {
        case STATE1:
        {
            driftAmount = curveFunction(k2val, 0, 1, EXPONENTIAL);
            break;
        }
        case STATE2:
        {
            driftAmount = curveFunction(k2val, 0, 1, EXPONENTIAL);
            break;
        }
        default: break;
    }
}
//--------------------------------------------------------------------------

void Button1RE(int state)
{
    switch(state)
    {
        case STATE1:
        {
            autoMode = !autoMode;
            break;
        }
        case STATE2:
        {
            delayON = !delayON;
            break;
        }
        case STATE3:
        {
            phaserON = !phaserON;
            break;
        }
        case STATE4:
        {
            chorusON = !chorusON;
            break;
        }
        default: break;
    }
}
//--------------------------------------------------------------------------

void Button2RE(int state)
{
    switch(state)
    {
        case STATE1:
        {
            stopSound = !stopSound;
            break;
        }
        case STATE2:
        {
            break;
        }
        default: break;
    }
}

//--------------------------------------------------------------------------

//} // namespace DryftorNS
void synthInit()
{
    sample_rate = pod.AudioSampleRate();
    randomGen_init();
    // Init the drifting oscillators
    for(int i = 0; i < OSC_NUMBER; i++)
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

    Delay_init();
    Chorus_init();
    PhaserInit(sample_rate);

    autoMode = true;
    delayON  = true;
    phaserON = true;
    chorusON = true;
    //fx_state  = DELAYonly;
    mainFreq = 440.0f;
}
//**************************************************************************

int main(void)
{
    //Init everything in Daisy Pod
    pod.Init();

    podUI.Init();

    synthInit();

    // start callback
    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1) {}
}

/*--------------------------------END ------------------------------------*/
