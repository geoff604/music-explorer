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
#include "iff.h"
#include "compress.h"
#include "dpcm.h"
#include <cstdlib>

#define ChunkName(a,b,c,d) (                 \
    ((static_cast<unsigned long>(a)&255)<<24)          \
  + ((static_cast<unsigned long>(b)&255)<<16)          \
  + ((static_cast<unsigned long>(c)&255)<<8)           \
  + ((static_cast<unsigned long>(d)&255)))

bool IsIffFile(istream &file) {
   file.seekg(0); // Seek to beginning of file
   unsigned long form = ReadIntMsb(file,4);
   if (form != ChunkName('F','O','R','M'))
      return false; // Not IFF file
   SkipBytes(file,4);  // Skip chunk size
   unsigned long type = ReadIntMsb(file,4);
   if (type == ChunkName('8','S','V','X'))
      return true;
   return false; // IFF file, but not 8SVX file
}

IffRead::IffRead(istream & s): _stream(s) {
   cerr << "File Format: Electronic Arts' IFF/8SVX\n";
   _decoder = 0;
   _formatData = 0;
   _formatDataLength = 0;
   
   _currentChunk = -1; // Empty the stack
   NextChunk(); // Read first chunk
   // Ensure first chunk is a FORM/8SVX container
   if (  (_currentChunk != 0)
      || (_chunk[0].type != ChunkName('F','O','R','M'))
      || (_chunk[0].isContainer != true)
      || (_chunk[0].containerType != ChunkName('8','S','V','X'))
      )
   {
      cerr << "Outermost chunk in IFF file isn't FORM/8SVX!!";
      exit(1);
   }
}

IffRead::~IffRead() {
   if(_formatData) { delete [] _formatData; }
   if(_decoder) delete _decoder;
}

void IffRead::PopStack(void) {
   
if ((_currentChunk >= 0) && (!_chunk[_currentChunk].isContainer)) {
   unsigned long lastChunkSize = _chunk[_currentChunk].size;
   if (lastChunkSize & 1) {  // Is there padding?
      _chunk[_currentChunk].remaining++;
      lastChunkSize++; // Account for padding in the container update
   }
   SkipBytes(_stream,_chunk[_currentChunk].remaining); // Flush the chunk
   _currentChunk--;  // Drop chunk from the stack
   // Sanity check: containing chunk must be container
   if ((_currentChunk < 0) || (!_chunk[_currentChunk].isContainer)) {
      cerr << "Chunk contained in non-Container?!?!\n";
      exit(1);
   }
   // Reduce size of container
   if (_currentChunk >= 0) {
      // Sanity check: make sure container is big enough.
      // Also, avoid a really nasty underflow situation.
      if ((lastChunkSize+8) > _chunk[_currentChunk].remaining) {
         cerr << "Error: Chunk is too large to fit in container!?!?\n";
         _chunk[_currentChunk].remaining = 0; // container is empty
      } else
         _chunk[_currentChunk].remaining -= lastChunkSize + 8;
   }
}
   
// There may be forms that are finished, drop them too
while (  (_currentChunk >= 0)  // there is a chunk
      &&  (_chunk[_currentChunk].remaining < 8)
      )
{
   SkipBytes(_stream,_chunk[_currentChunk].remaining); // Flush it
   unsigned long lastChunkSize = _chunk[_currentChunk].size;
   _currentChunk--;  // Drop container chunk
   // Sanity check, containing chunk must be container
   if (!_chunk[_currentChunk].isContainer) {
      cerr << "Chunk contained in non-container?!?!\n";
      exit(1);
   }
   // Reduce size of container
   if (_currentChunk >= 0) {
      if ((lastChunkSize+8) > _chunk[_currentChunk].remaining) {
         cerr << "Error in WAVE file: Chunk is too large to fit!?!?\n";
         lastChunkSize = _chunk[_currentChunk].remaining;
      }
      _chunk[_currentChunk].remaining -= lastChunkSize + 8;
   }
}
}

bool IffRead::ReadIff8svxChunk(unsigned long type, unsigned long size) {
   
if ((_currentChunk >= 0) && (_chunk[0].type != ChunkName('F','O','R','M'))){
   cerr << "Outermost chunk is not FORM ?!?!\n";
   _currentChunk = -1;
   return false;
}
if (type == ChunkName('F','O','R','M')) {
   _chunk[_currentChunk].isContainer = true;
   // Need to check size of container first.
   _chunk[_currentChunk].containerType = ReadIntMsb(_stream,4);
   _chunk[_currentChunk].remaining -= 4;
   if (_currentChunk > 0) {
      cerr << "FORM chunk seen at inner level?!?!\n";
   }
   return true;
}
if (type == ChunkName('V','H','D','R')) {
   if (_currentChunk != 1) {
      cerr << "VHDR chunk seen at wrong level?!?!\n";
   }
   _formatData = new unsigned char[size+2];
   _stream.read(reinterpret_cast<char *>(_formatData),size);
   _formatDataLength = _stream.gcount();
   _chunk[_currentChunk].remaining = 0;
   return true;
}
if (type == ChunkName('B','O','D','Y')) {
   if (_formatData) {
      unsigned long vhdrSamples = BytesToIntMsb(_formatData+0,4);
      unsigned long bodyLength = _chunk[_currentChunk].size;

      // Do they make any sense?
      if ((bodyLength == 0) && (vhdrSamples == 0)) {
         cerr << "IFF/8SVX file says it has no data!?\n";
         cerr << "I'll try to play something anyway, here goes...\n";
         _chunk[_currentChunk].size = 1000000L;
      } else if (bodyLength == 0) {
         cerr << "IFF/8SVX file has samples, but the body has no data!?\n";
         cerr << "Maybe the body is damaged, I'll try anyway...\n";
         _chunk[_currentChunk].size = vhdrSamples;
      } else if (vhdrSamples == 0) {
         cerr << "IFF/8SVX file has data in the body, but no samples?!\n";
         cerr << "Maybe the header is damaged, I'll try anyway...\n";
      }
   }
   return true;
}
   return false;
}

