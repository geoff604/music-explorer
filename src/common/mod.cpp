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
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "mod.h"
#include "audio.h"
#include "instrumt.h"
#include "sampled.h"

// Waveforms for vibrato (varying pitch) and tremolo (varying volume)
static float vibratoTremoloWaveforms[3][64] = {
   // Sine waveform
   {0.0F,   .09802F, .19509F, .29028F, .38268F, .47140F, .55557F,
    .63439F, .70711F, .77301F, .83147F, .88192F, .92388F, .95694F,
    .98079F, .99518F,1.00000F, .99518F, .98079F, .95694F, .92388F,
    .88192F, .83147F, .77301F, .70711F, .63439F, .55557F, .47140F,
    .38268F, .29028F, .19509F, .09802F,-.00000F,-.09802F,-.19509F,
   -.29028F,-.38268F,-.47140F,-.55557F,-.63439F,-.70711F,-.77301F,
   -.83147F,-.88192F,-.92388F,-.95694F,-.98079F,-.99518F,-1.0F,
   -.99518F,-.98079F,-.95694F,-.92388F,-.88192F,-.83147F,-.77301F,
   -.70711F,-.63439F,-.55557F,-.47140F,-.38268F,-.29028F,-.19509F,
   -.09802F },
   // Square waveform: 32x1, then 32x-1
   {1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F,
    1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F,
    1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F,
    1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F,
   -1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,
   -1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,
   -1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,
   -1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F,-1.0F },
   // Ramp waveform: 0->1, then -1->0
   {0.0F,    .03125F, .06250F, .09375F, .12500F, .15625F, .18750F,
    .21875F, .25000F, .28125F, .31250F, .34375F, .37500F, .40625F,
    .43750F, .46875F, .50000F, .53125F, .56250F, .59375F, .62500F,
    .65625F, .68750F, .71875F, .75000F, .78125F, .81250F, .84375F,
    .87500F, .90625F, .93750F, .96875F
   -1.0F,   -.96875F,-.93750F,-.90625F,-.87500F,-.84375F,-.81250F,
   -.78125F,-.75000F,-.71875F,-.68750F,-.65625F,-.62500F,-.59375F,
   -.56250F,-.53125F,-.50000F,-.46875F,-.43750F,-.40625F,-.37500F,
   -.34375F,-.31250F,-.28125F,-.25000F,-.21875F,-.18750F,-.15625F,
   -.12500F,-.09375F,-.06250F,-.03125F }
   };
bool IsModFile(istream &file) {
   file.seekg(20+30*31+1+1+128);
   char marker[4];
   file.read(marker,4);
   if (memcmp(marker,"M.K.",4)==0) return true;
   if (memcmp(marker,"M!K!",4)==0) return true;
   return false;
}
void ModInstrument::ReadHeader(istream &modFile){
   modFile.read(_name,22);
   _name[22]=0;
   _length = ReadIntMsb(modFile,2) * 2;
   _finetune = ReadIntMsb(modFile,1);
   if (_finetune > 7) _finetune -= 16;
   _volume = ReadIntMsb(modFile,1);
   if (_volume > 64) _volume = 64;
   _repeatStart = ReadIntMsb(modFile,2) * 2;
   _repeatLength = ReadIntMsb(modFile,2) * 2;
}

