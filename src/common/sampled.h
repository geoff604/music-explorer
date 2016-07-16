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
#ifndef SAMPLED_H_INCLUDED
#define SAMPLED_H_INCLUDED
#include "audio.h"
#include "instrumt.h"

class SampledNote : public AbstractNote {
   friend class SampledInstrument;
   
private:
   SampledInstrument *_instrument;
   AudioSample *_currentSample;
   AudioSample *_endData;
   AudioSample *_startLoop;
   AudioSample *_endLoop;
   bool _repeating; // Should I loop?
public:
   // To end a note, just stop repeating
   void EndNote(float) { _repeating = false; }
protected:
   SampledNote(SampledInstrument *instr);
   SampledNote(SampledInstrument *instr, float pitch, float volume);
public:
   void Restart();
private:
   int _volume;
   enum {volumeBits = 13}; // _volume is 1/8192
public:
   void Volume(float volume) {
      _volume = int(volume * (1<<volumeBits));
   }
   float Volume() {
      return static_cast<float>(_volume) / (1<<volumeBits);
   }
private:
   enum {fractionBits = 10};
public:
   size_t AddSamples(AudioSample *buffer, size_t samples);
private:
   int _increment, _fraction;
   float _requestPitch;
   float _requestSampleRate;
   void SetIncrement();
public:
   void Pitch(float pitch) {
      _requestPitch = pitch;
      SetIncrement();
   };
   float Pitch() { return _requestPitch; };
public:
   void SetSampleOffset(int offset);
};

class SampledInstrument : public AbstractInstrument {
private:
   AudioSample *_samples;
   int _sampleLength;
   int _repeatStart;
   int _repeatEnd;
   float _basePitch;
   float _baseSampleRate;

public:
   SampledInstrument();
   SampledInstrument(AudioSample * samples, int length,
                     int repeatStart, int repeatEnd);
   virtual ~SampledInstrument();
   void BasePitch(float basePitch, float baseSampleRate);
   friend class SampledNote; 
   AbstractNote * NewNote(float pitch, float volume) {
      return new SampledNote(this,pitch,volume); 
   };
};

class SineWaveInstrument: public AbstractInstrument {
private:
   SampledInstrument *_sampledInstrument;
   void CreateInstrument();
public:
   SineWaveInstrument() { _sampledInstrument = 0; };
   ~SineWaveInstrument() {
      if (_sampledInstrument)
         delete _sampledInstrument;
   }
   AbstractNote *NewNote(float pitch, float volume) {
      CreateInstrument();
      return _sampledInstrument->NewNote(pitch,volume);
   }
   void SamplingRate(long samplingRate) {
      AbstractInstrument::SamplingRate(samplingRate);
      CreateInstrument();
      _sampledInstrument->SamplingRate(samplingRate);
   }
   long SamplingRate() {
        return AbstractInstrument::SamplingRate();
   }
};
#endif
