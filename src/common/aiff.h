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

bool IsAiffFile(istream &file);

class AiffRead: public AudioAbstract {
private:
   istream & _stream;
   AbstractDecompressor *_decoder; // current decompressor object
   void InitializeDecompression();
   unsigned char *_formatData; // contents of COMM chunk
   unsigned long _formatDataLength; // length of COMM chunk contents
   void MinMaxSamplingRate(long *min, long *max, long *preferred);
   void MinMaxChannels(int *min, int *max, int *preferred);
public:
   AiffRead(istream & s);
   ~AiffRead();
   size_t GetSamples(AudioSample *buffer, size_t numSamples);
   size_t ReadBytes(AudioByte *buffer, size_t numSamples);

   
private:
   // chunk stack
   struct {
      unsigned long type; // Type of chunk
      unsigned long size; // Size of chunk
      unsigned long remaining; // Bytes left to read
      bool isContainer;   // true if this is a container
      unsigned long containerType; // type of container
   } _chunk[5];

   int _currentChunk; // top of stack

   void PopStack();
   bool ReadAiffSpecificChunk(unsigned long type, unsigned long size);
   void DumpTextChunk(unsigned long size, const char *);
   bool ReadIffGenericChunk(unsigned long type, unsigned long size);
   void NextChunk(void);
};