void ModInstrument::ReadSample(istream &modFile) {
   if (_length < 1) return;
   AudioSample *data 
         = new AudioSample[_length+_repeatLength+1024];
   AudioByte *byteBuff 
         = reinterpret_cast<AudioByte *>(data);
   modFile.read(reinterpret_cast<char *>(byteBuff),_length);

   for(long s = _length-1; s>=0; s--) {
      data[s] = static_cast<signed char>(byteBuff[s]) * 
               (1<<(8*sizeof(AudioSample)-8));
   }

   // Adjust the repeat portion
   int repeatTest=0;
   for(int i=_repeatStart;i<_repeatLength;i++)
      repeatTest += data[i];
   // If entire repeat is zero, set to no repeat
   if(repeatTest==0) {
      _repeatLength = 0;
      _repeatStart = _length;
   }
   // MOD convention: a two-byte repeat at the beginning means
   // no repeat
   if((_repeatLength<=2) && (_repeatStart == 0)) {
      _repeatLength = 0;
      _repeatStart = _length;
   }

   _sampledInstrument = new SampledInstrument(data,
            _length,_repeatStart,_repeatStart+_repeatLength);

   // The Pitch() parameters are determined by
   // the convention that A440 uses a divider of
   // 254, as modified by the finetune value
   _sampledInstrument
         ->BasePitch(440*pow(2.0,-_finetune/96.0),
                     3575872.0F/254.0F);

   delete [] data;
}
void ModNote::SetModVolume(int volume) {
   if (volume > 64) volume = 64;
   if (volume < 0) volume = 0;
   if (_abstractNote)
      _abstractNote->Volume(volume/64.0/4.0);
}

void ModNote::SetModPeriod(int period) {
   if (period < 113) period=113;
   if (period > 856) period=856;
   if (_abstractNote)
      _abstractNote->Pitch(440.0*254.0/period);
}
ModNote::ModNote(AbstractNote *abstractNote, int period,
   int volume) {
   _abstractNote = abstractNote;
   SetModVolume(volume);
   SetModPeriod(period);
   Restart();
}

ModNote::~ModNote() {
   if (_abstractNote)  delete _abstractNote;
}

ModNote *
ModInstrument::NewModNote(int period) {
   AbstractNote *note = NewNote(440,0.5); // Generic note
   // ModNote constructor will reset pitch and volume
   return new ModNote(note,period,_volume);
}

AbstractNote *
ModInstrument::NewNote(float pitch, float volume) {
   if (_sampledInstrument)
      return _sampledInstrument->NewNote(pitch,volume);
   else
      return 0;
}
void ModNoteData::ReadNote(istream &modFile) {

   static ModNoteData::effectType primaryEffects[] = {
      arpeggio, slideUp, slideDown, pitchSlide,
      vibrato, pitchSlidePlusVolumeSlide,
      vibratoPlusVolumeSlide, tremolo, none,
      setSampleOffset, volumeSlide, farJump, setVolume,
      smallJump, none, setTempo
   };

   static ModNoteData::effectType secondaryEffects[] = {
      setFilter, pitchUp, pitchDown, setGlissando,
      setVibrato, setFinetune, patternLoop, setTremolo,
      none, retrigger, volumeUp, volumeDown, cutNote,
      delayNote, delayPattern, invertLoop
   };

   unsigned char b[4];
   int effectValue;
   modFile.read(reinterpret_cast<char *>(b),4);
   _instrument = (b[0]&0xF0) | ((b[2]>>4) & 0x0F);
   _period = ((static_cast<int>(b[0])&0x0F)<<8)
          + (static_cast<int>(b[1]) & 0xFF);
   effectValue = static_cast<int>(b[2]) & 0x0F;
   _parameter = static_cast<int>(b[3]) & 0xFF;
   // Effect 14 is handled a bit differently...
   if (effectValue == 14) {
      _effect = secondaryEffects[(_parameter>>4)&0x0F];
      _parameter &= 0x0F;
   } else if ((effectValue == 0) && (_parameter == 0)){
      _effect = none;
   } else {
      _effect = primaryEffects[effectValue];
   }
}
void ModSong::Stop() {
   _nextIndex = _playListLength;
}

bool ModSong::Advance() { // Returns true if end-of-song
   _lastBeat = _thisBeat;
   _lastIndex = _thisIndex;
   _thisBeat = _nextBeat;
   _thisIndex = _nextIndex;
   _nextBeat++;
   if (_nextBeat >= 64) {  // Advance to next pattern?
      _nextBeat = 0;
      _nextIndex++;
   }
   if (_thisIndex >= _playListLength)  return true;
   else return false;
}

