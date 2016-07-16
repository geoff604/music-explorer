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
#ifndef MOD_H_INCLUDED
#define MOD_H_INCLUDED
#include <cstdio>
#include "audio.h"
#include "instrumt.h"
#include "sampled.h"

bool IsModFile(istream &file);

// First, pre-declare some things
struct ModNoteData;
class ModInstrument;

class ModNote: public AbstractNote {
   
private:
   AbstractNote *_abstractNote;
public:
   void Restart() {
      if (_abstractNote) _abstractNote->Restart();
   }
   size_t AddSamples(AudioSample *buff, size_t length) {
      if (_abstractNote) return _abstractNote->AddSamples(buff,length);
      else return 0;
   }

   // This is the only place that assumes we're using a sampled note
   void SetSampleOffset(int offset) {
      // Should use dynamic_cast here
      if (_abstractNote)
         reinterpret_cast<SampledNote *>(_abstractNote)
               ->SetSampleOffset(offset);
   }
   void Pitch(float t) { 
      if (_abstractNote)  _abstractNote->Pitch(t);
   }
   float Pitch() {
      if (_abstractNote)   return _abstractNote->Pitch();
      else                 return 440.0;
   }
   void EndNote(float v) {
      if (_abstractNote)   _abstractNote->EndNote(v);
   }      
private:
   void Volume(float) {};
   float Volume() {return 1.0;}
public:
   void SetModVolume(int volume);
   void SetModPeriod(int period);
   ModNote(AbstractNote *,int period, int volume);
   ~ModNote();
};

class ModInstrument: public AbstractInstrument {
   
private:
   SampledInstrument *_sampledInstrument;
private:
   char _name[23];  /* Null-terminated string */
public:
   const char *Name() { return _name; }
private:
   signed char _finetune; /* -8 to +7 */
   unsigned char _volume; /* 0 to 64 */
   long _length; /* Bytes */
   long _repeatStart; /* Bytes from beginning */
   long _repeatLength; /* length of repeat */
public:
   ModInstrument() { _sampledInstrument = 0; _name[0] = 0; }
   virtual ~ModInstrument(){
      if (_sampledInstrument)
         delete _sampledInstrument;
   };
   void ReadHeader(istream &modFile);
   void ReadSample(istream &modFile);
   ModNote *NewModNote(int period);
   void SamplingRate(long s) {
      AbstractInstrument::SamplingRate(s);
      if(_sampledInstrument)
         _sampledInstrument->SamplingRate(s);
   }
   long SamplingRate() {
        return AbstractInstrument::SamplingRate();
   }
private:
   AbstractNote *NewNote(float pitch, float volume);
};

struct ModNoteData {
   
   enum effectType { none=0, arpeggio, slideUp, slideDown,
      pitchSlide, vibrato, pitchSlidePlusVolumeSlide,
      vibratoPlusVolumeSlide, tremolo, setSampleOffset,
      volumeSlide, farJump, setVolume, smallJump, setTempo,
      setFilter, pitchUp, pitchDown, setGlissando, setVibrato,
      setFinetune, patternLoop, setTremolo, retrigger, volumeUp,
      volumeDown, cutNote, delayNote, delayPattern, invertLoop,
      effectCount
       };

   unsigned char _instrument; // 8 bits for instrument
   unsigned short _period; // 12 bits for period
   ModNoteData::effectType _effect; // 4(8) bits for effect
   unsigned char _parameter; // 8(4) bits for parameter

public:
   void ReadNote(istream &modFile); // HUH?
};

class ModSong {
   
private:
   unsigned char _playList[128];
   int _playListLength;
   int _maxPattern;
   typedef ModNoteData Beat[4]; // 4 notes in a beat
   typedef Beat *Pattern;  // Pattern is an array of beats
   Pattern *_patterns;
public:
   void ReadPlayList(istream &modFile);
   void ReadPatterns(istream &modFile);
   ModSong();
   ~ModSong();
private:
   // Information needed to play MOD file
   int _thisIndex; // current position in play list
   int _thisBeat; // current beat in corresponding pattern
   int _lastIndex; // previous index/beat
   int _lastBeat;
   int _nextIndex; // Next index/beat
   int _nextBeat;
public:
   bool Advance(); // Advance to next beat, true if end-of-song
   void Back(); // Backup to previous beat
   void AdvanceNextIndex(); // Skip to next pattern index
   void NextIndex(int i); // Set next pattern index
   void NextBeat(int b); // Set next beat
   void Stop(); // Next call to Advance() will return true
   ModNoteData &ThisNote(int ch) {
      return _patterns[ _playList[_thisIndex] ] [_thisBeat] [ch];
   }
};

struct ModChannel {
   
public:
   ModNoteData _currentNote;
   int _currentVolume;
   ModNote *_liveNote;
   unsigned char _defaultParameter[ModNoteData::effectCount];
   ModChannel();
   ~ModChannel();
public:
   int _pitchGoal; // current target period for Pitch Slide
public:
   const float *_vibratoWaveform; // Current waveform
   int _currentVibrato; // position in waveform
public:
   const float *_tremoloWaveform; // current waveform
   int _currentTremolo; // position in waveform
public:
   static const float _waveform[3][64]; // waveform library
public:
   int _delayPatternCount; // >= 0 if delay pattern loop is in effect
};

class ModRead: public AudioAbstract {
   
   void MinMaxSamplingRate(long *min, long *max, long *preferred) {
      *min = 8000;
      *max = 44100;
      *preferred = 11025;
   }
   long SamplingRate() { return AudioAbstract::SamplingRate(); }
   void SamplingRate(long s) {
      AudioAbstract::SamplingRate(s);
      for (int i=0;i<numInstruments;i++)
         if(_instrument[i])
            _instrument[i]->SamplingRate(s);
   }
   void MinMaxChannels(int *min, int *max, int *preferred) {
      *min = 1;
      *max = *preferred = 2;
   }
public:
   ModRead(istream &); // Constructor takes an open stream
   ~ModRead();

private:
   // Read MOD file data
   char _name[21];
   char _marker[4];
   enum {numInstruments = 32};
   ModInstrument *_instrument[numInstruments];
   ModSong _song;
   void ReadMod(istream &modFile);
public:
   size_t GetSamples(AudioSample *buffer, size_t numSamples);
private:
   AudioSample *_sampleBufferLeft; // Left channel
   AudioSample *_sampleBufferRight; // Right channel
   int _sampleBufferSize;
   AudioSample *_sampleStartLeft; // position in sampleBufferLeft
   AudioSample *_sampleStartRight;
   int _sampleLength; // length of data in sample buffers
   void SetSampleBufferSize();
private:
   int _ticksPerMinute; // tick rate
   int _ticksPerBeat;
   int _samplesPerBeat;
   int _samplesPerTick; // This is approximate!!
private:
   // Current `live' notes
   enum {numberChannels = 4};
   ModChannel _channel[numberChannels];

   void PlayBeat(); // play next beat into sampleBuffer
};

#endif
