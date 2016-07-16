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
#include "fft.h"
#include <cmath>
#include <iostream>

const double PI = 3.14159265358979323846264338;

void ForwardDft(complex <double> *samples,
                int length, complex<double>*result) {
   static const double
         twoPi = 2*3.1415926535897932384626;
   for(int f = 0; f<length; f++) {
      result[f] = 0.0;
      for(int t = 0; t < length; t++)
         result[f] += samples[t] * polar(1.0,-twoPi*f*t/length);
   }
}
static void SlowFftRecursion(complex<double> *samples, int length,
                   int start, int skip, complex<double> *result) {

   if (length == 1) {  // 1-point FFT is easy!
      *result = samples[start];
      return;
   }

   // Compute two half-size FFTs
   SlowFftRecursion(samples,length/2,start,skip*2,result);
   SlowFftRecursion(samples,length/2,start+skip,skip*2,result+length/2);

   // Compute sums and differences of pairs
   for(int j=0; j<length/2; j++) {
      // Multiply the second piece by a phase-shift coefficient
      complex<double> t = result[j+length/2] * polar(1.0,-2*PI*j/length);
      // subtract
      result[j + length/2] = result[j] - t;
      // add
      result[j] += t;
   }
}

void SlowForwardFft(complex<double> *samples, int length,
             complex<double>*result) {
   SlowFftRecursion(samples,length,0,1,result);
}
static void Rearrange(complex<double> *samples, int length) {
   static int rearrangeSize = 0; // size of rearrange table
   static int *rearrange = 0;

   if (rearrangeSize != length) {
      // Recompute whenever size changes
      if (rearrange) delete [] rearrange;
      rearrange = new int[length];

      // Fill in destination of each value
      rearrange[0] = 0;
      for(int limit=1, bit=length/2; limit<length; limit <<= 1, bit>>=1 )
         for(int i=0;i<limit;i++)
            rearrange[i+limit] = rearrange[i] + bit;

      // Put zero in any spot that stays the same
      // Also zero one side of each swap,
      // so each swap only happens once
      for(int i=0; i<length; i++) {
         if (rearrange[i] == i) rearrange[i] = 0;
         else rearrange[ rearrange[i] ] = 0;
      }
      rearrangeSize = length;
   }

   // Use the rearrange table to swap elements
   // zero indexes are simply skipped
   complex <double> t;
   for (int i=0; i<length; i++)
      if (rearrange[i]) { // Does this element get exchanged?
         t = samples[i];  // Yes, do the exchange
         samples[i] = samples[ rearrange[i] ];
         samples[ rearrange[i] ] = t;
      }
}

void ForwardFft(complex<double> *samples, int length ) {
   Rearrange(samples,length);

   for(int halfSize=1; halfSize < length; halfSize *= 2) {
      complex<double> phaseShiftStep = polar(1.0,-PI/halfSize);
      complex<double> currentPhaseShift(1,0);
      for(int fftStep = 0; fftStep < halfSize; fftStep++) {
         for(int i=fftStep; i < length; i += 2*halfSize) {
            complex<double> t = currentPhaseShift * samples[i+halfSize];
            samples[i+halfSize] = samples[i] - t;
            samples[i] += t;
         }
         currentPhaseShift *= phaseShiftStep;
      }
   }
}

//
// Using some simple facts about complex numbers, you
// can compute the Inverse FFT by conjugating the samples
// before and after the Forward FFT computation.
// I also scale by 1/N here.
//
void InverseFft(complex<double> *samples, int length ) {
   for(int i=0; i<length; i++)
      samples[i] = conj(samples[i]);
   ForwardFft(samples,length);
   for(int i=0; i<length; i++)
      samples[i] = conj(samples[i]) / static_cast<double>(length);
}
