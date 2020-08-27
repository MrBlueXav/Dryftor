//-------------------------------------------------------------------------------------
//
//          podController.h
//          Pod controller : state machine to manage buttons, knobs and encoder
//          by Xavier Halgand
//			august 2020
//
//*************************************************************************************
#pragma once
#ifndef POD_CONTROLLER_H
#define POD_CONTROLLER_H

#include "daisy_pod.h"
#include "Dryftor.h"

extern daisy::DaisyPod pod;

enum ControllerState : int
{
    STATE1, // Don't change the first state name !
    STATE2,
    STATE3,
    STATE4,
    STATE_END // Don't change that virtual end state name !
};

class PodController
{
  public:
    PodController() {}
    ~PodController() {}

    void Init();
    void NextState();
    void PrevState();
    void Process();
    bool stateHasChanged();

  private:
    ControllerState state;
    ControllerState oldstate;
    daisy::Color    c1;
    daisy::Color    c2;
    float           k1val;
    float           k2val;
    float           oldk1val;
    float           oldk2val;
};

#endif

//-----------------------------------------------------------------------------