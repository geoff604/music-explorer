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

#include "compress.h"
size_t DecompressPcm8Signed::GetSamples(AudioSample * buffer,
                                    size_t length) {
   AudioByte *byteBuff = 
      reinterpret_cast<AudioByte *>(buffer);
   size_t samplesRead = ReadBytes(byteBuff,length);
   for(long i=samplesRead-1; i>=0; i--)
      buffer[i] = static_cast<AudioSample>(byteBuff[i])
                  << ((sizeof(AudioSample)-1)*8);
   return samplesRead;
}
size_t DecompressPcm8Unsigned::GetSamples(AudioSample * buffer,
                                    size_t length) {
   AudioByte *byteBuff =
      reinterpret_cast<AudioByte *>(buffer);
   size_t samplesRead = ReadBytes(byteBuff,length);
   for(long i=samplesRead-1; i>=0; i--)
      buffer[i] = static_cast<AudioSample>(byteBuff[i] ^ 0x80)
                  << ((sizeof(AudioSample)-1)*8);
   return samplesRead;
}
size_t DecompressPcm16MsbSigned::GetSamples(AudioSample *buffer,
                                       size_t length) {
   AudioByte *byteBuff = 
      reinterpret_cast<AudioByte *>(buffer);
   size_t read = ReadBytes(byteBuff,length*2)/2;
   for(long i=read-1; i>=0; i--) {
      short s = static_cast<AudioSample>(byteBuff[2*i]) << 8;
      s |= static_cast<AudioSample>(byteBuff[2*i+1]) & 255;
      buffer[i] = static_cast<AudioSample>(s)
                  << ((sizeof(AudioSample)-2)*8);
   }
   return read;
}
size_t DecompressPcm16LsbSigned::GetSamples(AudioSample *buffer,
                                       size_t length) {
   AudioByte *byteBuff = 
      reinterpret_cast<AudioByte *>(buffer);
   size_t read = ReadBytes(byteBuff,length*2)/2;
   for(long i=read-1; i>=0; i--) {
      short s = static_cast<AudioSample>(byteBuff[2*i+1]) << 8;
      s |= static_cast<AudioSample>(byteBuff[2*i]) & 255;
      buffer[i] = static_cast<AudioSample>(s)
                  << ((sizeof(AudioSample)-2)*8);
   }
   return read;
}
