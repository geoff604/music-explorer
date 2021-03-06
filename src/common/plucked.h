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
#ifndef PLUCKED_H_INCLUDED
#define PLUCKED_H_INCLUDED
#include "audio.h"
#include "instrumt.h"

class PluckedStringNote : public AbstractNote {
   friend class PluckedStringInstrument;
private: // Only a friend can create a PluckedStringNote
   PluckedStringInstrument *_instr;
   PluckedStringNote(PluckedStringInstrument *instr,
          float pitch, float volume);
public: // But anyone can delete one
   virtual ~PluckedStringNote();
private:
   float _pitch;
   float _volume;
public:
   void Pitch(float pitch) { _pitch = pitch; };
   float Pitch() { return _pitch; }
   void Volume(float volume) { _volume = volume; };
   float Volume() { return _volume; };
   void Restart();
   size_t AddSamples(AudioSample *buffer, size_t samples);
   void EndNote(float rate) { _decayRate = rate; }
private:
   float _decayRate; // Decay factor
   int _bufferSize; // Size of buffers
   long *_buffer;  // Most recent data
   long *_future; // Next filtered data
   int _pos; // Current position in buffer
   int _iterations; // How often to filter the buffer
   int _remaining; // When next to filter the buffer
};
class PluckedStringInstrument : public AbstractInstrument {
public: // Do-nothing constructor and destructor
   PluckedStringInstrument() {};
   virtual ~PluckedStringInstrument() {};
public:
   AbstractNote * NewNote(float pitch, float volume) {
      return new PluckedStringNote(this,pitch,volume);
   };
};
#endif