bool IffRead::ReadIffGenericChunk(unsigned long type, unsigned long size) {
   
if (type == ChunkName('A','N','N','O')) { // Comment
   DumpTextChunk(size,"Annotation:");
   return true;
}
if (type == ChunkName('(','c',')',' ')) { // Copyright
   DumpTextChunk(size,"Copyright:");
   return true;
}
if (type == ChunkName('N','A','M','E')) { // Name of work
   DumpTextChunk(size,"Name:");
   return true;
}
if (type == ChunkName('A','U','T','H')) { // Author
   DumpTextChunk(size,"Author:");
   return true;
}
   return false;
}

void IffRead::NextChunk(void) {
   PopStack();

   // Read the next chunk
   if (_stream.eof()) {
      _currentChunk = -1; // empty the stack
      return;
   }
   unsigned long type = ReadIntMsb(_stream,4);
   unsigned long size = ReadIntMsb(_stream,4);
   if (_stream.eof()) {
      _currentChunk = -1; // empty the stack
      return;
   }

   _currentChunk++;
   _chunk[_currentChunk].type = type;
   _chunk[_currentChunk].size = size;
   _chunk[_currentChunk].remaining = size;
   _chunk[_currentChunk].isContainer = false;
   _chunk[_currentChunk].containerType = 0;

   if (ReadIff8svxChunk(type,size)) return;
   if (ReadIffGenericChunk(type,size)) return;

   char code[5] = "CODE";
   code[0] = (type>>24)&255;   code[1] = (type>>16)&255;
   code[2] = (type>>8 )&255;   code[3] = (type    )&255;
   cerr << "Ignoring unrecognized `" << code << "' chunk\n";
}
void IffRead::MinMaxSamplingRate(long *min, long *max, long *preferred) {
   if (!_decoder) InitializeDecompression();
   long samplingRate = BytesToIntMsb(_formatData+12,2);
   *min = *max = *preferred = samplingRate;
}

size_t IffRead::GetSamples(AudioSample *buffer, size_t numSamples) {
   if (!_decoder) InitializeDecompression();
   return _decoder->GetSamples(buffer,numSamples);
}
void IffRead::MinMaxChannels(int *min, int *max, int *preferred) {
   *min = *max = *preferred = 1;
}
void IffRead::InitializeDecompression() {
   if (_decoder) return;

   while (!_formatData) {    // Read the VHDR chunk
      NextChunk();
      if (_currentChunk < 0) {
         cerr << "No `VHDR' chunk found?!?!\n";
         exit(1);
      }
   }

   // Select decompressor based on compression type
   unsigned long type = BytesToIntMsb(_formatData+15 , 1);
   
if (type == 0) {  // PCM format
   _decoder = new DecompressPcm8Signed(*this);
}
if (type == 1) {  // Fibonacci DPCM
   _decoder = new DecompressDpcmFibonacci(*this);
}
if (type == 2) {  // Exponential
   _decoder = new DecompressDpcmExponential(*this);
}
   if (!_decoder) {
      cerr << "I don't support IFF/8SVX compression type " << type << "\n";
      exit(1);
   }
}
size_t IffRead::ReadBytes(AudioByte *buffer, size_t numBytes) {
   while (_chunk[_currentChunk].type != ChunkName('B','O','D','Y')) {
      NextChunk();
      if (_currentChunk < 0) {
         cerr << "I didn't find any sound data!?!?\n";
         return 0;
      }
   }
   if (numBytes > _chunk[_currentChunk].remaining)
      numBytes = _chunk[_currentChunk].remaining;
   _stream.read(reinterpret_cast<char *>(buffer), numBytes);
   numBytes = _stream.gcount();
   _chunk[_currentChunk].remaining -= numBytes;
   return numBytes;
}
void IffRead::DumpTextChunk(unsigned long size, const char *name) {
   char *text = new char[size+2];
   _stream.read(text,size);
   long length = _stream.gcount();
   _chunk[_currentChunk].remaining -= length;
   text[length] = 0;
   cerr << name << " " << text << "\n";
   delete [] text;
}
