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
#include "au.h"
#include "g711.h"

bool IsAuFile(istream &file) {
   file.seekg(0);  // Seek to beginning
   long magic = ReadIntMsb(file,4);
   return (magic == 0x2E736E64); // Should be `.snd'
}
size_t AuRead::ReadBytes(AudioByte *buffer, size_t length) {
   if (length > _dataLength) { length = _dataLength; }
   _stream.read(reinterpret_cast<char *>(buffer),length);
   size_t lengthRead = _stream.gcount();
   _dataLength -= lengthRead;
   return lengthRead;
}
void AuRead::ReadHeader(void) {
   if (_headerRead) return;
   _headerRead = true;

   char header[24];
   _stream.read(header,24);

   long magic = BytesToIntMsb(header+0,4);
   if (magic != 0x2E736E64) { // '.snd'
	  throw new CSoundException( CSoundException::causeInputFileNotAU);
   }

   long headerLength = BytesToIntMsb(header+4,4);
   _dataLength = BytesToIntMsb(header+8,4);
   int format = BytesToIntMsb(header+12,4);
   _headerRate = BytesToIntMsb(header+16,4);
   _headerChannels = BytesToIntMsb(header+20,4);
   SkipBytes(_stream,headerLength - 24); // Junk rest of header

   // Create an appropriate decompression object
   switch(format) {
   case 1: // ITU G.711 mu-Law
      _decoder = new DecompressG711MuLaw(*this);
      break;
   case 2: // 8-bit linear
      _decoder = new DecompressPcm8Unsigned(*this);
      break;
   case 3: // 16-bit linear
      _decoder = new DecompressPcm16MsbSigned(*this);
      break;
   default:
      TRACE1( "AU format %d not supported.", format );
	  throw new CSoundException( CSoundException::causeAUFormatNotSupported);
   }

   TRACE1( "Sampling Rate: %d", _headerRate );
   TRACE1( "Channels: %d", _headerChannels );
 
}
static void WriteBuffer
           (ostream &out, AudioSample *buffer, int length) {
   AudioSample *sampleBuff = buffer;
   AudioByte *byteBuff = 
         reinterpret_cast<AudioByte *>(buffer);
   int i = length;
   while (i-->0) {
      int sample = *sampleBuff++;
      *byteBuff++ = sample >> 8;
      *byteBuff++ = sample;
   }
   out.write(reinterpret_cast<char *>(buffer),length*2);
};

void AuWrite::Play(void) {
   int samplingRate = SamplingRate();
   int channels = Channels();

   // Write AU header
   _stream.write(".snd",4); // Magic
   WriteIntMsb(_stream,28L,4); // Length of header
   WriteIntMsb(_stream,0x7FFFFFFFL,4); // length of data
   WriteIntMsb(_stream,3L,4); // 16-bit linear
   WriteIntMsb(_stream,samplingRate,4); // Sampling rate
   WriteIntMsb(_stream,channels,4); // number of channels
   WriteIntMsb(_stream,0,4); // padding

   // Read sound data and write it to the file
   long length;
   do {
      const int BuffSize = 10240;
      AudioSample buff[BuffSize];
      length = Previous()->GetSamples(buff,BuffSize);
      WriteBuffer(_stream,buff,length);
   } while (length > 0);
}
