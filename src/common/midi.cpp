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
#include <istream>
#include <cstdio>
#include <cmath>

#include "audio.h"
#include "instrumt.h"
#include "sampled.h"
#include "plucked.h"
#include "midi.h"

#define ChunkName(a,b,c,d) (                 \
    ((static_cast<unsigned long>(a)&255)<<24)          \
  + ((static_cast<unsigned long>(b)&255)<<16)          \
  + ((static_cast<unsigned long>(c)&255)<<8)           \
  + ((static_cast<unsigned long>(d)&255)))

bool IsMidiFile(istream &file) {
   file.seekg(0); // Seek to beginning of file
   unsigned long form = ReadIntMsb(file,4);
   return (form == ChunkName('M','T','h','d'));
}
AbstractInstrument *MidiInstrumentMap::Instrument(
               int bank, int program, int note) {
   
{
   if((bank == gmRhythmBank)&&(program==0)) {
      static bool instrumentPrinted[128];
      const struct {
         int note;
         const char *name;
      } instruments[] = {
       {35,"Acoustic Bass Drum"}, {36,"Bass Drum 1"},
       {37,"Side Stick"}, {38,"Acoustic Snare"}, {39,"Hand Clap"},
       {40,"Electric Snare"}, {41,"Low Floor Tom"},
       {42,"Closed Hi-Hat"}, {43,"High Floor Tom"},
       {44,"Pedal Hi-Hat"}, {45,"Low Tom"}, {46,"Open Hi-Hat"},
       {47,"Low-Mid Tom"}, {48,"Hi-Mid Tom"}, {49,"Crash Cymbal 1"},
       {50,"High Tom"}, {51,"Ride Cymbal 1"}, {52,"Chinese Cymbal"},
       {53,"Ride Bell"}, {54,"Tambourine"}, {55,"Splash Cymbal"},
       {56,"Cowbell"}, {57,"Crash Cymbal 2"}, {58,"Vibraslap"},
       {59,"Ride Cymbal 2"}, {60,"Hi Bongo"}, {61,"Low Bongo"},
       {62,"Mute Hi Conga"}, {63,"Open Hi Conga"}, {64,"Low Conga"},
       {65,"High Timbale"}, {66,"Low Timbale"}, {67,"High Agogo"},
       {68,"Low Agogo"}, {69,"Cabasa"}, {70,"Maracas"},
       {71,"Short Whistle"}, {72,"Long Whistle"}, {73,"Short Guiro"},
       {74,"Long Guiro"}, {75,"Claves"}, {76,"Hi Wood Block"},
       {77,"Low Wood Block"}, {78,"Mute Cuica"}, {79,"Open Cuica"},
       {80,"Mute Triangle"}, {81,"Open Triangle"}, {0,0}
      };

      if (!instrumentPrinted[note]) {
         TRACE0( "Instrument: " );
         int i=0;
         while( (instruments[i].note != 0)
               && (instruments[i].note != note+1)) {
            i++;
         }

         if ((instruments[i].note == 0))
            TRACE0( "Unknown Percussion." );
         else
            TRACE2( "%s (Note %s)", instruments[i].name, note );
         instrumentPrinted[note] = true;
      }
   }

   if (bank == gmMelodyBank) {
      static bool instrumentPrinted[128];
      const char *instruments[] = {
         "Acoustic Grand Piano", "Bright Acoustic Piano",
         "Electric Grand Piano", "Honky-tonk Piano", "Electric Piano 1",
         "Electric Piano 2", "Harpsichord", "Clavinet", "Celesta",
         "Glockenspiel", "Music Box", "Vibraphone", "Marimba",
         "Xylophone", "Tubular Bells", "Dulcimer", "Drawbar Organ",
         "Percussive Organ", "Rock Organ", "Church Organ",
         "Reed Organ", "Accordion", "Harmonica", "Tango Accordion",
         "Acoustic Guitar (nylon)", "Acoustic Guitar (steel)",
         "Electric Guitar (jazz)", "Electric Guitar (clean)",
         "Electric Guitar (mute)", "Overdriven Guitar",
         "Distortion Guitar", "Guitar Harmonics", "Acoustic Bass",
         "Electric Bass (finger)", "Electric Bass (pick)",
         "Fretless Bass", "Slap Bass 1", "Slap Bass 2", "Synth Bass 1",
         "Synth Bass 2", "Violin", "Viola", "Cello", "Contrabass",
         "Tremolo Strings", "Pizzicato Strings", "Orchestral Harp",
         "Timpani", "String Ensemble 1", "String Ensemble 2",
         "SynthStrings 1", "SynthStrings 2", "Choir Aahs", "Voice Oohs",
         "Synth Voice", "Orchestra Hit", "Trumpet", "Trombone", "Tuba",
         "Muted Trumpet", "French Horn", "Brass Section", "SynthBrass 1",
         "SynthBrass 2", "Soprano Sax", "Alto Sax", "Tenor Sax",
         "Baritone Sax", "Oboe", "English Horn", "Bassoon",
         "Clarinet", "Piccolo", "Flute", "Recorder", "Pan Flute",
         "Blown Bottle", "Shakuhachi", "Whistle", "Ocarina",
         "Lead 1 (square)", "Lead 2 (sawtooth)", "Lead 3 (calliope)",
         "Lead 4 (chiff)", "Lead 5 (charang)", "Lead 6 (voice)",
         "Lead 7 (fifths)", "Lead 8 (bass+lead)", "Pad 1 (new age)",
         "Pad 2 (warm)", "Pad 3 (polysynth)", "Pad 4 (choir)",
         "Pad 5 (bowed)", "Pad 6 (metallic)", "Pad 7 (halo)",
         "Pad 8 (sweep)", "FX 1 (rain)", "FX 2 (soundtrack)",
         "FX 3 (crystal)", "FX 4 (atmosphere)", "FX 5 (brightness)",
         "FX 6 (goblins)", "FX 7 (echoes)", "FX 8 (sci-fi)", "Sitar",
         "Banjo", "Shamisen", "Koto", "Kalimba", "Bag pipe", "Fiddle",
         "Shanai", "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock",
         "Taiko Drum", "Melodic Tom", "Synth Drum", "Reverse Cymbal",
         "Guitar Fret Noise", "Breath Noise", "Seashore",
         "Bird Tweet", "Telephone Ring", "Helicopter", "Applause",
         "Gunshot"
      };
      if(!instrumentPrinted[program]) {
         TRACE1( "Instrument: %s", instruments[program] );
         TRACE1( " (Program %s)", program );
         instrumentPrinted[program] = true;
      }
   }
}
   return _instr;
};
static unsigned long ReadVarInt(istream &in, int *size) {
   *size = 0;
   unsigned long l, retVal = 0;
   do {
      l = static_cast<unsigned long>(in.get()) & 255;
      (*size)++;
      if (*size > 6) {
		throw new CSoundException( CSoundException::causeUnterminatedVariableInteger );
      }
      retVal = (retVal << 7) + (l & 0x7F);
   } while (l & 0x80);
   return retVal;
}
MidiChannelStandard::
MidiChannelStandard(MidiRead *midiRead,
                    int currentBank,
                    float masterVolume) {
   _masterVolume = masterVolume;
   _midiRead = midiRead;
   _defaultInstrumentBank = currentBank;
   _currentInstrumentBank = currentBank;
   _currentInstrumentNumber = 0;
   _currentTuning = 0;
   for (int i=0; i<128;i++)
      _note[i] = 0;
   ResetControllers();
}


