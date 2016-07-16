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

#include "wav.h"
#include "compress.h"
#include "imaadpcm.h"
#include "g711.h"
#include <cstdlib>

#define ChunkName(a,b,c,d) (                 \
    ((static_cast<unsigned long>(a)&255)<<24)          \
  + ((static_cast<unsigned long>(b)&255)<<16)          \
  + ((static_cast<unsigned long>(c)&255)<<8)           \
  + ((static_cast<unsigned long>(d)&255)))

bool IsWaveFile(istream &file) {
   file.seekg(0); // Seek to beginning of file
   unsigned long form = ReadIntMsb(file,4);
   if (form != ChunkName('R','I','F','F'))
      return false; // Not RIFF file
   SkipBytes(file,4);  // Skip chunk size
   unsigned long type = ReadIntMsb(file,4);
   if (type == ChunkName('W','A','V','E'))
      return true;
   return false; // RIFF file, but not WAVE file
}


void WaveRead::BackwardsStop()
{
	_stopped = true;
}

void WaveRead::BackwardsStart()
{
	_stopped = false;
}


void WaveRead::NextChunk(void) 
{
   
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
			throw new CSoundException( CSoundException::causeChunkContainedInNonContainer );     
		}
		// Reduce size of container
		if (_currentChunk >= 0) {
			// Sanity check: make sure container is big enough.
			// Also, avoid a really nasty underflow situation.
			if ((lastChunkSize+8) > _chunk[_currentChunk].remaining) {
				TRACE0( "Error: Chunk is too large to fit in container!?!?");
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
		  throw new CSoundException( CSoundException::causeChunkContainedInNonContainer );
		}
		// Reduce size of container
		if (_currentChunk >= 0) {
			if ((lastChunkSize+8) > _chunk[_currentChunk].remaining) {
				TRACE0( "Error in WAVE file: Chunk is too large to fit!?!?");
				lastChunkSize = _chunk[_currentChunk].remaining;
			}
			_chunk[_currentChunk].remaining -= lastChunkSize + 8;
		}
	}

		// Read the next chunk
		if (_stream.eof()) {
			_currentChunk = -1; // empty the stack
			return;
		}
		unsigned long type = ReadIntMsb(_stream,4);
		unsigned long size = ReadIntLsb(_stream,4);
		if (_stream.eof()) {
			_currentChunk = -1; // empty the stack
			return;
		}

		// Put this chunk on the stack
		_currentChunk++;
		_chunk[_currentChunk].type = type;
		_chunk[_currentChunk].size = size;
		_chunk[_currentChunk].remaining = size;
		_chunk[_currentChunk].isContainer = false;
		_chunk[_currentChunk].containerType = 0;

   
	if ((_currentChunk >= 0) && 
		 (_chunk[0].type != ChunkName('R','I','F','F'))){
		TRACE0( "Outermost chunk is not RIFF ?!?!");
		_currentChunk = -1;
		return;
	}
	if (type == ChunkName('R','I','F','F')) {
		_chunk[_currentChunk].isContainer = true;
		// Need to check size of container first.
		_chunk[_currentChunk].containerType = ReadIntMsb(_stream,4);
		_chunk[_currentChunk].remaining -= 4;
		if (_currentChunk > 0) {
			TRACE0( "RIFF chunk seen at inner level?!?!");
		}
		return;
	}
	if (type == ChunkName('f','m','t',' ')) {
		if (_currentChunk != 1) {
			TRACE0( "FMT chunk seen at wrong level?!?!");
		}
		_formatData = new unsigned char[size+2];
		_stream.read(reinterpret_cast<char *>(_formatData),size);
		_formatDataLength = _stream.gcount();
		_chunk[_currentChunk].remaining = 0;
		return;
	}
	if (type == ChunkName('d','a','t','a')) {
		return;
	}
   
	if ((type & 0xFF000000) == ChunkName('I',0,0,0)) { // First letter 'I'??
		char *text = new char[size+2];
		_stream.read(text,size);
		long length = _stream.gcount();
		_chunk[_currentChunk].remaining -= length;
		text[length] = 0;
		if (type == ChunkName('I','C','M','T')) // Comment
			TRACE0( "Comment: ");
		else if (type == ChunkName('I','C','O','P')) // Copyright notice
			TRACE0( "Copyright: ");
		else if (type == ChunkName('I','N','A','M')) // Name of work
			TRACE0( "Title: ");
		else if (type == ChunkName('I','A','R','T')) // Name of artist
			TRACE0( "Artist: ");
		else
			TRACE0( "Text: "); // Other Informational chunk
		TRACE1( "%s", text );
		return;
	}

   char code[5] = "CODE";
   code[0] = (type>>24)&255;   code[1] = (type>>16)&255;
   code[2] = (type>>8 )&255;   code[3] = (type    )&255;
   TRACE1( "Ignoring unrecognized '%s' chunk", code );
}
void WaveRead::MinMaxSamplingRate(long *min, long *max, long *preferred) {
   InitializeDecompression();
   unsigned long samplingRate = BytesToIntLsb(_formatData+4,4);
   *max = *min = *preferred = samplingRate;
}

