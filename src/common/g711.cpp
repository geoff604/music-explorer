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

/* The number of bits required by each value */
static unsigned char numBits[] = {
   0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

/* Mu-Law conversions */
static bool muLawDecodeTableInitialized = false;
static AudioSample muLawDecodeTable[256];

// Constructor initializes the decoding table
DecompressG711MuLaw::DecompressG711MuLaw(AudioAbstract &a)
      : AbstractDecompressor(a) {
   TRACE0( "Encoding: ITU G.711 mu-Law" );
   if (!muLawDecodeTableInitialized) {
      muLawDecodeTableInitialized = true;
      for(int i=0;i<256;i++)
         muLawDecodeTable[i] = MuLawDecode(i);
   }
}
size_t DecompressG711MuLaw::GetSamples(AudioSample *buffer,
                                       size_t length) {
   AudioByte *byteBuff = 
      reinterpret_cast<AudioByte *>(buffer);
   size_t read = ReadBytes(byteBuff,length);
   for(long i=read-1; i>=0; i--)
      buffer[i] = muLawDecodeTable[ byteBuff[i] ];
   return read;
};
AudioByte MuLawEncode(AudioSample s) {
   unsigned char sign = (s<0)?0:0x80; // Save the sign
   if (s<0) s=-s; // make sample positive
   signed long adjusted = static_cast<long>(s) << (16-sizeof(AudioSample)*8);
   adjusted += 128L+4L;
   if (adjusted > 32767) adjusted = 32767;
   unsigned char exponent = numBits[(adjusted>>7)&0xFF] - 1;
   unsigned char mantissa = (adjusted >> (exponent + 3)) & 0xF;
   return ~(sign | (exponent << 4) | mantissa);
};
AudioSample MuLawDecode(AudioByte ulaw) {
   ulaw = ~ulaw;
   unsigned char exponent = (ulaw >> 4) & 0x7;
   unsigned char mantissa = (ulaw & 0xF) + 16;
   unsigned long adjusted = (mantissa << (exponent + 3)) - 128 - 4;
   return (ulaw & 0x80)? adjusted : -adjusted;
};
static bool aLawDecodeTableInitialized = false;
static AudioSample aLawDecodeTable[256];

DecompressG711ALaw::DecompressG711ALaw(AudioAbstract &a)
      : AbstractDecompressor(a) {
   TRACE0( "Encoding: ITU G.711 A-Law" );
   if (!aLawDecodeTableInitialized) {
      aLawDecodeTableInitialized = true;
      for(int i=0;i<256;i++)
         aLawDecodeTable[i] = ALawDecode(i);
   }
}

size_t DecompressG711ALaw::GetSamples(AudioSample *buffer, size_t length) {
   AudioByte *byteBuff = 
      reinterpret_cast<AudioByte *>(buffer);
   size_t read = ReadBytes(byteBuff,length);
   for(long i=read-1; i>=0; i--)
      buffer[i] = aLawDecodeTable[ byteBuff[i] ];
   return read;
}

AudioByte ALawEncode(AudioSample s) {
   unsigned char sign = (s<0)?0:0x80; // Save the sign
   if (s<0) s=-s; // make sample positive
   signed long adjusted = static_cast<long>(s)+8L; // Round it
   if (adjusted > 32767) adjusted = 32767; // Clip it
   unsigned char exponent = numBits[(adjusted>>8)&0x7F];
   unsigned char mantissa = (adjusted >> (exponent + 4)) & 0xF;
   return sign | (((exponent << 4) | mantissa) ^ 0x55);
};

AudioSample ALawDecode(AudioByte alaw) {
   alaw ^= 0x55;
   unsigned char exponent = (alaw >> 4) & 0x7;
   unsigned char mantissa = (alaw & 0xF) + (exponent?16:0);
   unsigned long adjusted = (mantissa << (exponent + 4));
   return (alaw & 0x80)? -adjusted : adjusted;
};
