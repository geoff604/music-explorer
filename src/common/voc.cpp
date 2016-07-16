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
#include <cstring>
#include "voc.h"

bool IsVocFile(istream &file) {
   file.seekg(0);  // Seek to beginning
   char id[21];
   file.read(id,20);
   return (!strncmp(id,"Creative Voice File\x1a",20));
}
void VocRead::ReadHeader(void) {
   char id[21];
   int headerSize, fileVersionCode, fileVersionCheck;

   _stream.read(id,20);
   if (strncmp(id,"Creative Voice File\x1a",20)) {
      cerr << "This is not a VOC file.\n";
      exit(1);
   }
   headerSize = ReadIntLsb(_stream,2);
   if (headerSize < 26) {
      cerr << "This VOC file is corrupted.\n";
      exit(1);
   }

   fileVersionCode = ReadIntLsb(_stream,2);
   fileVersionCheck = ReadIntLsb(_stream,2);

   if (fileVersionCode + fileVersionCheck != 0x1233)
      cerr << "This VOC file may be corrupted.\n";

   // Is header larger than 26 bytes?
   if (headerSize > 26)
      SkipBytes(_stream,26-headerSize); // Skip rest
}
void VocRead::GetBlock(void) {
   long blockLength = 0;
   _blockType = ReadIntLsb(_stream,1);
   if(_stream.eof()) _blockType = 0;
   if (_blockType != 0)
      blockLength = ReadIntLsb(_stream,3);

   switch(_blockType) {

      
case 0: // End of data
   break;
case 1: // Sound Data Block
   ReadBlock1(blockLength);
   break;
case 2:  // Sound Data Continued
   bytesRemaining = blockLength;
   break;
case 3: // Silence
{
   if (blockLength != 3) {
      cerr << "VOC Silence Block has length " << blockLength;
      cerr << "(should be 3)\n";
   }
   bytesRemaining = ReadIntLsb(_stream,2) + 1;
   int sampleRateCode = ReadIntLsb(_stream,1);
   if (_fileSampleRate == -1)
      _fileSampleRate = 1000000/(256-sampleRateCode);
   bytesRemaining = blockLength - 3;
   break;
}
case 4: // Marker block
   if (blockLength != 2) {
      cerr << "VOC Marker Block has length " << blockLength;
      cerr << "(should be 2)\n";
   }
   cerr << "VOC Marker: ";
   cerr << ReadIntLsb(_stream,blockLength);
   cerr << "\n";
   bytesRemaining = blockLength - 2;
   break;
case 5: // ASCII text block
{
   char *text = new char[blockLength+1];
   _stream.read(text,blockLength);
   text[blockLength] = 0;
   cerr << "Comment: " << text << "\n";
   delete [] text;
   bytesRemaining = 0;
   break;
}
case 6: // Repeat loop
{
   if (blockLength != 2) {
      cerr << "VOC Repeat Loop Block has length " << blockLength;
      cerr << "(should be 2)\n";
   }
   cerr << "Start of VOC Repeat Block.\n";
   int repeatCount = ReadIntLsb(_stream,2);
   repeatDepth++;
   if (repeatDepth > maxRepeat) {
      cerr << "Too many nested repeats.\n";
      exit(1);
   }
   repeatCounts[repeatDepth] = repeatCount;
   repeatPosition[repeatDepth] = _stream.tellg();
   bytesRemaining = blockLength - 2;
   break;
}
case 7: // Repeat end
{
   bytesRemaining = blockLength;
   if (blockLength != 0) {
      cerr << "VOC `End Repeat' block has length " << blockLength;
      cerr << "(should be 0)\n";
   }
   if (repeatDepth < 0) {
      cerr << "Improper VOC `End Repeat' block.\n";
      break;
   }
   // On Mac, the repeatCounts[repeatDepth] always prints as 1 or 0 ??
   // Is this a bug in the C++ library?
   cerr << "End of VOC Repeat Block(";
   cerr << repeatCounts[repeatDepth] << ")\n";
   if (repeatCounts[repeatDepth] <= 0) { // End of repeat?
      repeatDepth--;
   } else { // Position for next iteration
      _stream.seekg(repeatPosition[repeatDepth]);
      repeatCounts[repeatDepth]--;
   }
   break;
}
case 8: // Extension
   ReadBlock8(blockLength);
   break;
case 9: // Extension
   ReadBlock9(blockLength);
   break;
default: // Skip any other type of block
   cerr << "Ignoring unrecognized VOC block type " << _blockType << "\n";
   SkipBytes(_stream,blockLength);
   break;

   }
}
void VocRead::ReadBlock1(long blockLength) {
   // Read and interpret first two bytes...
   int sampleRateCode = ReadIntLsb(_stream,1);
   int compressionCode = ReadIntLsb(_stream,1);
   if (_fileSampleRate == -1)
      _fileSampleRate = 1000000L/(256-sampleRateCode);
   if (_fileCompression == -1)
      _fileCompression = compressionCode;
   if (_fileChannels == -1)
      _fileChannels = 1;
   if (_fileWidth == -1)
      _fileWidth = 8;
   if (_decoder == 0) { // No decoder object yet?
      switch(_fileCompression) {
      case 0: // Unsigned 8-bit PCM
         _decoder = new DecompressPcm8Unsigned(*this);
         break;
      case 4: // Signed 16-bit PCM
         _decoder = new DecompressPcm16LsbSigned(*this);
         break;
      case 1: // Creative Labs 8-bit to 4-bit ADPCM
      case 2: // Creative Labs 8-bit to 2.6-bit ADPCM
      case 3: // Creative Labs 8-bit to 2-bit ADPCM
      case 6: // CCITT A-Law
      case 7: // CCITT mu-Law
      case 512: // Creative Labs 16-bit to 4-bit ADPCM
      default:
         cerr << "I don't support VOC compression type ";
         cerr << _fileCompression << ".\n";
         exit(1);
      }
   }
   bytesRemaining = blockLength - 2;
}
void VocRead::ReadBlock8(long blockLength) {
   if (blockLength != 4) {
      cerr << "VOC Extension Block 8 has length " << blockLength;
      cerr << "(should be 4)\n";
   }
   int sampleRateCode = ReadIntLsb(_stream,2);
   if (_fileSampleRate == -1)
      _fileSampleRate = 256000000L/(65536L-sampleRateCode);
   int compressionCode = ReadIntLsb(_stream,1);
   if (_fileCompression == -1)
      _fileCompression = compressionCode;
   int channels = ReadIntLsb(_stream,1);
   if (_fileChannels == -1) {
      _fileChannels = channels + 1;
      _fileSampleRate /= _fileChannels;
   }
   if (_fileWidth == -1)
      _fileWidth = 8;
   bytesRemaining = blockLength - 4;
}
void VocRead::ReadBlock9(long blockLength) {
   if (blockLength != 12) {
      cerr << "VOC Extension Block 9 has length " << blockLength;
      cerr << "(should be 12)\n";
   }
   long sampleRate = ReadIntLsb(_stream,4);
   if (_fileSampleRate == -1)
      _fileSampleRate = sampleRate;
   int bitsPerSample = ReadIntLsb(_stream,1);
   if (_fileWidth == -1)
      _fileWidth = bitsPerSample;
   int channels = ReadIntLsb(_stream,1);
   if (_fileChannels == -1)
      _fileChannels = channels;
   int compressionCode = ReadIntLsb(_stream,2);
   if (_fileCompression == -1)
      _fileCompression = compressionCode;
   SkipBytes(_stream,blockLength - 8);
}
VocRead::VocRead(istream &input):AudioAbstract(),_stream(input) {
   cerr << "File Format: Creative Labs VOC\n";
   
   repeatDepth = -1;
   _fileChannels = -1;
   _fileSampleRate = -1;
   _fileCompression = -1;
   _fileWidth = -1;
   _decoder = 0;
   ReadHeader();
   do {   // Find sound data block or terminator block
      GetBlock();
   } while ((_blockType != 1) && (_blockType != 0));
}
size_t VocRead::GetSamples(AudioSample *buffer, size_t numSamples) {
   size_t samplesReturned = 0;
   while (numSamples > 0) {
      switch(_blockType) {
      case 0: // End of file
         return samplesReturned;
      case 1: // Encoded sound data
      case 2: {
            size_t samplesRead = _decoder->GetSamples(buffer,numSamples);
            buffer += samplesRead;
            numSamples -= samplesRead;
            samplesReturned += samplesRead;
            if (bytesRemaining == 0) GetBlock();
            break;
         }
      case 3: // Silence block
         while ((numSamples > 0) && (bytesRemaining > 0)) {
            *buffer++ = 0;
            samplesReturned++;
            numSamples--;
            bytesRemaining--;
         }
         if (bytesRemaining == 0) GetBlock();
         break;
      default:
         GetBlock();
         break;
      }
   }
   return samplesReturned;
}
size_t VocRead::ReadBytes(AudioByte *buffer, size_t length) {
   if (length > bytesRemaining) length = bytesRemaining;
   _stream.read(reinterpret_cast<char *>(buffer), length);
   size_t bytesRead = _stream.gcount();
   if (bytesRead < length) {
      cerr << "Error:  VOC file ended prematurely.";
      exit(1);
   }
   bytesRemaining -= bytesRead; // Amount left in block
   return bytesRead;
}