MidiChannelStandard::~MidiChannelStandard() { // Clean up
   for (int i=0; i<128; i++)
      if(_note[i]) {
         delete _note[i];
         _note[i] = 0;
      }
}
void MidiChannelStandard::Program(int instrument) {
   _currentInstrumentNumber = instrument;
   _currentInstrumentBank = _controllerLong[0];
}

void MidiChannelStandard::StartNote(int note, int velocity) {
   if (_note[note]) delete _note[note];

   // Select the correct instrument
   AbstractInstrument *instr = 
      _midiRead->InstrumentMap()->Instrument(_currentInstrumentBank,
                                  _currentInstrumentNumber,note);

   // Get the tuning for this note
   float pitch = _midiRead->TuningMap()->Tuning(_currentTuning,note);

   _note[note] = instr->NewNote(pitch,
                         velocity*_channelVolume*_masterVolume);
}

void MidiChannelStandard::EndNote(int note, int velocity) {
   if(_note[note])
      _note[note]->EndNote(velocity/127.0);
}

void MidiChannelStandard::AllNotesOff() {
   for (int i=0; i<128;i++)
      if(_note[i])
         _note[i]->EndNote(0.0);
}
// I don't yet support aftertouch or pitch wheel
void MidiChannelStandard::KeyAftertouch(int note, int velocity) {
   //cerr << "Key Aftertouch: " << note << " velocity: " << velocity << "\n";
}

