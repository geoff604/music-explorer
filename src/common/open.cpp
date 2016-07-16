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
#include "open.h"

// Include headers for various formats
#include "mpeg.h" // MPEG Audio File
#include "au.h"   // Sun AU / NeXT SND
#include "voc.h"  // Creative Labs VOC
#include "wav.h"  // Microsoft RIFF WAVE
#include "aiff.h" // Apple AIFF, Apple AIFF-C
#include "iff.h"  // Electronic Arts IFF/8SVX
#include "midi.h" // MIDI File
#include "mod.h"  // ProTracker MOD

AudioAbstract *OpenFormat(istream &file) {
   if (IsVocFile(file)) {
      file.seekg(0);
      return new VocRead(file);
   }
   if (IsAuFile(file)) {
      file.seekg(0);
      return new AuRead(file);
   }
   if (IsWaveFile(file)) {
      file.seekg(0);
      return new WaveRead(file);
   }
   if (IsAiffFile(file)) {
      file.seekg(0);
      return new AiffRead(file);
   }
   if (IsIffFile(file)) {
      file.seekg(0);
      return new IffRead(file);
   }
   if (IsMidiFile(file)) {
      file.seekg(0);
      return new MidiRead(file);
   }
   if (IsModFile(file)) {
      file.seekg(0);
      return new ModRead(file);
   }
   if (IsMpegFile(file)) {
      file.seekg(0);
      return new MpegRead(file);
   }
   cerr << "I don't recognize this format.\n";
   return 0;
}
