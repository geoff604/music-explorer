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
/* Player class for Network Audio System */
#ifndef NASPLAYER_H_INCLUDED
#define NASPLAYER_H_INCLUDED
#include "audio.h"
#include "aplayer.h"
extern "C" { /* NAS definitions */
#include <audio/audiolib.h>
}

class NasPlayer : public AbstractPlayer {
private:
   bool _finished; // true -> all data played
   
private:
   AuServer *_server;
   AuFlowID _flow;
private:
   void OpenServer(); // Connect to server
   friend AuBool NasEventHandler(AuServer *server,
           AuEvent *event,  AuEventHandlerRec *eventData);
   AuBool Notify(AuElementNotifyEvent *notifyEvent);
   void SendData(AuUint32 numBytes);
private:
#define nasBufferSize 100000
   Sample16 _buffer[nasBufferSize];
public:
   NasPlayer(AudioAbstract *a):AbstractPlayer(a) {
      _finished = false;
   } ;
   ~NasPlayer() {};
   void Play();  // Actually play the sound source
};
#endif
