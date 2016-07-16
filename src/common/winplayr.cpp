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

#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include "aplayer.h"
#include "winplayr.h"
#include "soundexception.h"


WinPlayer::WinPlayer(AudioAbstract *a): AbstractPlayer(a) 
{

	_wave_device = NULL;	

	InitializeCriticalSection( &_queue_lock );

	_queueAvailableEvent = CreateEvent( 
        NULL,         // no security attributes
        FALSE,         // auto-reset event
        TRUE,         // initial state is signaled
        NULL			 // object name
        ); 

	_queueItemsEvent = CreateEvent( 
        NULL,         // no security attributes
        FALSE,         // auto-reset event
        TRUE,         // initial state is signaled
        NULL			 // object name
        ); 
	
	_buffAvailableEvent = CreateEvent( 
        NULL,         // no security attributes
        FALSE,         // auto-reset event
        TRUE,         // initial state is signaled
        NULL			 // object name
        ); 

	_wantToQuitEvent = CreateEvent( 
        NULL,         // no security attributes
        FALSE,         // auto-reset event
        FALSE,         // initial state is not signaled
        NULL			 // object name
        ); 

	_quitEvent = CreateEvent(
		  NULL,         // no security attributes
        TRUE,         // manual-reset event
        TRUE,         // initial state is signaled
        NULL			 // object name
        ); 
};

WinPlayer::~WinPlayer()
{
	if( _wave_device )
	{
		//todo: close the wave device here
		delete _wave_device;
	}

	DeleteCriticalSection( &_queue_lock );
}

WinPlayer::QUIT_MODE WinPlayer::GetStatus()
{
	return _quit_mode;
}


void WinPlayer::BackwardsStop()
{

	AudioAbstract::BackwardsStop();

	SetEvent( _wantToQuitEvent );
	_quit_mode = QUIT_IMMEDIATE;
	::SetEvent( _quitEvent );
	
}
	
void WinPlayer::BackwardsStart()
{
	AudioAbstract::BackwardsStart();


}


void WinPlayer::Play(void) 
{

	Start();   
		
	SelectDevice();

	ClearQueue();
	
	::ResetEvent( _quitEvent );
	::ResetEvent( _queueItemsEvent );
	_quit_mode = RUN;

	CWinThread* worker_thread = AfxBeginThread(WorkerThreadFunc, (LPVOID)this,
			THREAD_PRIORITY_NORMAL,
			0, CREATE_SUSPENDED, NULL);

    worker_thread->m_bAutoDelete = FALSE;	
    worker_thread->ResumeThread();		

	HANDLE dummyEvent = CreateEvent(
		  NULL,         // no security attributes
        TRUE,         // manual-reset event
        TRUE,         // initial state is signaled
        NULL			 // object name
        ); 

	HANDLE events[2];
	events[0] = _wantToQuitEvent;
	events[1] = dummyEvent;

   while(! IsEndOfSource() && _quit_mode == RUN) 
	{

		DWORD result = WaitForMultipleObjects( 2, events, FALSE, INFINITE );
		int test = WAIT_OBJECT_0;
		bool want_to_quit = (result - WAIT_OBJECT_0) == 0;

		if( want_to_quit )
		{
			// signal the quit
			_quit_mode = QUIT_IMMEDIATE;
			::SetEvent( _quitEvent );
		}
      else
		{
			FillQueue(); // Top off the queue
			::SetEvent( _queueItemsEvent );
			
			Sleep(50 /* ms */); // Loop about 20 times a second
		}
   }

	::WaitForSingleObject( *worker_thread, INFINITE );
	delete worker_thread;

	_wave_device->Close( WAVE_FORM_FLOW_OUT );
	delete _wave_device;
	_wave_device = NULL;

	ResetEvent( _wantToQuitEvent );

}

