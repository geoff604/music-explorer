
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
#include "aplayer.h"

// Try to select appropriate player for Win32, Macintosh, or UNIX/NAS
#if defined(_WIN32)
#include "winplayr.h"
typedef WinPlayer Player;
#elif defined(macintosh)
#include "macplayr.h"
typedef MacPlayer Player;
#else
#include "nasplayr.h"
typedef NasPlayer Player;
#endif

#include "instrumt.h"
#include "sampled.h"
#include "au.h"
#include "plucked.h"
#include <cstdlib>
#include <fstream>

class PlayInstrument: public AudioAbstract {
private:
   long _samplesRemaining;
   AbstractInstrument *_instr;
   AbstractNote *_notes[5];
   float _pitches[5];
   float _volumes[5];
protected:
   void MinMaxChannels(int *min, int *max, int *preferred) {
      *min = *max = *preferred = 1;
   };
public:
   void Init() {
      _samplesRemaining = 44100 * 5; // Play for five seconds
      for(int i=0;i<5;i++) {
         _notes[i] = 0;
         _pitches[i] = _volumes[i] = 0;
      }
   }
   PlayInstrument(AbstractInstrument *instr) {
      Init(); _instr = instr;
   }
   void AddNote(float pitch, float volume) {
      int i=0;
      while(_pitches[i] != 0)
         i++;
      _pitches[i] = pitch;
      _volumes[i] = volume;
   }
   ~PlayInstrument() {
      for(int i=0;i<5;i++)
         if (_notes[i]) delete _notes[i];
   }
   size_t GetSamples(AudioSample *buffer, size_t numSamples) {
      for(size_t s=0; s<numSamples; s++) // Zero the buffer
         buffer[s] = 0;
      size_t maxSamples = 0;
      int i;
      for(i=0;i<5;i++) { // Create notes if necessary
         if(_pitches[i] && !_notes[i]) // Create note if necessary
            _notes[i] = _instr->NewNote(_pitches[i],_volumes[i]);
      }

      for(i=0;i<5;i++) { // Play each note into buffer
         if(_notes[i]) { // If this note exists, play it
            size_t samplesRead = 0;
            // If it's time for note to start decaying...
            if((_samplesRemaining >= 0)
               && (static_cast<long>(numSamples) > _samplesRemaining)) {
               _notes[i]->AddSamples(buffer,_samplesRemaining);
               _notes[i]->EndNote(0.5); // Tell note to start decaying
               samplesRead
                  = _samplesRemaining + _notes[i]->AddSamples(
                  buffer+_samplesRemaining,numSamples-_samplesRemaining);
            } else {
               samplesRead = _notes[i]->AddSamples(buffer,numSamples);
            }
            if (samplesRead < numSamples) { // This note done
               delete _notes[i]; // Delete it
               _notes[i] = 0;
               _pitches[i] = 0; // Don't recreate it!
               _volumes[i] = 0; // Don't recreate it!
            }
            if (samplesRead > maxSamples)
               maxSamples = samplesRead;
         }
      }
      _samplesRemaining -= numSamples;
      return maxSamples;
   };
   void SamplingRate(long rate) {
      AudioAbstract::SamplingRate(rate); 
      _instr->SamplingRate(rate);
   };
};

int main(int argc, char **argv) {
   bool saveOutput = false;
   AbstractInstrument *instr = 0;
   argv++;  argc--; // Skip program name

   // Process command-line options
   while ((argc>0) && (**argv == '-')) {
      char *p=*argv;
      switch(p[1]) {
      case '0': // Select instrument 0
         instr = new SineWaveInstrument;
         break;
      case '1': // Select instrument 1
         instr = new PluckedStringInstrument;
         break;
      case 'p': // Play output
         saveOutput = false;
         break;
      case 'a': // Record output in AU file
         saveOutput = true;
         break;
      }
      argv++; argc--;
   }

   if(instr == 0) {instr = new SineWaveInstrument; }
   PlayInstrument pi(instr);

   if (argc == 0) { // Default is a just major triad
      pi.AddNote(330.0F, 0.3F);
      pi.AddNote(440.0F, 0.3F);
      pi.AddNote(550.0F, 0.3F);
   } else { // Pick notes/volumes from command line
      while (argc>0) {
         float volume = 1.0/((argc+1)/2);
         if(argc>1) {
            volume = atof(argv[1]);
            argc--;
         }
         pi.AddNote(atof(argv[0]),volume);
         argc--;
         argv+= 2;
      }
   }

   AbstractPlayer *player;
   if (saveOutput) { // Play output into AU file
      ofstream out("playnote.au");
      player = new AuWrite(&pi, out);
      player->Play();
      delete player;
   } else { // Play output to speaker
      player = new Player(&pi);
      player->Play();
      delete player;
   }
   delete instr;
   return 0;
}   
