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
#include <cmath>
#include "instrumt.h"
#include "sampled.h"

SampledInstrument::SampledInstrument() {
   _basePitch = 440;
   _baseSampleRate = 8000;
   _samples = 0;
   _sampleLength = 0;
   _repeatStart = _repeatEnd = 0;
}

SampledInstrument::SampledInstrument(AudioSample * samples,
       int length, int repeatStart, int repeatEnd) {
   _basePitch = 440;
   _baseSampleRate = 8000;
   _samples = 0;

   if (length > 0) { // Copy samples into local buffer
      _samples = new AudioSample[length];
      for(int i=0; i<length; i++)
         _samples[i] = samples[i];
   }
   _repeatStart = repeatStart;
   _repeatEnd = repeatEnd;
   _sampleLength = length;
}

SampledInstrument::~SampledInstrument() {
   if (_samples) delete [] _samples;
}

void SampledInstrument::BasePitch(float basePitch, float baseSampleRate) {
   _basePitch=basePitch;  _baseSampleRate=baseSampleRate;
}
SampledNote::SampledNote(SampledInstrument *instr) {
   _instrument = instr;
   _requestPitch = instr->_basePitch;
   _requestSampleRate = instr->_baseSampleRate;
};

SampledNote::SampledNote(SampledInstrument *instr,
                         float pitch, float volume) {
   _instrument = instr;
   _requestPitch = instr->_basePitch;
   _requestSampleRate = instr->_baseSampleRate;
   Pitch(pitch);
   Volume(volume);
   Restart();
};
void SampledNote::Restart() {
   _repeating = true;
   _currentSample = _instrument->_samples;
   _endData = _instrument->_samples + _instrument->_sampleLength;
   _startLoop = _instrument->_samples + _instrument->_repeatStart;
   _endLoop = _instrument->_samples + _instrument->_repeatEnd;
   _fraction = 0;
}
size_t SampledNote::AddSamples(AudioSample *buffer, size_t samplesRequested) {
   int samplesRemaining = samplesRequested;
   if (!_currentSample) return 0;  // No data?
   while(samplesRemaining) {
      if (_repeating && (_currentSample >= _endLoop)) {
         if (_startLoop == _endLoop) // No loop
            _repeating = false;  // Don't repeat
         else
            _currentSample -= _endLoop - _startLoop;
      }

      if (!_repeating && (_currentSample >= _endData))
            return samplesRequested - samplesRemaining;

      // This assumes that 'long' is larger than the
      // largest sample * (1<<volumeBits)
      long newSample = (*_currentSample) * static_cast<long>(_volume);
      newSample >>= volumeBits;

      *buffer++ += newSample;
      _fraction += _increment;
      _currentSample += _fraction >> fractionBits; // 8-bit fraction
      _fraction &= (1<<fractionBits)-1;
      samplesRemaining--;
   }
   return samplesRequested - samplesRemaining;
};
void SampledNote::SetIncrement() {
   _increment = int(_requestPitch/_instrument->_basePitch
              * _instrument->_baseSampleRate/_instrument->SamplingRate()
              * (1<<fractionBits));
}
void SampledNote::SetSampleOffset(int offset) {
   _currentSample = _instrument->_samples + offset;
   while (_currentSample >= _endData) {
      if (_startLoop == _endLoop) {
         _currentSample = 0;
         return;
      }
      _currentSample = _startLoop + (_currentSample - _endData);
      _endData = _endLoop;
   }
};
void SineWaveInstrument::CreateInstrument() {
   if(_sampledInstrument) return;
   
const float maxAmplitude 
   = static_cast<float>((1L<<(8*sizeof(AudioSample)-1))-1);
const int numSamples = 44000;
AudioSample *buffer= new AudioSample[numSamples];
for(int i=0;i<numSamples;i++) {
   float amplitude = 0.0; // default
   float time = i / static_cast<float>(numSamples);
   if(time < 0.05) // Attack is 0.05 second
      amplitude = time / 0.05; // increase to one
   else if(time < 0.25) // Decay is 0.2 second
      amplitude = 1.0 - ((time-0.05) / 0.2 * 0.75); // Decay to 0.25
   else if(time < 0.5) // Sustain period (0.25 second in template)
      amplitude = 0.25;
   else if(time < 1.0) // decay for 0.5 second
      amplitude = 0.25 - ((time-0.5) / 0.5 * 0.25);

   buffer[i] = static_cast<AudioSample>(
      maxAmplitude
      * amplitude
      * sin(time * ( 440.0 * 2.0 * 3.14159265358979 ) )
      );
}
_sampledInstrument 
   = new SampledInstrument(buffer,numSamples,numSamples/4,numSamples/2);
_sampledInstrument->BasePitch(440.0,static_cast<float>(numSamples));
delete [] buffer;
}
