/*
*
*   Dryftor.h
*
*
*/
#pragma once
#ifndef DRYFTOR_H
#define DRYFTOR_H

//#include "daisysp.h"
//#include "daisy_pod.h"

#include <math.h>
#include <stdint.h>

#include "constants.h"
#include "random.h"
#include "delay.h"
#include "chorusFD.h"
#include "phaser.h"
#include "drifter.h"
#include "podController.h"

/*----- number of drifting oscillators  ------*/
#define OSC_NUMBER 8

// namespace DryftorNS
// {
//float curveFunction(float input, float min, float max, Curve curve);
void Button1RE(int state);
void Button2RE(int state);
void Knob1Do(float k1val, float oldk1val, int state);
void Knob2Do(float k2val, float oldk2val, int state);

//} // namespace DryftorNS

#endif

/*--------------------------------------------------------------------------*/