void WaveRead::MinMaxChannels(int *min, int *max, int *preferred) {
   InitializeDecompression();
   unsigned long channels = BytesToIntLsb(_formatData+2,2);
   *min = *max = *preferred = channels;
}

size_t WaveRead::GetSamples(AudioSample *buffer, size_t numSamples) {
   if (!_decoder) InitializeDecompression();
   
	if( _stopped )
		return 0;
	else
		return _decoder->GetSamples(buffer,numSamples);
}
void WaveRead::InitializeDecompression() {
   if (_decoder) return;

   // Make sure we've read the fmt chunk
   while (!_formatData) {
      NextChunk();
      if (_currentChunk < 0) {
		throw new CSoundException( CSoundException::causeNoFmtChunkFound);
      }
   }

   // Select decompressor based on compression type
   unsigned long type = BytesToIntLsb(_formatData+0 , 2);

   
	if (type == 1) {  // PCM format
		unsigned long bitsPerSample = BytesToIntLsb(_formatData+14, 2);
		if (bitsPerSample <= 8) // Wave stores 8-bit data as unsigned
			_decoder = new DecompressPcm8Unsigned(*this);
		else if (bitsPerSample <= 16) // 16 bit data is signed
			_decoder = new DecompressPcm16LsbSigned(*this);
	}
	if (type == 17) {  // IMA ADPCM format
		unsigned long bitsPerSample = BytesToIntLsb(_formatData+14, 2);
		if (bitsPerSample != 4) {
			TRACE1( "IMA ADPCM requires 4 bits per sample, not %d", bitsPerSample );
			throw new CSoundException( CSoundException::causeADPCMRequires4BitsPerSample);
		}
		if (_formatDataLength < 20) {
			throw new CSoundException( CSoundException::causeADPCMRequiresDecompressionData);
		}
		int packetLength = BytesToIntLsb(_formatData+18,2);
		int channels = BytesToIntLsb(_formatData+2,2);
		_decoder = new DecompressImaAdpcmMs(*this,packetLength,channels);
	}
	if (type == 6) {
		_decoder = new DecompressG711ALaw(*this);
	}
	if (type == 7) {
		_decoder = new DecompressG711MuLaw(*this);
	}
	if (type == 2) {
		TRACE0( "I don't support MS ADPCM compression.");
	}

   if (!_decoder) {
      TRACE1( "I don't support WAVE compression type %d", type );
      throw new CSoundException( CSoundException::causeUnsupportedWAVCompression);
   }
}

size_t WaveRead::ReadBytes(AudioByte *buffer, size_t numBytes) 
{
   while (_chunk[_currentChunk].type != ChunkName('d','a','t','a')) {
      NextChunk();
      if (_currentChunk < 0) {
         TRACE0( "I didn't find any sound data!?!?" );
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

WaveRead::WaveRead(istream & s): _stream(s) 
{

   TRACE0( "File Format: Microsoft WAVE");
   _decoder = 0;
   _formatData = 0;
   _formatDataLength = 0;
   
	_stopped = false;

   _currentChunk = -1; // Empty the stack
   NextChunk();
   // Ensure first chunk is RIFF/WAVE container
   if (  (_currentChunk != 0)
      || (_chunk[0].type != ChunkName('R','I','F','F'))
      || (_chunk[0].isContainer != true)
      || (_chunk[0].containerType != ChunkName('W','A','V','E'))
      )
   {
	  throw new CSoundException( CSoundException::causeOutermostChunkInWAVNotRIFF);
   }
}

WaveRead::~WaveRead() 
{
   if (_decoder) delete _decoder;
   if(_formatData) { delete [] _formatData; }
}