UINT WorkerThreadFunc( LPVOID pParam )
{
	WinPlayer* player = (WinPlayer*)pParam;

	bool started = false;
	bool done = false;

	while( (player->GetStatus() == WinPlayer::RUN) && !done)
	{
		HANDLE handles[2];		
		handles[0] = player->_quitEvent;

		TRACE0( "\nWorkerThreadGotInLoop");
		
		if( !(player->GetStatus() == WinPlayer::QUIT_IMMEDIATE)
			&& player->_wave_device->WaveOutFull()
			)
		{
			TRACE0( "\nWorkerThreadWaitingOnWaveOutFull");
			handles[1] = player->_buffAvailableEvent;		
			::WaitForMultipleObjects( 2, handles, false, INFINITE );
		}


		if( !(player->GetStatus() == WinPlayer::QUIT_IMMEDIATE)
			&& ! player->ItemsInQueue() )
		{
			TRACE0( "\nWorkerThreadWaitingOnQueueItemsEvent");
			handles[1] = player->_queueItemsEvent;
			::WaitForMultipleObjects( 2, handles, false, INFINITE );
		}

		if( player->IsEndOfSource() && ! player->ItemsInQueue() )
		{
			done = true;
			TRACE0( "\nSet done=true");
			continue;
		}

		while( (!(player->GetStatus() == WinPlayer::QUIT_IMMEDIATE)) &&
					player->ItemsInQueue() && 
			    (! player->_wave_device->WaveOutFull() )
			)
		{

			TRACE0( "\nFeedingDataToPlayer");
			PWAVEFRAME frame;

			player->_wave_device->Write( &frame ); // allocate the frame

			unsigned long samples_space_avail = MAX_WAVE_BUFFER_LEN / (player->_sampleWidth / 8);

			unsigned long samplesRead = 0;
			
			if( player->_sampleWidth == 16) 
			{			
				samplesRead = player->FromQueue(
                              reinterpret_cast<Sample16 *>(frame->waveHdr.lpData),
                              samples_space_avail,
										done
										);
				
			}
			else // _sampleWidth == 8
			{
				samplesRead = player->FromQueue(
                              reinterpret_cast<Sample8 *>(frame->waveHdr.lpData),
                              samples_space_avail,
										done
										);
			}
				
			if( samplesRead == 0 )
			{
				TRACE0( "\nSamplesRead=0");
				// Write some zeros to keep this block in Windows' queue
				memset(frame->waveHdr.lpData,0,MAX_WAVE_BUFFER_LEN);
				frame->waveHdr.dwBufferLength = 256;				
			}
			else // some samples were read
			{
				
				frame->waveHdr.dwBufferLength = samplesRead * (player->_sampleWidth / 8);
				TRACE2( "\nSamplesRead=%d,BufferLength=%d", samplesRead, frame->waveHdr.dwBufferLength );
			}

			player->_wave_device->Write( frame );

			if( ! started )
			{
				player->_wave_device->Start( WAVE_FORM_FLOW_OUT );
				started = true;
			}
	
		}
   }
   
	TRACE0( "\nWaiting for player to finish playing" );
	while( !(player->GetStatus() == WinPlayer::QUIT_IMMEDIATE) 
		     && player->_wave_device->WaveOutStillPlaying() )
	{
		Sleep( 50 );
	}

	TRACE0( "\nClosing player");

	// stop the player, and clear the player's buffer.

	if( player->_wave_device )
	{
		player->_wave_device->ClearWaveOutBuffer();
		
		UINT state = player->_wave_device->GetState( WAVE_FORM_FLOW_OUT );

		if( state == WAVE_STATE_PAUSE || state == WAVE_STATE_RESET)// another thread is operating
		{
			Sleep( 50 ); // hopefully the other thread will finish
		}

		player->_wave_device->Reset(WAVE_FORM_FLOW_OUT);
	
	}

	return 0;
}

// These are the primary formats supported by Windows
static struct {
   DWORD format; // Constant
   UINT rate;    // break down for this constant
   UINT channels;
   UINT width;
} winFormats[] = {
   {WAVE_FORMAT_1S16, 11025, 2, 16},
   {WAVE_FORMAT_1S08, 11025, 2, 8},
   {WAVE_FORMAT_1M16, 11025, 1, 16},
   {WAVE_FORMAT_1M08, 11025, 1, 8},
   {WAVE_FORMAT_2S16, 22050, 2, 16},
   {WAVE_FORMAT_2S08, 22050, 2, 8},
   {WAVE_FORMAT_2M16, 22050, 1, 16},
   {WAVE_FORMAT_2M08, 22050, 1, 8},
   {WAVE_FORMAT_4S16, 44100, 2, 16},
   {WAVE_FORMAT_4S08, 44100, 2, 8},
   {WAVE_FORMAT_4M16, 44100, 1, 16},
   {WAVE_FORMAT_4M08, 44100, 1, 8},
   {0,0,0,0}
};