void ModSong::Back() {
   _nextBeat = _thisBeat;
   _nextIndex = _thisIndex;
   _thisBeat = _lastBeat;
   _thisIndex = _lastIndex;
}

void ModSong::NextBeat(int b) {
   _nextBeat = b;
}

void ModSong::NextIndex(int i) {
   _nextIndex = i;
}

void ModSong::AdvanceNextIndex() {
   _nextIndex++;
}
ModSong::ModSong() {
   _maxPattern=0;
   _patterns = 0;
   
   _lastIndex = _lastBeat = 0;
   _thisIndex = _thisBeat = 0;
   _nextIndex = _nextBeat = 0;
}

ModSong::~ModSong() { // Free patterns
   if (_patterns) {
      for(int p=0;p<=_maxPattern;p++)
         delete [] _patterns[p];
      delete [] _patterns;
   }
}
void ModSong::ReadPlayList(istream &modFile) {
   _playListLength = ReadIntMsb(modFile,1);
   (void)ReadIntMsb(modFile,1); // Discard extra byte
   _maxPattern=0;
   for(int i=0;i<128;i++) {
      _playList[i] = ReadIntMsb(modFile,1);
      if (_playList[i] > _maxPattern)
         _maxPattern = _playList[i];
   }   
}

void ModSong::ReadPatterns(istream &modFile) {
   _patterns = new Pattern[_maxPattern+1];
   for (int p=0;p<=_maxPattern;p++) {
      _patterns[p] = new Beat[64];
      for (int b=0;b<64;b++)
         for (int n=0;n<4;n++)
            _patterns[p][b][n].ReadNote(modFile);
   }
}
ModRead::ModRead(istream &modFile) {
   cerr << "File Format: ProTracker MOD\n";
   
   _sampleBufferLeft = 0;
   _sampleBufferRight = 0;
   _sampleBufferSize = 0;
   _sampleLength = 0;
   _ticksPerMinute = 50 * 60;
   _ticksPerBeat = 6;
   ReadMod(modFile);   // Slurp in the file
}

ModRead::~ModRead() {
   for (int i=1;i<numInstruments;i++)
      if (_instrument[i]) delete _instrument[i];
   if (_sampleBufferLeft) delete [] _sampleBufferLeft;
   if (_sampleBufferRight) delete [] _sampleBufferRight;

}

