/**
 ******************************************************************************
 * File Name          : drifter.cpp
 * Author			  : Xavier Halgand
 * Date               :
 * Description        : random segment waveform generator
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

#include "drifter.h"

/*-------------------------------------------------------------------------------------------*/
void Drifter::SetAmplitude(float amp)
{
    gain_ = amp;
}
/*-------------------------------------------------------------------------------------------*/
void Drifter::SetMinFreq(float freq)
{
    fmin_ = freq;
}
/*-------------------------------------------------------------------------------------------*/
void Drifter::SetMaxFreq(float freq)
{
    fmax_ = freq;
}
/*-------------------------------------------------------------------------------------------*/
void Drifter::NewSegment() //
{
    n_         = 0;
    initial_   = final_;
    minLength_ = 0.5f * sr_ / fmax_;
    maxLength_ = 0.5f * sr_ / fmin_;
    length_    = frand_a_b(minLength_, maxLength_);
    final_     = frand_a_b(-1, 1);
    slope_     = (final_ - initial_) / length_;
}
/*-------------------------------------------------------------------------------------------*/
float Drifter::NextSample() //
{
    out_ = gain_ * (slope_ * n_ + initial_);
    (n_)++;
    if(n_ >= length_)
    {
        NewSegment();
    }
    return out_;
}
/*-------------------------------------------------------------------------------------------*/
void Drifter::Init(float sample_rate)
{
    sr_     = sample_rate;
    final_  = 0;
    float r = frand_a_b(0, 1);
    fmax_   = 4 * r + 0.1;
    fmin_   = 2 * r + 0.1;
    gain_   = .01f;

    NewSegment();
}

/*------------------------------------------END--------------------------------------------*/
