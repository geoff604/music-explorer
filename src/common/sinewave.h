/*
Music Explorer
Copyright (c) 2003 Geoff Peters and Gabriel Lo

This file is part of Music Explorer.

Music Explorer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Music Explorer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Music Explorer (COPYING.TXT); if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
   Copyright 1997 Tim Kientzle.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
      This product includes software developed by Tim Kientzle
      and published in ``The Programmer's Guide to Sound.''
4. Neither the names of Tim Kientzle nor Addison-Wesley
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL TIM KIENTZLE OR ADDISON-WESLEY BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef SINEWAVE_H_INCLUDED
#define SINEWAVE_H_INCLUDED

#include "audio.h"

class SineWave: public AudioAbstract 
{
public:
   void Frequency(double f) 
	{ frequency = f; 
	}

   SineWave(void):AudioAbstract() 
	{ Init(); 
	}

   SineWave(int f):AudioAbstract() 
	{
      Init();
      Frequency(f);
   };

   ~SineWave(void)
	{
      if (sine) delete [] sine;
   }

   void SamplingRate(long rate) 
	{
      AudioAbstract::SamplingRate(rate);
      
   }   

	long SamplingRate(void) 
	{
		return AudioAbstract::SamplingRate();
	}

	virtual void BackwardsStop();
	virtual void BackwardsStart();

protected:
   void MinMaxChannels(int *min, int *max, int *preferred) 
	{
      *min = *max = *preferred = 1;
   };

   size_t GetSamples(AudioSample *buff, size_t bytesWanted);

private:
   AudioSample *sine; // table of sine values
   int length; // length of above table
	long int virtual_length; // length of the virtual table
   long int pos; // current position in virtual table
   double frequency; // desired output frequency

   void Init(void) 
	{
      sine = (AudioSample *)0;
      length=0; pos=0; frequency=1;   
		_stopped = false;

		BuildTable();
   }

	void BuildTable();

	volatile bool _stopped;
};
#endif
