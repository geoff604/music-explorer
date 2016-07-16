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
#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <typeinfo>
#include <iostream>
#include <cstddef>
#include "soundexception.h"

#include "audiosample.h"

// The following line is necessary if your compiler
//    strictly follows the ANSI C++ Standard (almost none do).
// However, some compilers don't implement this feature at all.
// If your compiler complains about this line,
//   simply comment it out and try again.
using namespace std;



class AudioAbstract 
{
  
public:
   virtual void Stop();
	virtual void Start();

	AudioAbstract *Previous(void) { return _previous; }
   void Previous(AudioAbstract *a) { _previous = a; }
   AudioAbstract *Next(void) { return _next; }
   void Next(AudioAbstract *a) {_next = a;}

   AudioAbstract(void) {
      _previous = 0;
      _next = 0;
      _samplingRate = 0;  
		_samplingRateFrozen = false;
      _channels = 0;      
		_channelsFrozen = false;
   };

   AudioAbstract(AudioAbstract *audio) {
      _previous = audio;
      _next = 0;
      audio->Next(this);
      _samplingRate = 0;  _samplingRateFrozen = false;
      _channels = 0;      _channelsFrozen = false;
   };
   virtual ~AudioAbstract(void) {};

   // Returns number of samples actually read, 0 on error.
   // This should always return the full request unless there is
   // an error or end-of-data.
   virtual size_t GetSamples(AudioSample *, size_t) = 0;

   virtual size_t ReadBytes(AudioByte * buff, size_t length) {
      return Previous()->ReadBytes(buff,length);
   };
   
	virtual long SamplingRate(void) {
      if (!_samplingRateFrozen)  // Not frozen?
         NegotiateSamplingRate(); // Go figure it out
      return _samplingRate; // Return it
   };

   virtual void SamplingRate(long s) { // Set the sampling rate
      if( _samplingRate == s )
			return;
		if (_samplingRateFrozen) {
		  throw new CSoundException( CSoundException::causeChangeSampling );
      }
      _samplingRate = s;
   };

   
   virtual int Channels(void) {
      if (!_channelsFrozen) NegotiateChannels();
      return _channels;
   };

   virtual void Channels(int ch) {
      if( _channels == ch )
			return;
		if (_channelsFrozen) {
		 throw new CSoundException( CSoundException::causeChangeNumberOfChannels );        
      }
      _channels = ch;
   };

protected:
   virtual void NegotiateSamplingRate(void);
   virtual void MinMaxSamplingRate(long *min, long *max, long *prefer);
   virtual void SetSamplingRateRecursive(long s);
	
	virtual void NegotiateChannels(void);
   virtual void MinMaxChannels(int *min, int *max, int *preferred) ;
   virtual void SetChannelsRecursive(int s);
	
	virtual void BackwardsStop();
	virtual void BackwardsStart();


private:
   long _samplingRate;
   bool _samplingRateFrozen;
   long _channels;
   bool _channelsFrozen;

   AudioAbstract *_previous; // object to get data from
   AudioAbstract *_next; // object pulling data from us

};

#endif
