//----------------------------------------------------------------------------
//
//    podController.cpp
//    Pod controller : state machine to manage buttons, knobs and encoder
//      by Xavier Halgand
//			august 2020
//
//****************************************************************************

#include "podController.h"

using namespace std;

void PodController::Init()
{
    state    = STATE1;
    oldstate = state;
    c1.Init(daisy::Color::PresetColor::WHITE);
    c2.Init(daisy::Color::PresetColor::WHITE);
    pod.led1.SetColor(c1);
    pod.led2.SetColor(c2);
    k1val    = 0.5;
    k2val    = 0.5;
    oldk1val = k1val;
    oldk2val = k2val;
}

void PodController::NextState()
{
    if(state < STATE_END - 1)
        state = ControllerState(state + 1);

    //if(state == STATE_END)
    else
        state = STATE1;
}

void PodController::PrevState()
{
    if(state == STATE1)
    {
        state = STATE_END;
        state = ControllerState(state - 1);
    }
    else
        state = ControllerState(state - 1);
}

 bool PodController::stateHasChanged()
 {
     return (state != oldstate);
 }

/* ----  Call this function in AudioCallback(float *in, float *out, size_t size) -----*/
void PodController::Process()
{
    int32_t incr;

    pod.UpdateAnalogControls();
    pod.DebounceControls();

    /* --------- Read encoder ----------*/
    incr = pod.encoder.Increment();

    if(incr > 0)
    {
        NextState();
        //EncoderIncr(state);
    }
    if(incr < 0)
    {
        PrevState();
        //EncoderDecr(state);
    }

    /*---------- Read knobs ------------*/
    k1val = pod.knob1.Process();
    k2val = pod.knob2.Process();

    Knob1Do(k1val, oldk1val, state);
    Knob2Do(k2val, oldk2val, state);

    /*---------- Read buttons ----------*/
    if(pod.button1.RisingEdge())
    {
        Button1RE(state);
    }
    if(pod.button2.RisingEdge())
    {
        Button2RE(state);
    }

    /*----------- Update Leds -----------*/
    switch(state)
    {
        case STATE1:
        {
            c1.Init(daisy::Color::PresetColor::WHITE);
            c2.Init(daisy::Color::PresetColor::WHITE);
            break;
        }
        case STATE2:
        {
            c1.Init(daisy::Color::PresetColor::BLUE);
            c2.Init(daisy::Color::PresetColor::BLUE);
            break;
        }
        case STATE3:
        {
            c1.Init(daisy::Color::PresetColor::RED);
            c2.Init(daisy::Color::PresetColor::RED);
            break;
        }
        case STATE4:
        {
            c1.Init(daisy::Color::PresetColor::GREEN);
            c2.Init(daisy::Color::PresetColor::GREEN);
            break;
        }
        default: break;
    }

    pod.led1.SetColor(c1);
    pod.led2.SetColor(c2);
    // Led1Set(state);
    // Led2Set(state);
    pod.UpdateLeds();

    oldk1val = k1val;
    oldk2val = k2val;
    oldstate = state;
}