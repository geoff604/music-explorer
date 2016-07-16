/*
Music Explorer
Copyright (c) 2003 Geoff Peters and Gabriel Lo

This file is part of Music Explorer.

Music Explorer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Music Explorer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Music Explorer (COPYING.TXT); if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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
/*
* Player class for Win32
*/
#ifndef WIN_PLAYER_H_INCLUDED
#define WIN_PLAYER_H_INCLUDED
#include "audio.h"
#include "aplayer.h"
#include <windows.h>
#include <mmsystem.h>

#include "waveform.h"

#define winBufferSize 10000  // Number of samples per buffer



class WinPlayer : public AbstractPlayer {
public:
   WinPlayer(AudioAbstract *a);
   ~WinPlayer();

   void Play();  // Actually play the sound source

	enum QUIT_MODE { RUN, QUIT_GRACEFUL, QUIT_IMMEDIATE };

	QUIT_MODE GetStatus();

protected:
	virtual void BackwardsStop();
	virtual void BackwardsStart();
private:	

	HANDLE _queueAvailableEvent;
	HANDLE _queueItemsEvent;
	HANDLE _buffAvailableEvent;
	HANDLE _wantToQuitEvent;
	HANDLE _quitEvent;

	CWaveForm* _wave_device;
	
	int   _sampleWidth; // width of data to output

   int SelectDevice(void); // Open a suitable device

	QUIT_MODE _quit_mode;

	friend UINT WorkerThreadFunc( LPVOID pParam );

	CRITICAL_SECTION _queue_lock;
};
#endif
