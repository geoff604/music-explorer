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
#include <Resources.h>
#include <Sound.h>
#include <Memory.h>
#include <Menus.h>
#include <istream>
#include <fstream>

#include "audio.h"
#include "aplayer.h"
#include "macplayr.h"

pascal void NextBlockCallback(SndChannelPtr scp,
         SndDoubleBufferPtr doubleBuffer)
{
   MacPlayer *me 
      = reinterpret_cast<MacPlayer *>(doubleBuffer->dbUserInfo[0]);
   me->NextBlock(scp,doubleBuffer);
}

void MacPlayer::NextBlock(SndChannelPtr, SndDoubleBufferPtr doubleBuffer) {
   AudioSample *pDest = 
      reinterpret_cast<Sample16 *>(doubleBuffer->dbSoundData);
   long copied = FromQueue(pDest,doubleBufferSize);

   doubleBuffer->dbNumFrames = copied/Channels();
   doubleBuffer->dbFlags = (doubleBuffer->dbFlags) | dbBufferReady;
   if (_endOfQueue)
      doubleBuffer->dbFlags = (doubleBuffer->dbFlags) | dbLastBuffer;
};
void MacPlayer::Play(void (*serviceFunc)(void)) {
   SCStatus          Stats;
   SndChannelPtr        chan;

   chan = nil;
   OSErr err = SndNewChannel (&chan, sampledSynth, 0, nil);
   if (err != noErr)
      Debugger();

   SndDoubleBufferHeader   doubleHeader;
   doubleHeader.dbhNumChannels = Channels();
   doubleHeader.dbhSampleSize = 16; // Bits per sample
   doubleHeader.dbhCompressionID = 0; // Sound is not compressed
   doubleHeader.dbhPacketSize = 0; // Not used, since no compression
   doubleHeader.dbhSampleRate = 
      static_cast<Fixed>(SamplingRate()) << 16;

   /* create a UPP for the SndDoubleBackProc */
   doubleHeader.dbhDoubleBack = NewSndDoubleBackProc(NextBlockCallback);

   // Initialize queue _after_ above negotiations
   InitializeQueue(128 * 1024L); // Allocate 128k queue

   for (int i = 0; i <= 1; ++i) {
      SndDoubleBufferPtr doubleBuffer = 
         reinterpret_cast<SndDoubleBufferPtr>
         (NewPtrClear(sizeof(SndDoubleBuffer) + doubleBufferSize*2));
      doubleHeader.dbhBufferPtr [i] = doubleBuffer;
   
      if ((doubleBuffer == nil) || (MemError() != 0))
         Debugger();
      
      doubleBuffer->dbNumFrames = 0;
      doubleBuffer->dbFlags = 0;
      doubleBuffer->dbUserInfo [0] = static_cast<long>(this);
      NextBlockCallback(chan, doubleBuffer); // initialize the buffers
   }
   
   err = SndPlayDoubleBuffer (chan, &doubleHeader);
   if (err != noErr)
      Debugger();
   
   do {
      FillQueue(); // Keep queue full
      if (serviceFunc)
         serviceFunc(); // Call the service function
      err = SndChannelStatus (chan, sizeof (Stats), &Stats);
   } while (Stats.scChannelBusy);
      
   DisposePtr ((Ptr) doubleHeader.dbhBufferPtr[0]);
   DisposePtr ((Ptr) doubleHeader.dbhBufferPtr[1]);

   err = SndDisposeChannel (chan,0);
   if (err != noErr)
      Debugger();
}
