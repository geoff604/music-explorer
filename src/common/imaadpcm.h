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
#ifndef IMAADPCM_H_INCLUDED
#define IMAADPCM_H_INCLUDED
#include "audio.h"
#include "compress.h"

struct ImaState {
   int index;    // Index into step size table
   int previousValue; // Most recent sample value
};

// Decode/Encode a single sample and update state
AudioSample ImaAdpcmDecode(AudioByte deltaCode, ImaState &);
AudioByte ImaAdpcmEncode(AudioSample, ImaState &);

class DecompressImaAdpcmMs: public AbstractDecompressor {
private:
   int  _channels;
   AudioSample *_samples[2];  // Left and right sample buffers
   AudioSample *_samplePtr[2]; // Pointers to current samples
   size_t   _samplesRemaining; // Samples remaining in each channel
   size_t   _samplesPerPacket; // Total samples per packet
public:
   DecompressImaAdpcmMs(AudioAbstract &a, int packetLength, int channels);
   ~DecompressImaAdpcmMs();
   size_t GetSamples(AudioSample *outBuff, size_t len);
private:
   AudioByte *_packet;   // Temporary buffer for packets
   size_t   _bytesPerPacket; // Size of a packet
   size_t  NextPacket();
};
class DecompressImaAdpcmApple: public AbstractDecompressor {
private:
   int _channels;

   ImaState _state[2];
   AudioSample _samples[2][64];
   AudioSample *_samplePtr[2];

   size_t   _samplesRemaining;
   size_t  NextPacket(AudioSample *sampleBuffer, ImaState &state);

public:
   DecompressImaAdpcmApple(AudioAbstract &a, int channels);
   size_t GetSamples(AudioSample *outBuff, size_t len);
};
#endif
