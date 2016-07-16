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
#include "compress.h"
#include "dpcm.h"

static const signed char fibonacci[] = {
   -34, -21, -13, -8, -5, -3, -2, -1, 0, 1, 2, 3, 5, 8, 13, 21
};

static const signed char exponential[] = {
   -128, -64, -32, -16, -8, -4, -2, -1, 0, 1, 2, 4, 8, 16, 32, 64
};

size_t DecompressDpcmFibonacci::GetSamples(AudioSample * buffer,
                                  size_t length) {
   AudioByte *byteBuffer = 
         reinterpret_cast<AudioByte *>(buffer)
         + length * sizeof(AudioSample) // buffer length
         - length / 2 ; // Space needed for compressed data
   AudioSample *sampleBuffer = buffer;

   // Read encoded data into the buffer
   size_t bytesRead = ReadBytes(byteBuffer,length/2);
   for(size_t i=0; i<bytesRead; i++) {
      // Decode low-order nybble
      int nybble = (static_cast<int>(*byteBuffer>>4)+8)&0xF;
      _previousValue +=
               static_cast<AudioSample>(fibonacci[nybble])
               << ((sizeof(AudioSample)-1)*8);
      *sampleBuffer++ = _previousValue;
      // Decode high-order nybble
      nybble = (static_cast<int>(*byteBuffer)+8)&0xF;
      _previousValue +=
               static_cast<AudioSample>(fibonacci[nybble])
               << ((sizeof(AudioSample)-1)*8);
      *sampleBuffer++ = _previousValue;
   }
   return bytesRead * 2;
}
size_t DecompressDpcmExponential::GetSamples(AudioSample * buffer,
                                  size_t length) {
   AudioByte *byteBuffer = 
         reinterpret_cast<AudioByte *>(buffer)
         + length * sizeof(AudioSample) // buffer length
         - length / 2 ; // Space needed for compressed data
   AudioSample *sampleBuffer = buffer;

   // Read encoded data into the buffer
   size_t bytesRead = ReadBytes(byteBuffer,length/2);
   for(size_t i=0; i<bytesRead; i++) {
      // Decode low-order nybble
      int nybble = (static_cast<int>(*byteBuffer>>4)+8)&0xF;
      _previousValue +=
               static_cast<AudioSample>(exponential[nybble])
               << ((sizeof(AudioSample)-1)*8);
      *sampleBuffer++ = _previousValue;
      // Decode high-order nybble
      nybble = (static_cast<int>(*byteBuffer)+8)&0xF;
      _previousValue +=
               static_cast<AudioSample>(exponential[nybble])
               << ((sizeof(AudioSample)-1)*8);
      *sampleBuffer++ = _previousValue;
   }
   return bytesRead * 2;
}
