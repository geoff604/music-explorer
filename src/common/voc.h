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
#ifndef VOC_H_INCLUDED
#define VOC_H_INCLUDED
#include "audio.h"
#include "compress.h"
#include <iostream>

bool IsVocFile(istream &file);

class VocRead: public AudioAbstract {
   
private:
   void ReadHeader(void);
private:
   void GetBlock(void);
   int _blockType;  // Type of current block
private:
   unsigned long   bytesRemaining;  // bytes left in this block
   void ReadBlock1(long blockLength);
private:
   int repeatDepth;  // Number of nested repeats
#define maxRepeat 5
   int repeatCounts[maxRepeat];
   streampos repeatPosition[maxRepeat];
private:
   void ReadBlock8(long blockLength);
private:
   void ReadBlock9(long blockLength);
private:
   istream &_stream;
public:
   VocRead(istream &input = cin);
   ~VocRead() {
      if (_decoder) delete _decoder;
   };
private:
   int _fileChannels;
   int _fileSampleRate;
   int _fileWidth;
   int _fileCompression;
   AbstractDecompressor *_decoder;
protected:
   void MinMaxSamplingRate(long *min, long *max, long *preferred) {
      *min = *max = *preferred = _fileSampleRate;
   };
   void MinMaxChannels(int *min, int *max, int *preferred) {
      *min = *max = *preferred = _fileChannels;
   };
   size_t GetSamples(AudioSample *buffer, size_t numSamples);
public:
   size_t ReadBytes(AudioByte *buffer, size_t length);
};
#endif