//
// Negotiate the sound format and open a suitable output device
// 
int WinPlayer::SelectDevice(void) 
// Precondition: wave device is not constructed
// Postcondition: wave device is constructed and open
{
   // Get everyone else's idea of format
   int channelsMin = 1, channelsMax = 2, channelsPreferred = 0;
   long rateMin = 8000, rateMax = 44100, ratePreferred = 22050;

   MinMaxChannels(&channelsMin,&channelsMax,&channelsPreferred);
   if (channelsMin > channelsMax) {
      throw new CSoundException( CSoundException::causeNegotiateChannels);
   }

   MinMaxSamplingRate(&rateMin,&rateMax,&ratePreferred);
   if (rateMin > rateMax) {
      throw new CSoundException( CSoundException::causeNegotiateSampling);
   }

   // First, try for an exact match
   static const int NO_MATCH=100000;
   UINT matchingDevice = NO_MATCH;
   WAVEFORMATEX waveFormat;
   waveFormat.wFormatTag = WAVE_FORMAT_PCM;
   waveFormat.nChannels = channelsPreferred;
   waveFormat.nSamplesPerSec = ratePreferred;
   waveFormat.wBitsPerSample = 8 * sizeof(Sample16);
   waveFormat.nBlockAlign = waveFormat.nChannels
                  * waveFormat.wBitsPerSample / 8;
   waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign
                  * waveFormat.nSamplesPerSec;
   waveFormat.cbSize = 0;
   MMRESULT err = waveOutOpen(0,WAVE_MAPPER,&waveFormat,
                              0,0,WAVE_FORMAT_QUERY);

   if (err == 0) {
      matchingDevice = WAVE_MAPPER;
      channelsMax = channelsMin = channelsPreferred;
      rateMax = rateMin = ratePreferred;
      _sampleWidth = 16;
   } else {
      TRACE0( "WinPlay: Custom format failed, trying standard formats." );
   }

   // Get count of available devices
   UINT numDevs = waveOutGetNumDevs();
   if (numDevs == 0) {
      throw new CSoundException( CSoundException::causeNoAudioOutputDevice);
   }

   // Check each available device
   for (UINT i=0; (i<numDevs) && (matchingDevice == NO_MATCH); i++) {
      // What formats does this device support?
      WAVEOUTCAPS waveOutCaps;
      MMRESULT err =
                waveOutGetDevCaps(i,&waveOutCaps,sizeof(waveOutCaps));
      if (err != MMSYSERR_NOERROR) {
         TRACE1( "Couldn't get capabilities of device %d", i );
         continue;
      }
      // Check each standard format
      for(UINT j=0; winFormats[j].format != 0; j++) {
         if ((winFormats[j].format & waveOutCaps.dwFormats) // supported?
            &&(rateMin <= winFormats[j].rate) // Rate ok?
            &&(rateMax >= winFormats[j].rate)
            &&(channelsMin <= winFormats[j].channels) // channels ok?
            &&(channelsMax >= winFormats[j].channels)) {

            // Set up my parameters
            matchingDevice = i;
            rateMin = rateMax = ratePreferred = winFormats[j].rate;
            channelsPreferred = winFormats[j].channels;
            channelsMin = channelsMax = channelsPreferred;
            _sampleWidth = winFormats[j].width;

            // Set up WAVEFORMATEX structure accordingly
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = winFormats[j].channels;
            waveFormat.nSamplesPerSec = winFormats[j].rate;
            waveFormat.wBitsPerSample = winFormats[j].width;
            waveFormat.nBlockAlign = waveFormat.wBitsPerSample / 8
                                 * waveFormat.nChannels;
            waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign
                                  * waveFormat.nSamplesPerSec;
            waveFormat.cbSize = 0;
         }
      }
   }

   if (matchingDevice == NO_MATCH) {
      TRACE0( "Can't handle this sound format." );
      TRACE2( "Rate: %d-%d", rateMin, rateMax );
      TRACE2( "Channels: %d-%d", channelsMin, channelsMax );
      return 1;
   }

   // If we found a match, set everything
   SetChannelsRecursive(channelsPreferred);
   SetSamplingRateRecursive(ratePreferred);

   // Open the matching device
	_wave_device = new CWaveForm( WAVE_FORM_FLOW_OUT, 0, matchingDevice, NULL, &waveFormat );
	bool open_success = _wave_device->Open( WAVE_FORM_FLOW_OUT );

	_wave_device->Advise( (unsigned long)_buffAvailableEvent, 0, 0, CALLBACK_EVENT );

   if (! open_success ) {
	  throw new CSoundException( CSoundException::causeOpenWAVOutputDevice);
   }

	InitializeQueue(128*1024L); // Allocate 128k queue

   return 0;
}
