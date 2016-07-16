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
#include "audio.h"
#include "soundexception.h"

void AudioAbstract::Stop() {
   if (Next()) // Are we not the leftmost?
      Next()->Stop(); // No, keep going
   else  // Leftmost
	{ 
      BackwardsStop();
   }
}

void AudioAbstract::BackwardsStop()
{
	if( Previous() ) Previous()->BackwardsStop();
	
	// in derived versions,
	// do something here, such as flushing buffers or quelling source
}

void AudioAbstract::Start() {
   if (Next()) // Are we not the leftmost?
      Next()->Start(); // No, keep going
   else  // Leftmost
	{ 
      BackwardsStart();
   }
}

void AudioAbstract::BackwardsStart()
{
	if( Previous() ) Previous()->BackwardsStart();
	
	// in derived versions,
	// do something here, such as flushing buffers and starting source
}

void AudioAbstract::NegotiateSamplingRate(void) {
   if (Next()) // Are we the leftmost?
      Next()->NegotiateSamplingRate(); // No, keep goin
   else { // Yes, we are
      long min = 8000, max = 44100, preferred = 44100;
      MinMaxSamplingRate(&min,&max,&preferred); // Get preferred values
      if (min > max) { // Check for rediculous answers
		 throw new CSoundException( CSoundException::causeNegotiateSampling);         
      }
      SetSamplingRateRecursive(preferred); // Set them everywhere
   }
}
void AudioAbstract::MinMaxSamplingRate(long *min, long *max,
                                        long *preferred) {
   if (Previous()) Previous()->MinMaxSamplingRate(min,max,preferred);
   if (_samplingRate) *preferred = _samplingRate;
   if (*preferred < *min) *preferred = *min;
   if (*preferred > *max) *preferred = *max;
}

void AudioAbstract::SetSamplingRateRecursive(long s) {
   if (Previous()) // Set towards the right first
      Previous()->SetSamplingRateRecursive(s);
   SamplingRate(s); // Set it
   _samplingRateFrozen = true; // Yes, we've negotiated
}
void AudioAbstract::NegotiateChannels(void) {
   if (Next())
      Next()->NegotiateChannels();
   else {
      int min=1, max=2, preferred=1; // Some reasonable default
      MinMaxChannels(&min,&max,&preferred);
      if (min > max) {
		throw new CSoundException( CSoundException::causeNegotiateSampling);         
      }
      SetChannelsRecursive(preferred);
   }
}

void AudioAbstract::MinMaxChannels(int *min, int *max, int *preferred) {
   if (Previous())  Previous()->MinMaxChannels(min,max,preferred);
   if (_channels) *preferred = _channels;
   if (*preferred < *min) *preferred = *min;
   if (*preferred > *max) *preferred = *max;
}

void AudioAbstract::SetChannelsRecursive(int ch) {
   if (Previous()) Previous()->SetChannelsRecursive(ch);
   Channels(ch);
   _channelsFrozen = true;
}
