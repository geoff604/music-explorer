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
#ifndef INST_H_INCLUDED
#define INST_H_INCLUDED
#include "audio.h"

class AbstractNote {
protected:
   AbstractNote() {}; // Limit who can create AbstractNote objects
public:
   virtual ~AbstractNote() {}; // Anyone can delete one
   virtual size_t AddSamples(AudioSample *buffer, size_t samples) =0;
   virtual void Restart() = 0; // restart this note
   virtual void EndNote(float) = 0; // stop playing this note
   virtual void Pitch(float) = 0; // set pitch in Hz
   virtual float Pitch() = 0;  // get pitch
   virtual void Volume(float) = 0; // set volume (0.0--1.0)
   virtual float Volume() = 0; // get current volume
};

class AbstractInstrument {
private:
   long _samplingRate;
public:
   virtual void SamplingRate(long samplingRate) {
      _samplingRate = samplingRate;
   }
   virtual long SamplingRate() { return _samplingRate; }
public:
   virtual ~AbstractInstrument() {};
   virtual AbstractNote * NewNote(float pitch, float volume) = 0;
};
#endif
