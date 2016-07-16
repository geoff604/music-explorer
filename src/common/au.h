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
#ifndef AU_H_INCLUDED
#define AU_H_INCLUDED
#include "audio.h"
#include "compress.h"
#include "aplayer.h"
#include <iostream>
#include "soundexception.h"

bool IsAuFile(istream &file);

class AuRead: public AudioAbstract {
   
private:
   istream &_stream;
   AbstractDecompressor *_decoder;
public:
   AuRead(istream &input = cin):AudioAbstract(),_stream(input) {
      TRACE0( "File Format: Sun AU (also known as NeXT SND)" );
      _headerRead = false; // Haven't read header yet
      _decoder = 0;
   }
   ~AuRead() {
      if (_decoder) delete _decoder;
   }
   size_t GetSamples(AudioSample *buffer, size_t numSamples) {
      return _decoder->GetSamples(buffer,numSamples);
   }
private:
   size_t _dataLength;
public:
   size_t ReadBytes(AudioByte *buffer, size_t length);
private:
   bool _headerRead; // true if header has already been read
   int _headerChannels; // channels from header
   int _headerRate; // sampling rate from header
   void ReadHeader(void);
protected:
   void MinMaxSamplingRate(long *min, long *max, long *preferred) {
      ReadHeader();
      *min = *max = *preferred = _headerRate;
   }
   void MinMaxChannels(int *min, int *max, int *preferred) {
      ReadHeader();
      *min = *max = *preferred = _headerChannels;
   }
};

class AuWrite: public AbstractPlayer {
   
private:
   ostream &_stream;
public:
   AuWrite(AudioAbstract *audio, ostream &output = cout)
      :AbstractPlayer(audio),_stream(output) {
   };
public:
   void Play(void);
};
#endif
