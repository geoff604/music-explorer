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
#ifndef MPEG_H_INCLUDED
#define MPEG_H_INCLUDED
#include "audio.h"
#include "compress.h"

class DecompressMpeg: public AbstractDecompressor {
private:
   
private:
   void FillBuffer(); // Keep byte buffer full
   void ResetBits(); // Reset bit extraction
   long  GetBits(int numBits); // Extract bits

   AudioByte _bufferStorage[2048];
   AudioByte *_buffer; // Beginning of live data in buffer
   int       _bitsRemaining; // bits left in top byte
   AudioByte *_bufferEnd; // End of live data in buffer
   AudioByte *_header; // Location of header in buffer
   
private:
   bool ParseHeader(); // Parse header for next frame

   char _id; // 1 for MPEG-1, 0 for MPEG-2 extensions
   char _layer;
   char _protection; // 1 if CRC check omitted
   long _bitRate; // total bits/second
   long _samplingRate; // samples/second
   bool _padding; // this packet has an extra slot
   char _private; // private bit
   char _mode;   // single-channel, dual-channel, stereo, etc.
   char _modeExtension; // Type of stereo encoding
   char _bound; // subband where stereo encoding changes
   bool _copyright; // true if copyrighted
   bool _original; // true if original
   char _emphasis; // How to post-process audio
   int _channels; // Number channels
   int _headerSpacing; // in bytes
   
private:
   long *_V[2][16];  // Synthesis window for left/right channel
   void Layer12Synthesis(long *V[16], long *in, int inSamples,
                            AudioSample *out);
   void Layer1Decode(); // Decompress layer 1 data
   void Layer2Decode(); // Decompress layer 2 data
   
private:
   void Layer3Decode();

   AudioSample _sampleStorage[2][1152];
   AudioSample *_pcmSamples[2]; // Samples for left/right channels
   int _samplesRemaining; // Samples remaining from last frame

   void NextFrame(); // Read and decompress next frame
public:
   DecompressMpeg(AudioAbstract &a);
   ~DecompressMpeg();
   size_t GetSamples(AudioSample *outBuff, size_t len);
   void MinMaxSamplingRate(long *min, long *max, long *preferred) {
      *min=*max=*preferred=_samplingRate;
   };
   void MinMaxChannels(int *min, int *max, int *preferred) {
      *min=*max=*preferred=_channels;
   };
};

bool IsMpegFile(istream &file);

class MpegRead: public AudioAbstract {
private:
   istream &_stream;
   AbstractDecompressor *_decoder;
public:
   MpegRead(istream &input = cin);
   ~MpegRead();
   size_t GetSamples(AudioSample *buffer, size_t numSamples);
   size_t ReadBytes(AudioByte *buffer, size_t length);
   void MinMaxSamplingRate(long *min, long *max, long *preferred);
   void MinMaxChannels(int *min, int *max, int *preferred);
};
#endif
