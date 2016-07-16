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
#include "instrumt.h"
#include "plucked.h"

#include <cstdlib> // rand, srand
#include <ctime> // time(), for seeding srand
#include <cmath> // sqrt

PluckedStringNote::PluckedStringNote(PluckedStringInstrument *instr,
      float pitch, float volume) {
  _instr = instr;
  Pitch(pitch);
  Volume(volume);
  Restart();
}

PluckedStringNote::~PluckedStringNote() {
  delete [] _buffer;
  delete [] _future;
}
// Only invoke srand() once
static bool randomInitialized = false;

void PluckedStringNote::Restart() {
   if (!randomInitialized) {
      srand(time(0)); // Seed the random number generator
      randomInitialized = true; // Don't do it again
   }

   _bufferSize = static_cast<long>(_instr->SamplingRate() / _pitch);

   // Can only play up to half the sampling rate!
   if (_bufferSize < 2) {
      _bufferSize = 1;
      _iterations = 1;
   } else {
      // First approximation: Update buffer every 1/100 second
      _iterations = _instr->SamplingRate()/100/_bufferSize;
      // Second approximation: Square that
      _iterations *= _iterations;
   }

   if (_iterations < 1) _iterations = 1;
   _remaining = 1;
   _pos = 0;
   _decayRate = 0.0;

   
{  // Create and fill the buffer with random values
   _buffer = new long[_bufferSize];
   for(int i=0;i<_bufferSize;i++) {
      AudioSample s = (rand() - (RAND_MAX/2) - 1) 
            >> (sizeof(RAND_MAX)*8-sizeof(AudioSample)*8);
      _buffer[i] = s;
   }
}
   
long maxSample = 0;
{  // Use volume to pre-filter data.
   float s1 = 0.5 + _volume/2.0;
   float s2 = 0.5 - _volume/2.0;
   long lastSample = _buffer[_bufferSize-1];
   for(int i=0;i<_bufferSize;i++) {
      long thisSample = _buffer[i];
      _buffer[i] = static_cast<long>(thisSample * s1 + lastSample * s2);
      lastSample = thisSample;
      if (labs(_buffer[i])>maxSample) maxSample = labs(_buffer[i]);
   }
}
   
long average = 0;
{
   float volumeScale = _volume * ((1<<(sizeof(AudioSample)*8-1))-1)
                      /maxSample;
   for(int i=0;i<_bufferSize;i++) {
      _buffer[i] = static_cast<long>(_buffer[i] * volumeScale);
      average += _buffer[i];
   }
   average /= _bufferSize;
}
   
{
   for(int i=0;i<_bufferSize;i++)
      _buffer[i] -= average;
}
   
{
   _future = new long[_bufferSize];
   for(int i=0;i<_bufferSize;i++)
      _future[i] = _buffer[i];
}
}
size_t PluckedStringNote::AddSamples(AudioSample *buffer,
      size_t samplesRequested) {
   int samplesRemaining = samplesRequested;
   while(samplesRemaining > 0) {
      // Blend smoothly between _buffer and _future
      long blendedSample =
            (_buffer[_pos] * _remaining 
            + _future[_pos] * (_iterations - _remaining)
            )/_iterations;

      *buffer++ += blendedSample; // Play the sample
      samplesRemaining--;

      if(++_pos>=_bufferSize) { // Reached end of _buffer
         _pos = 0;  // reset to beginning
         if (--_remaining == 0) { // Time to re-process our data?
            long *t = _buffer;  // Swap buffers
            _buffer = _future;
            _future = t;

            // Filter _buffer into _future
            long lastSample = _buffer[_bufferSize-1];
            long average = 0;
            // Make divisor larger to decay faster
            long divisor = 1024 * (1 << static_cast<int>(10 * _decayRate));
            int i;
            for (i=0;i<_bufferSize;i++) {
               _future[i] = (_buffer[i]*512 + lastSample*512)/divisor;
               lastSample = _buffer[i];
               average += _future[i];
            }

            // Re-normalize _future and see if the note has faded out yet
            average /= _bufferSize;
            long total = 0;
            for(i=0;i<_bufferSize;i++) {
               _future[i] -= average;
               total += labs(_future[i]); // Accumulate total amplitude
            }

            // If nothing left, return now
            if (total == 0) return (samplesRequested - samplesRemaining);

            _remaining = _iterations; // Reset delay until next update
         }
      }
   }
   return (samplesRequested - samplesRemaining);
}
