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
#ifndef COMPR_H_INCLUDED
#define COMPR_H_INCLUDED

#include "audio.h"
#include <iostream>
#include <cstddef>

class AbstractDecompressor {
protected:
   AudioAbstract &_dataSource;  // The object to get raw bytes from
   size_t ReadBytes(AudioByte *buff, size_t length) {
      return _dataSource.ReadBytes(buff,length);
   };
public:
   AbstractDecompressor(AudioAbstract &a): _dataSource(a) {};
   virtual ~AbstractDecompressor() {};
   virtual size_t GetSamples(AudioSample *, size_t) = 0;
   virtual void MinMaxSamplingRate(long *, long *, long *) {
		throw new CSoundException( CSoundException::causeMinMaxSamplingRateUndefined );
   }
   virtual void MinMaxChannels(int *, int *, int *) {
      cerr << "MinMaxChannels undefined\n";
      exit(1);
   }
};

class DecompressPcm8Signed: public AbstractDecompressor {
public:
   DecompressPcm8Signed(AudioAbstract &a): AbstractDecompressor(a) {
      TRACE0( "Encoding: 8-bit signed (two's complement) PCM" );
   };
   size_t GetSamples(AudioSample * buffer, size_t length);
};
class DecompressPcm8Unsigned: public AbstractDecompressor {
public:
   DecompressPcm8Unsigned(AudioAbstract &a): AbstractDecompressor(a) {
      TRACE0( "Encoding: 8-bit unsigned (excess-128) PCM" );
   };
   size_t GetSamples(AudioSample * buffer, size_t length);
};
class DecompressPcm16MsbSigned: public AbstractDecompressor {
public:
   DecompressPcm16MsbSigned(AudioAbstract &a): AbstractDecompressor(a) {
		TRACE0( "Encoding: 16-bit MSB PCM" );
   };
   size_t GetSamples(AudioSample *buffer, size_t length);
};
class DecompressPcm16LsbSigned: public AbstractDecompressor {
public:
   DecompressPcm16LsbSigned(AudioAbstract &a): AbstractDecompressor(a) {
      TRACE0( "Encoding: 16-bit LSB PCM" );
   };
   size_t GetSamples(AudioSample *buffer, size_t length);
};
#endif