void MidiChannelStandard::ChannelAftertouch(int velocity) {
   //cerr << "Channel Aftertouch: velocity: " << velocity << "\n";
}

void MidiChannelStandard::PitchBend(int) {
}
size_t MidiChannelStandard::AddSamples(AudioSample *buffer,
                                       size_t numberSamples) {
   for(int i=0; i< 128; i++) {
      if (_note[i]) {
         size_t samplesRead =
            _note[i]->AddSamples(buffer,numberSamples);
         if (samplesRead < numberSamples) { // Note finished?
            delete _note[i];
            _note[i] = 0;
         }
      }
   }
   return numberSamples;
}
void MidiChannelStandard::Mode(int mode, int value) {
   // 14-bit controllers
   if (mode < 32) // 0--31 are most-significant bits
      _controllerLong[mode] = value << 7;
   if ((mode >= 32) && (mode < 64)) // least-significant bits
      _controllerLong[mode-32] =
            (_controllerLong[mode-32] & 0x3F80) + value;

   // 7-bit controllers
   if ((mode >= 64) && (mode < 96)) // single-byte controllers
      _controllerShort[mode-64] = value;

   // Parameter settings
   if (mode == 96) {    // 96 is Data Increment
      _registeredParameter[_currentRegisteredParameter]++;
      _controllerLong[6] = _registeredParameter[_currentRegisteredParameter];
   }
   if (mode == 97) {    // 97 is Data Decrement
      _registeredParameter[_currentRegisteredParameter]--;
      _controllerLong[6] = _registeredParameter[_currentRegisteredParameter];
   }
   // 98 is Non-Registered Parameter LSB
   // 99 is Non-Registered Parameter MSB
   if (mode == 100) {  // Registered Parameter LSB
      _currentRegisteredParameter =
         (_currentRegisteredParameter & 0x3F80)+(value);
      _controllerLong[6] = _registeredParameter[_currentRegisteredParameter];
   }
   if (mode == 101) {  // Registered Parameter MSB
      _currentRegisteredParameter = value<<7;
      _controllerLong[6] = _registeredParameter[_currentRegisteredParameter];
   }

   /************************************/
   /* Channel mode messages (120--127) */
   /************************************/
   // 120 is All Sound Off
   // 121 is Reset All Controllers
   if (mode == 121) ResetControllers();
   // 123 is All Notes Off (so are 124--127)
   if (mode >= 123) AllNotesOff();
   // 124 is Omni Mode Off
   // 125 is Omni Mode on
   // 126 is Mono Mode On (one note per channel)
   // 127 is Poly Mode On (multiple notes per channel)
}

