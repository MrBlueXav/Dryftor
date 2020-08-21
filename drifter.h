/**
 ******************************************************************************
 * File Name          : drifter.h
 * Author			  : Xavier Halgand
 * Date               :
 * Description        :
 ******************************************************************************
 */

/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#ifndef __DRIFTER_H__
#define __DRIFTER_H__
/************************************************************************************/
#include "constants.h"
#include "random.h"
#include <stdint.h>

// namespace daisysp
// {
/*---------------------------------------------------------------------------------*/
class Drifter
{
  public:
    Drifter() {}
    ~Drifter() {}

    void Init(float sample_rate);
    float NextSample();
    void SetAmplitude(float amp);
    void SetMinFreq(float freq);
    void SetMaxFreq(float freq);

  private:
    /*--- parameters ----*/
    float sr_; // sample rate
    float fmax_; // kind of max frequency
    float fmin_; // kind of min frequency
    float gain_; // drifter output is bound to [-gain, +gain]   
    float out_;

    float    minLength_;
    float    maxLength_;
    uint32_t length_;
    uint32_t n_; //sample counter
    float    initial_;
    float    final_;
    float    slope_;

    void NewSegment();
};
/*---------------------------------------------------------------------------------*/
//}



/************************************************************************************/
#endif
