
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
#include "g711.h"
#include <math.h>

void SNR(AudioSample magnitude) {
   // Build a one-cycle sine wave
   float original[1000];
   for(int i=0;i<1000;i++)
      original[i] = sin(i*2.0*3.14159265/1000.0)*magnitude;

   // Compress it
   AudioByte compressed[1000];
   for(int i=0;i<1000;i++)
      compressed[i] = MuLawEncode(static_cast<AudioSample>(original[i]));

   // Uncompress it
   AudioSample uncompressed[1000];
   for(int i=0;i<1000;i++)
      uncompressed[i] = MuLawDecode(compressed[i]);

   // Figure the error
   float error[1000];
   for(int i=0;i<1000;i++)
      error[i] = original[i] - uncompressed[i];

   // Compute total power
   float originalPower = 0.0,  errorPower = 0.0, pcmErrorPower = 0.0;
   for(int i=0;i<1000;i++) {
      originalPower += original[i] * original[i];
      errorPower += error[i] * error[i];
      float pcmError = original[i] - static_cast<AudioSample>(original[i]);
      pcmErrorPower += pcmError * pcmError;
   }
   // output magnitude and SNR
   cout << magnitude;
   cout << " " << 10*log10(originalPower/errorPower);
   cout << " " << 10*log10(originalPower/pcmErrorPower);
   cout << "\n";
}

int main() {
   int magnitude = 100;
   for (; magnitude < 32000; magnitude += 100)
      SNR(magnitude);
   return 0;
}