void ModRead::ReadMod(istream &modFile) {
   modFile.read(_name,20);
   _name[20] = 0;
   cerr << "Name: " << _name << "\n";

   _instrument[0] = NULL;
   for (int i=1;i<numInstruments;i++) {
      _instrument[i] = new ModInstrument();
      _instrument[i]->ReadHeader(modFile);
   }

   { // Many composers provide comments in the instrument names area
     // so dump that to the user.
      char msg[80];
      cerr << "Instruments: \n";
      int step = (numInstruments+2)/3; // Use three columns
      for (int i=1;i<=step;i++) {
         sprintf(msg,"%2d:%-22s  ",i,_instrument[i]->Name());
         cerr << msg;
         if (i+step < numInstruments) {
            sprintf(msg,"%2d:%-22s  ",i+step,
                  _instrument[i+step]->Name());
            cerr << msg;
         }
         if (i+step+step < numInstruments) {
            sprintf(msg,"%2d:%-22s",i+step+step,
                     _instrument[i+step+step]->Name());
            cerr << msg;
         }
         cerr << "\n";
      }
   }

   _song.ReadPlayList(modFile);
   modFile.read(_marker,4); // Read separator
   if (memcmp(_marker,"M.K.",4)==0) {
      // Protracker MOD with no more than 64 patterns
   } else if (memcmp(_marker,"M!K!",4)==0) {
      // Protracker MOD with more than 64 patterns
   } else {
      // I don't handle any other format
      cerr << "Unrecognized signature `"
            << _marker[0] << _marker[1] 
            << _marker[2] << _marker[3]
            << "'\n";
      exit(1);
   }
   _song.ReadPatterns(modFile);
   for (int i1=1;i1<numInstruments;i1++)
      _instrument[i1]->ReadSample(modFile);
}
size_t ModRead::GetSamples(AudioSample *buff, size_t length) {
   int requestedLength = length;
   do {
      switch(Channels()) {
      case 1: // Mono, just copy and convert
         while ((_sampleLength > 0)&&(length > 0)) {
            *buff++ = *_sampleStartLeft++;
            _sampleLength--;
            length--;
         }
         break;
      case 2: // Stereo, interleave two channels
         while ((_sampleLength > 0)&&(length > 0)) {
            *buff++ = *_sampleStartLeft++;
            *buff++ = *_sampleStartRight++;
            _sampleLength--;
            length-=2;
         }
         break;
      default: cerr << "Internal error\n";
         exit(1); // This can't happen!!
      }
      if (length > 0) // Need more data?
         PlayBeat(); // Generate next beat
      if (_sampleLength == 0) break; // No more data!!
   } while (length>0);
   return requestedLength-length;
}
// Make sure the sample buffer is large enough
void ModRead::SetSampleBufferSize() {
   _samplesPerBeat = SamplingRate() * _ticksPerBeat * 60
                   / _ticksPerMinute;
   _samplesPerTick = _samplesPerBeat / _ticksPerBeat;
   if (_sampleBufferLeft && (_sampleBufferSize >= _samplesPerBeat))
      return;
   if (_sampleBufferRight == _sampleBufferLeft)
      _sampleBufferRight = 0;
   if (_sampleBufferLeft)
      delete [] _sampleBufferLeft;
   _sampleBufferLeft = new AudioSample[_samplesPerBeat];
   switch(Channels()) {
   // If mono, both channels use same buffer
   case 1: _sampleBufferRight = _sampleBufferLeft; break;
   case 2:
      if (_sampleBufferRight)
         delete [] _sampleBufferRight;
      _sampleBufferRight = new AudioSample[_samplesPerBeat];
      break;
   default:
      cerr << "Illegal number of channels: " 
           << Channels()
           << "\n";
      exit(1);
   }
   _sampleBufferSize = _samplesPerBeat;
   _sampleStartLeft = _sampleBufferLeft;
   _sampleStartRight = _sampleBufferRight;
   _sampleLength = 0; // Nothing in buffer now
}
ModChannel::ModChannel() {
   _liveNote = NULL;
   for(int i0=0;i0<ModNoteData::effectCount;i0++)
      _defaultParameter[i0]=0;
   
_pitchGoal = 0;
_vibratoWaveform = &(vibratoTremoloWaveforms[0][0]);
_currentVibrato = 0;
_tremoloWaveform = &(vibratoTremoloWaveforms[0][0]);
_currentTremolo = 0;
_delayPatternCount = -1;
};