void MidiChannelStandard::ResetControllers() {
   int i;
   for(i=0;i<32;i++) {
      _controllerLong[i] = 0;
      _controllerShort[i] = 0;
   }

   for(i=0;i<8;i++) {
      _registeredParameter[i] = 0;
   }
   _controllerLong[0] = _defaultInstrumentBank;
   _controllerLong[7] = 100 << 7; // Default volume
   _controllerLong[8] = 0x2000; // Balance control: 1/2 = equal output
   _controllerLong[10] = 0x2000; // Pan control: 1/2 = centered
   _controllerLong[11] = 0x2000; // Average expression

   _currentRegisteredParameter = 0;
   _registeredParameter[0] = 0; // Pitch bend default: +-2 semitones
   _registeredParameter[1] = 0x2000; // Default fine tuning
   _registeredParameter[2] = 0x2000; // Default coarse tuning

   // Update some miscellaneous variables
   _channelVolume = _controllerLong[7]/16384.0;
}
void MidiRead::ReadHeader(void) {
   unsigned long chunkType = ReadIntMsb(_stream,4);
   unsigned long bytesRemaining = ReadIntMsb(_stream,4);
   if (chunkType != ChunkName('M','T','h','d')) {
	  throw new CSoundException( CSoundException::causeFirstChunkMustBeMthd );
   }

   _fileType = ReadIntMsb(_stream,2);
   _numberTracks = ReadIntMsb(_stream,2);
   _timeFormat = ReadIntMsb(_stream,2);
   bytesRemaining -= 6;

   // Sanity check the file type and number of tracks            
   switch(_fileType) {
   case 0:
      TRACE0( "Tracks: 1");
      if (_numberTracks != 1)
         TRACE1( "But it has %d tracks?!?!)", _numberTracks );
      break;
   case 1:
      TRACE1( "Tracks: %d simultaneous",  _numberTracks );
      break;
   case 2:
      TRACE1( "Tracks: %d independent", _numberTracks );
      break;
   default:
      TRACE1( "Unknown file type: %d", _fileType );
   }

   // Dump time format
   if (_timeFormat < 0) { // SMPTE time code
      TRACE1( "Time Format: %d (SMPTE)", _timeFormat );
   } else { // duration time code
      TRACE1( "Time Format: %d ticks/quarter note", _timeFormat );
   }

   SkipBytes(_stream,bytesRemaining);
}
void MidiRead::ReadTracks() {
   int tracksRead = 0;
   // Read rest of chunks
   while (!_stream.eof() && (tracksRead < _numberTracks)) {
      unsigned long chunkType = ReadIntMsb(_stream,4);
      long bytesRemaining = ReadIntMsb(_stream,4);
      if (_stream.eof()) continue; // Skip rest of loop

      // If this isn't an MTrk chunk, skip it
      if (chunkType != ChunkName('M','T','r','k')) {   
         char name[5];
         name[0] = chunkType >> 24;     name[1] = chunkType >> 16;
         name[2] = chunkType >> 8;      name[3] = chunkType;
         name[4] = 0;
         TRACE1( "Unrecognized chunk '%s'", name );
         SkipBytes(_stream,bytesRemaining);
         continue; // Back to top of while loop
      }

      tracksRead++;

      // Only read first track in a Type 2 file
      if ((_fileType == 2) & (tracksRead > 1)) {
         SkipBytes(_stream,bytesRemaining);
         continue;
      }

      MidiEvent *pLastEvent = 0;

      while((bytesRemaining > 0)&&(!_stream.eof())) {
         MidiEvent *pEvent = new MidiEvent;
         pEvent->track = tracksRead;
         
{  
   static const char eventLength[] = {
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0x80 - 0x8F
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0x90 - 0x9F
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xA0 - 0xAF
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xB0 - 0xBF
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xC0 - 0xCF
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0xD0 - 0xDF
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xE0 - 0xEF
   0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xF0 - 0xFF
   };

   int sizeRead;
   pEvent->delay = ReadVarInt(_stream,&sizeRead);
   bytesRemaining -= sizeRead; // Count the delta time
   int dataRead = 0;   // Number of bytes of data read so far
   int byte = _stream.get();
   bytesRemaining--;  // Count this byte
   if (byte >= 0x80) { // this is a new status
      pEvent->status = byte;
   } else { // running status
      pEvent->status = pLastEvent->status; // re-use last status
      pEvent->data[dataRead++] = byte; // this is first data byte
   }
   while(dataRead < eventLength[pEvent->status - 0x80]) {
      pEvent->data[dataRead++] = _stream.get();
      bytesRemaining--;
   }
}
         
// Read a sysex event from _stream:
//  * store event into pEvent structure
//  * decrease bytesRemaining appropriately
if (   (pEvent->status == 0xF0)
    || (pEvent->status == 0xF7) ) {
   int sizeRead;
   unsigned long msgLength = ReadVarInt(_stream,&sizeRead);
   bytesRemaining-= sizeRead;
   pEvent->metaData = new MidiExtendedEventData;
   pEvent->metaData->length = msgLength;
   pEvent->metaData->data = new AudioByte[msgLength];
   _stream.read(reinterpret_cast<char *>(pEvent->metaData->data),
               msgLength);
   bytesRemaining-= msgLength;
}
         
// Read a meta event from _stream:
//  * store event into pEvent structure
//  * decrease bytesRemaining appropriately
if (pEvent->status == 0xFF) {
   pEvent->data[0] = _stream.get(); // Meta event type
   bytesRemaining -= 1;
   int sizeRead;
   unsigned long msgLength = ReadVarInt(_stream,&sizeRead);
   bytesRemaining -= sizeRead;
   pEvent->metaData = new MidiExtendedEventData;
   pEvent->metaData->length = msgLength;
   pEvent->metaData->data = new AudioByte[msgLength+1];
   _stream.read(reinterpret_cast<char *>(pEvent->metaData->data),
               msgLength);
   bytesRemaining -= msgLength;
   pEvent->metaData->data[msgLength] = 0; // Add null terminator
   
if(pEvent->data[0] == 3) {
   TRACE1( "Track %d: ", static_cast<int>(pEvent->track) );
   TRACE1( "%s", reinterpret_cast<char *>(pEvent->metaData->data) );
} else if (pEvent->data[0] < 16) {
   const char *textPrefix[] = { "",
      "Comment: ", "Copyright: ", "Track Name: ",
      "Instrument Name ", "Lyric: ", "Marker: ",
      "Cue Point: ", "Text: ", "Text: ", "Text: ", 
      "Text: ", "Text: ", "Text: ", "Text: ", "Text: "};
   TRACE1( "%s", textPrefix[ pEvent->data[0] ]);
   TRACE1( "%s", reinterpret_cast<char *>(pEvent->metaData->data) );
}
}
         
if (   !_events
       || (!pLastEvent && (_events->delay > pEvent->delay))) {
   // this event goes at front of list
   pEvent->next = _events;
   _events = pLastEvent = pEvent;
} else { // Doesn't go at beginning of list.
   if (!pLastEvent) { // Skip past first event in list
      pLastEvent = _events;
      pEvent->delay -= _events->delay;
   }
   // Skip earlier events
   while( pLastEvent->next 
          && pLastEvent->next->delay <= pEvent->delay ) {
      pEvent->delay -= pLastEvent->next->delay;
      pLastEvent = pLastEvent->next;
   }
   // Splice into list
   pEvent->next = pLastEvent->next;
   pLastEvent->next = pEvent;
}
if (pEvent->next) // Reduce delay of next item, if any
   pEvent->next->delay -= pEvent->delay;
pLastEvent = pEvent; // Last event in this track
         if(bytesRemaining < 0) {
            TRACE0( "Contents of track chunk were too long." );
         }
      }
   }
}
void MidiRead::PostProcess() {
   TRACE0("Analyzing MIDI file . . . ");
   double samplesPerTick; // Samples to play for each MIDI tick
   double samplesRemainingFraction; // fractional samples left over

   
if (_timeFormat < 0) {          // SMPTE Time format
   
int frameCode = (-_timeFormat) >> 8;
float framesPerSecond;
switch(frameCode) {
case 24: framesPerSecond = 24.0; break;
case 25: framesPerSecond = 25.0; break;
case 29: framesPerSecond = 29.97F; break; // ``drop-frame'' format
case 30: framesPerSecond = 30.0; break;
default: 
   TRACE0( "Illegal SMPTE frame code." );
   framesPerSecond = 30.0;
   break;
}
int ticksPerFrame = _timeFormat & 0xFF;
samplesPerTick = 
      SamplingRate()       // samples per second
    / framesPerSecond 
    / ticksPerFrame;
} else {                        // Musical Time Format
   
samplesPerTick = 
   60.0 / 120  // Default is 120 quarter notes per minute
   *  SamplingRate()     // samples per second
   / _timeFormat;        // Ticks/quarter note
}

   samplesRemainingFraction = 0.0;

   // For finding the maximum volume
   long currentVolume = 0;
   long maxTotalVolume = 0;
   long simultaneousNotes = 0;
   long maxSimultaneousNotes = 0;
   char volume[16][128]; // Volume of each playing note
   for(int i=0;i<16;i++)
      for(int j=0;j<128;j++)
         volume[i][j] = 0;

   // For finding out which channels are being used
   bool possible[16];
   bool active[16];
   for(int j=0;j<16;j++) {
      active[j] = false;
      possible[j] = true;
   }

   MidiEvent *pEvent = _events;
   MidiEvent *pLastEvent = 0;

   while(pEvent) {
      // Convert delay from ticks to audio samples
      if (pEvent->delay > 0) {
         float samples = pEvent->delay * samplesPerTick
            + samplesRemainingFraction;
         pEvent->delay = static_cast<unsigned long>(samples);
         samplesRemainingFraction = samples - pEvent->delay;
      }

      // Track maximum volume
      int ch = pEvent->status & 0x0F;
      switch(pEvent->status & 0xF0) {
      case 0x90:
         if ((pEvent->data[1] != 0) && possible[ch]) {
            active[ch] = true; // This channel is used
            if (volume[ch][ pEvent->data[0] ]) { // Note already on?!?!
               // Turn it off before starting over...
               currentVolume -= volume[ch][ pEvent->data[0] ];
               simultaneousNotes--;
            }
            volume[ch][ pEvent->data[0] ]  = pEvent->data[1];
            currentVolume += volume[ch][ pEvent->data[0] ];
            if (currentVolume > maxTotalVolume)
               maxTotalVolume = currentVolume;
            simultaneousNotes++;
            if (simultaneousNotes > maxSimultaneousNotes)
               maxSimultaneousNotes = simultaneousNotes;
            break;
         }
         // Note on with zero velocity is really a note off
         // Convert it into a real note off
         pEvent->status = 0x80 | ch;
         pEvent->data[1] = 64; // With average velocity
         // Fall through and process it as a Note Off
      case 0x80:
         currentVolume -= volume[ch][ pEvent->data[0] ];
         simultaneousNotes--;
         volume[ch][ pEvent->data[0] ] = 0;
         break;
      }
      
if(   (pEvent->status == 0xFF) // Meta event
   && (pEvent->data[0] == 127) // Sequencer-specific
   && (pEvent->metaData->length >= 3)
   && (pEvent->metaData->data[0] == 0)  // three-byte manufacturer ID
   && (pEvent->metaData->data[1] == 0)  // 0 0 65 = Microsoft
   && (pEvent->metaData->data[2] == 65)
   ) {
      TRACE0( "This is an MPC MIDI file." );
      // Blank out channels 11 through 16 (we're an Extended MIDI device)
      for (int i=10;i<16;i++)
         if(!_channel[i]) {
            _channel[i] = new MidiChannelSilent();
            possible[i] = false; // Don't count notes on these channels
         }
}
      
if (   (pEvent->status == 0xFF)     // meta event
    && (pEvent->data[0] == 0x51)) { // Tempo meta event
   // argument is microseconds per beat
   float beatsPerSecond =  1.0E6  
         / BytesToIntMsb(pEvent->metaData->data,3);
   if (_timeFormat > 0) { // ``musical'' time specification
      samplesPerTick = 
         SamplingRate()   // samples per second
         / beatsPerSecond
         / _timeFormat;   // ticks per beat
   }
}
      pLastEvent = pEvent;
      pEvent = pEvent->next;
   }
   TRACE0( ". . done." );
   TRACE1( "Maximum Simultaneous Notes: %d", maxSimultaneousNotes  );
   TRACE1( "Maximum Total volume: %d", maxTotalVolume );

   
{
   // Fill up empty channels
   for(int ch=0; ch< 16; ch++) {
      if (_channel[ch]) continue; // Already there!
      if (!active[ch])
         _channel[ch] = new MidiChannelSilent();
      else {
         int instrumentSet;
         if (ch==10) instrumentSet = MidiInstrumentMap::gmRhythmBank;
         else       instrumentSet = MidiInstrumentMap::gmMelodyBank;
         _channel[ch] = new MidiChannelStandard(
                              this,
                              instrumentSet,
                              1.0/maxTotalVolume);
      }
   }
}

   // Set sampling rate on the instrument map
   _instrumentMap->SamplingRate(SamplingRate());
}
size_t MidiRead::GetSamples(AudioSample *buffer, size_t numSamples) {
   if (_events == 0) { // If I haven't yet read the file, do so.
      ReadHeader(); // Slurp in MIDI events from file
      ReadTracks();
      PostProcess(); // Convert to sample-based timing
      _currentEvent = _events; // Start with first event
      _samplesRemaining = 0;
   }
   for(size_t i=0; i<numSamples; i++)
      buffer[i]=0;
   size_t samplesReturned = 0;
   while (_currentEvent && (numSamples>0)) {
      if (_samplesRemaining > 0) {
         size_t samplesToPlay = numSamples;
         if (samplesToPlay > _samplesRemaining)
            samplesToPlay = _samplesRemaining;
         for (int i=0;i<16;i++)
            _channel[i]->AddSamples(buffer,samplesToPlay);
         buffer += samplesToPlay;
         samplesReturned += samplesToPlay;
         _samplesRemaining -= samplesToPlay;
         numSamples -= samplesToPlay;
         if (numSamples == 0)
            return samplesReturned;
      }

      _samplesRemaining += _currentEvent->delay;

      
int ch = _currentEvent->status & 0xF;
switch(_currentEvent->status & 0xF0) {
case 0x80: // Note-off event
   _channel[ch]->EndNote(_currentEvent->data[0],_currentEvent->data[1]);
   break;
case 0x90: // Note-on event
   if (_currentEvent->data[1] == 0)
      _channel[ch]->EndNote(_currentEvent->data[0], 64);
   else
      _channel[ch]->StartNote(_currentEvent->data[0],
                              _currentEvent->data[1]);
   break;
case 0xA0: // Key Aftertouch
   _channel[ch]->KeyAftertouch(_currentEvent->data[0],
                               _currentEvent->data[1]);
   break;
case 0xB0: // Mode change
   _channel[ch]->Mode(_currentEvent->data[0],
                      _currentEvent->data[1]);
   break;
case 0xC0: // Program channel
   _channel[ch]->Program(_currentEvent->data[0]);
   break;
case 0xD0: // Channel Aftertouch
   _channel[ch]->ChannelAftertouch(_currentEvent->data[0]);
   break;
case 0xE0: // Pitch Wheel
   _channel[ch]->PitchBend(_currentEvent->data[1]*128
                           + _currentEvent->data[0]);
   break;
case 0xF0: // Special
   if(_currentEvent->status == 0xFF) { // Meta event
      switch(_currentEvent->data[0]) {
      case 0: break; // Track Sequence Number
      case 1: case 2: case 3: case 4: case 5:
      case 6: case 7: case 8: case 9: case 10:
      case 11: case 12: case 13: case 14: case 15:
         break; // Text Comments
      case 47: break; // End of Track
      case 81: break; // Tempo
      case 88: break; // Time Signature
      case 89: break; // Key Signature
      case 127: break; // Sequencer-Specific
      default: fprintf(stderr,"Track %2d: (Meta 0x%02x)\n",
                     _currentEvent->track,
                     _currentEvent->data[0]);
      }
   } else if(_currentEvent->status == 0xF0) { // sysex event
      fprintf(stderr,"Track %2d: (Sysex Event)\n",
                     _currentEvent->track);
   } else if(_currentEvent->status == 0xF7) { // special sysex event
      fprintf(stderr,"Track %2d: (Special Sysex Event)\n",
                     _currentEvent->track);
   } else {
      fprintf(stderr,"Track %2d: ",_currentEvent->track);
      fprintf(stderr, "Status: 0x%02x\n", _currentEvent->status);
   }
   break;
default: // Some event not covered above??
   fprintf(stderr,"Track %2d, Bad Status: 0x%x\n",
           _currentEvent->track,_currentEvent->status);
   break;
}
      _currentEvent = _currentEvent->next;
   }

   return samplesReturned;
}

// Read file
MidiRead::MidiRead(istream &s):AudioAbstract(), _stream(s) {
   TRACE0( "File Format: MIDI" );
   _currentEvent = _events = 0;
   for(int i=0;i<16;i++) _channel[i] = 0;
   _instrumentMap = new MidiInstrumentMap;
   _tuningMap = new MidiTuningMap;
}

MidiRead::~MidiRead() {
   MidiEvent *pEvent, *pNext = _events;
   while (pNext) { // Delete event list
      pEvent = pNext;
      pNext = pEvent->next;
      delete pEvent;
   }
   for(int i=0; i< 16; i++) { // Delete channel objects
      if (_channel[i])
         delete _channel[i];
   }
   delete _instrumentMap; // Delete instrument map
   delete _tuningMap; // Delete tuning map
};