ModChannel::~ModChannel() {
   if (_liveNote) delete _liveNote;
};
/* Play this Beat */
void ModRead::PlayBeat() {
   AudioSample *sampleBuffer;
   if (_song.Advance()) return;

   for(int ch0=0;ch0<numberChannels;ch0++) {
      ModNoteData currentNote = _song.ThisNote(ch0);
      switch(currentNote._effect) {
      default: break;
      
   case ModNoteData::farJump:
      _song.NextIndex(currentNote._parameter); // Jump to this pattern
      _song.NextBeat(0); // at beat 0
      break;
   case ModNoteData::smallJump:
      _song.AdvanceNextIndex();  // jump to next pattern
      _song.NextBeat((currentNote._parameter>>4)*10
             + (currentNote._parameter &0x0F)); // at this beat
      break;
   case ModNoteData::patternLoop:
      cerr << "Loop pattern effect not implemented.\n";
      break;
   case ModNoteData::delayPattern:
      // Delay Pattern is essentially a one-beat loop
      if (_channel[ch0]._delayPatternCount < 0) { // Starting new delay
         _channel[ch0]._delayPatternCount = currentNote._parameter;
      }
      if (_channel[ch0]._delayPatternCount >0) { // Positive count,
         _song.Back();
         _channel[ch0]._delayPatternCount--; // Count delays
      } else {
         _channel[ch0]._delayPatternCount = -1; // no longer delaying
      }
      break;
   case ModNoteData::setTempo:
      if (currentNote._parameter == 0) { // stop song
         cerr << "Set tempo 0 exercised.\n";
         _song.Stop();
         _sampleLength = 0;
         return;
      } else if (currentNote._parameter <= 32) { // set ticks per beat
         _ticksPerBeat = currentNote._parameter;
      } else { // set tick rate
         _ticksPerMinute = 50L * 60 * currentNote._parameter/125;
      }
      break;
      }
   }

   SetSampleBufferSize();
   memset(_sampleBufferLeft, 0,
         sizeof(_sampleBufferLeft[0]) * _samplesPerBeat);
   if (Channels()==2)
      memset(_sampleBufferRight, 0,
         sizeof(_sampleBufferRight[0]) * _samplesPerBeat);

   // Set up and play each channel
   for(int ch=0;ch<numberChannels;ch++) {
      switch(ch) {
      case 0: case 3: case 4: case 7:
         sampleBuffer = _sampleBufferLeft; break;
      case 1: case 2: case 5: case 6: default:
         sampleBuffer = _sampleBufferRight; break;
      }
      ModNoteData currentNote = _song.ThisNote(ch);
      // If either instrument or period is non-zero,
      // start a new note.
      if (   (currentNote._instrument != 0) 
          || (currentNote._period != 0)) {
         // First, get defaults from the previous note
         if (currentNote._instrument == 0)
            currentNote._instrument = _channel[ch]._currentNote._instrument;
         if (currentNote._period == 0)
            currentNote._period = _channel[ch]._currentNote._period;
         if (currentNote._instrument >= numInstruments) {
            cerr << "Illegal instrument number "
                 << int(currentNote._instrument) << ".\n";
            break;
         }
         
if (currentNote._instrument != 0) {
   if (  (currentNote._effect == ModNoteData::tremolo)
      && ((_channel[ch]._defaultParameter[ModNoteData::tremolo] & 4)==0) )
         _channel[ch]._currentTremolo = 0;
   if ( (currentNote._effect == ModNoteData::vibrato)
      ||(currentNote._effect == ModNoteData::vibratoPlusVolumeSlide)) {
      if ((_channel[ch]._defaultParameter[ModNoteData::vibrato] & 4)==0)
         _channel[ch]._currentVibrato = 0;
   }
   if (currentNote._effect == ModNoteData::pitchSlide) {
      if (currentNote._period) {
         _channel[ch]._pitchGoal = currentNote._period;
      } else if (_channel[ch]._pitchGoal == 0) {
         // No applicable goal, so ignore this effect
         currentNote._effect = ModNoteData::none;
         currentNote._parameter = 0;
      }
      // Put actual period/instrument into currentNote
      currentNote._period = _channel[ch]._currentNote._period;
      currentNote._instrument = _channel[ch]._currentNote._instrument;
   }
   // Note: this is not the same as `else'
   if (currentNote._effect != ModNoteData::pitchSlide) {
      if (_channel[ch]._liveNote) delete _channel[ch]._liveNote;
      _channel[ch]._liveNote = _instrument[currentNote._instrument]->
                      NewModNote(currentNote._period);
   }
}
      } else { // Otherwise, continue the previous note
         currentNote._instrument = _channel[ch]._currentNote._instrument;
         currentNote._period = _channel[ch]._currentNote._period;
      }

      if (_channel[ch]._liveNote) {
         ModNote &currentLiveNote = *_channel[ch]._liveNote;
         int defaultParameter 
               = _channel[ch]._defaultParameter[currentNote._effect];
         int thisParameter = currentNote._parameter;
         switch(currentNote._effect) {
            
case ModNoteData::none:
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::arpeggio:
   {  const float halfTones[] = {
            0.0F,        1.05946309F, 1.12246205F, 1.18920712F,
            1.25992105F, 1.33483985F, 1.41421356F, 1.49830708F,
            1.58740105F, 1.68179283F, 1.78179744F, 1.88774863F,
            2.0F,        2.11892619F, 2.24492410F, 2.37841423F};

      AudioSample *currentSampleBuffer = sampleBuffer;
      float pitches[3];
      currentLiveNote.SetModPeriod(currentNote._period);
      pitches[0] = currentLiveNote.Pitch();
      pitches[1] = pitches[0] * halfTones[(thisParameter>>4)&0xF];
      pitches[2] = pitches[0] * halfTones[(thisParameter&0xF)];
      int currentPitch = 0;
      int i;
      for(i=0;i<_samplesPerBeat-_samplesPerTick*3/2;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         if (++currentPitch > 2) currentPitch = 0;
         currentLiveNote.Pitch(pitches[currentPitch]);
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
      currentLiveNote.Pitch(pitches[0]); // reset original pitch
   }
   break;
case ModNoteData::slideUp:
   {
      if (thisParameter == 0) thisParameter = defaultParameter;
      AudioSample *currentSampleBuffer = sampleBuffer;
      int i;
      for(i=0;i<_samplesPerBeat-_samplesPerTick*3/2;
                  i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         currentNote._period -= thisParameter;
         currentLiveNote.SetModPeriod(currentNote._period);
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::slideDown:
   {
      if (thisParameter == 0) thisParameter = defaultParameter;
      AudioSample *currentSampleBuffer = sampleBuffer;
      int i = 0;
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         currentNote._period += thisParameter;
         currentLiveNote.SetModPeriod(currentNote._period);
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::pitchSlide:
   {
      if (thisParameter == 0) thisParameter = defaultParameter;
      AudioSample *currentSampleBuffer = sampleBuffer;
      int i = 0;
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         if (_channel[ch]._pitchGoal == 0) {
            // Do nothing
         } else if (currentNote._period > _channel[ch]._pitchGoal) {
            currentNote._period -= thisParameter;
            if (currentNote._period <= _channel[ch]._pitchGoal) {
               currentNote._period = _channel[ch]._pitchGoal;
               _channel[ch]._pitchGoal = 0;
            }
         } else {
            currentNote._period += thisParameter;
            if (currentNote._period >= _channel[ch]._pitchGoal) {
               currentNote._period = _channel[ch]._pitchGoal;
               _channel[ch]._pitchGoal = 0;
            }
         }
         currentLiveNote.SetModPeriod(currentNote._period);
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::vibrato:
   {
      if (thisParameter == 0)
         thisParameter = defaultParameter;
      int step = (thisParameter >> 4)&0xF;
      int amplitude = (thisParameter&0xF)*2;
      AudioSample *currentSampleBuffer = sampleBuffer;

      // Set vibrato
      currentLiveNote.SetModPeriod(currentNote._period
         +int(_channel[ch]._vibratoWaveform[_channel[ch]._currentVibrato]
              *amplitude));
      // Step vibrato
      _channel[ch]._currentVibrato += step;
      _channel[ch]._currentVibrato &= 63;

      int i=0;      
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         // Set vibrato
         currentLiveNote.SetModPeriod(currentNote._period
            +int(_channel[ch]._vibratoWaveform[_channel[ch]._currentVibrato]
                *amplitude));
         // Step vibrato
         _channel[ch]._currentVibrato += step;
         _channel[ch]._currentVibrato &= 63;
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::pitchSlidePlusVolumeSlide:
   {
      int portParameter 
            = _channel[ch]._defaultParameter[ModNoteData::pitchSlide];
      // No default for volume slide
      int volumeChange = (thisParameter & 0xF0)?
                        (thisParameter >> 4) & 0xF : 
                        -(thisParameter & 0xF);
      AudioSample *currentSampleBuffer = sampleBuffer;
      int i=0;
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         // Adjust the volume
         currentLiveNote.SetModVolume(_channel[ch]._currentVolume
                                       +volumeChange);
         // Slide the pitch
         if (_channel[ch]._pitchGoal == 0) {
            // Do nothing
         } else if (currentNote._period > _channel[ch]._pitchGoal) {
            currentNote._period -= portParameter;
            if (currentNote._period <= _channel[ch]._pitchGoal) {
               currentNote._period = _channel[ch]._pitchGoal;
               _channel[ch]._pitchGoal = 0;
            }
         } else {
            currentNote._period += _channel[ch]._pitchGoal;
            if (currentNote._period >= _channel[ch]._pitchGoal) {
               currentNote._period = _channel[ch]._pitchGoal;
               _channel[ch]._pitchGoal = 0;
            }
         }
         currentLiveNote.SetModPeriod(currentNote._period);
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::vibratoPlusVolumeSlide:
   {
      // Pick up vibrato parameter from last vibrato
      int vibratoParameter 
            = _channel[ch]._defaultParameter[ModNoteData::vibrato];
      int step = (vibratoParameter >> 4)&0xF;
      int amplitude = (vibratoParameter&0xF)*2;
      // No default for volume slide
      int volumeChange = (thisParameter & 0xF0)?
                        (thisParameter >> 4) & 0xF : 
                        -(thisParameter & 0xF);
      AudioSample *currentSampleBuffer = sampleBuffer;
      // Set vibrato
      currentLiveNote.SetModPeriod(currentNote._period
         +int(_channel[ch]._vibratoWaveform[_channel[ch]._currentVibrato]
              *amplitude));
      // Step vibrato
      _channel[ch]._currentVibrato += step;
      _channel[ch]._currentVibrato &= 63;

      int i=0;      
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         // Adjust the volume
         currentLiveNote.SetModVolume(_channel[ch]._currentVolume
                                       +volumeChange);
         // Set vibrato
         currentLiveNote.SetModPeriod(currentNote._period
            +int(_channel[ch]._vibratoWaveform[_channel[ch]._currentVibrato]
                *amplitude));
         // Step vibrato
         _channel[ch]._currentVibrato += step;
         _channel[ch]._currentVibrato &= 63;
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::tremolo:
   cerr << "Effect tremolo exercised.\n";
   {
      if (thisParameter == 0)
         thisParameter = defaultParameter;
      int step = (thisParameter >> 4)&0xF;
      int amplitude = (thisParameter&0xF)*2*2;
      AudioSample *currentSampleBuffer = sampleBuffer;
      // Set volume
      currentLiveNote.SetModVolume(_channel[ch]._currentVolume
         +int(_channel[ch]._tremoloWaveform[_channel[ch]._currentTremolo]
             *amplitude));
      // Step tremolo
      _channel[ch]._currentTremolo += step;
      _channel[ch]._currentTremolo &= 63;

      int i=0;      
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         // Set volume
         currentLiveNote.SetModVolume(_channel[ch]._currentVolume
            +int(_channel[ch]._tremoloWaveform[_channel[ch]._currentTremolo]
                *amplitude));
         // Step tremolo
         _channel[ch]._currentTremolo += step;
         _channel[ch]._currentTremolo &= 63;
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::setSampleOffset:
   currentLiveNote.Restart();
   currentLiveNote.SetSampleOffset(thisParameter*512);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::volumeSlide:
   {
      AudioSample *currentSampleBuffer = sampleBuffer;
      int volumeChange = (thisParameter & 0xF0)?
                        (thisParameter >> 4) & 0xF : 
                        -(thisParameter & 0xF);
      int i=0;
      for(;i<_samplesPerBeat-_samplesPerTick;i+=_samplesPerTick) {
         currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerTick);
         currentSampleBuffer += _samplesPerTick;
         _channel[ch]._currentVolume += volumeChange;
         currentLiveNote.SetModVolume(_channel[ch]._currentVolume);
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::farJump:
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setVolume:
   currentLiveNote.SetModVolume(thisParameter);
   _channel[ch]._currentVolume=thisParameter;
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::smallJump:
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setFilter:
   cerr << "Effect setFilter not implemented.\n";
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::pitchUp:
   cerr << "Effect pitchUp exercised.\n";
   currentNote._period -= thisParameter;
   currentLiveNote.SetModPeriod(currentNote._period);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::pitchDown:
   cerr << "Effect pitchDown exercised.\n";
   currentNote._period += thisParameter;
   currentLiveNote.SetModPeriod(currentNote._period);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setGlissando:
   cerr << "Effect setGlissando not implemented.\n";
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setVibrato:
   cerr << "Effect setVibrato exercised.\n";
   // Random selection always selects 0
   if ((thisParameter & 3) == 3) thisParameter &= 4;
   _channel[ch]._vibratoWaveform 
      = &(vibratoTremoloWaveforms[thisParameter & 0x3][0]);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setFinetune:
   cerr << "Effect setFinetune not implemented.\n";
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::patternLoop:
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setTremolo:
   cerr << "Effect setTremolo exercised.\n";
   // Random selection always selects 0
   if ((thisParameter & 3) == 3) thisParameter &= 4;
   _channel[ch]._tremoloWaveform 
      = &(vibratoTremoloWaveforms[thisParameter & 0x3][0]);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::retrigger:
   // Restart note several times
   {
      if (thisParameter == 0) thisParameter = defaultParameter;
      AudioSample *currentSampleBuffer = sampleBuffer;
      int samplesPerNote = _samplesPerTick * thisParameter;
      int i=0;
      for(;i<_samplesPerBeat-samplesPerNote-_samplesPerTick/2
            ;i+=samplesPerNote) {
         currentLiveNote.AddSamples(currentSampleBuffer,samplesPerNote);
         currentSampleBuffer += samplesPerNote;
         currentLiveNote.Restart();
      };
      currentLiveNote.AddSamples(currentSampleBuffer,_samplesPerBeat-i);
   }
   break;
case ModNoteData::volumeUp:
   _channel[ch]._currentVolume += thisParameter;
   currentLiveNote.SetModVolume(_channel[ch]._currentVolume);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::volumeDown:
   cerr << "Effect volumeDown exercised.\n";
   currentLiveNote.SetModVolume(_channel[ch]._currentVolume-thisParameter);
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::cutNote:
   // Play for `parameter' ticks, then cut volume to zero
   // and continue playing
   cerr << "Effect cutNote exercised.\n";
   {
      if (thisParameter == 0) thisParameter = defaultParameter;
      int playSamples = _samplesPerTick * thisParameter;
      if (playSamples < _samplesPerBeat) {
         currentLiveNote.AddSamples(sampleBuffer, playSamples);
         _channel[ch]._currentVolume = 0;
         currentLiveNote.SetModVolume(_channel[ch]._currentVolume);
         currentLiveNote.AddSamples(sampleBuffer+playSamples,
                              _samplesPerBeat-playSamples);
      } else { // Play whole beat
         currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
      }
   }
   break;
case ModNoteData::delayNote:
   {
      if (thisParameter == 0) thisParameter = defaultParameter;
      int skipSamples = _samplesPerTick * thisParameter;
      if (skipSamples < _samplesPerBeat) 
         currentLiveNote.AddSamples(sampleBuffer+skipSamples,
                              _samplesPerBeat-skipSamples);
   }
   break;
case ModNoteData::delayPattern:
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::invertLoop:
   cerr << "Effect invertLoop not implemented.\n";
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
case ModNoteData::setTempo:
   currentLiveNote.AddSamples(sampleBuffer,_samplesPerBeat);
   break;
         default:
            cerr << "Internal error: illegal effect code.\n";
            break;
         }
         // Save the current (updated) parameter as the new default.
         _channel[ch]._defaultParameter[currentNote._effect] =thisParameter;
      }
      // Save the defaults for next time
      _channel[ch]._currentNote = currentNote;
   }

   // Set variables for GetSamples
   _sampleLength = _samplesPerBeat;
   _sampleStartLeft = _sampleBufferLeft;
   _sampleStartRight = _sampleBufferRight;
}
