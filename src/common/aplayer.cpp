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
#include "aplayer.h"

AbstractPlayer::AbstractPlayer(AudioAbstract *a) : AudioAbstract(a) {
   _endOfSource = false;
   _endOfQueue = false;
   _queue = NULL;

	InitializeCriticalSection( &_player_lock );
}

AbstractPlayer::~AbstractPlayer(void) {
   if (_queue)      
      delete _queue;

	DeleteCriticalSection( &_player_lock );
}

void AbstractPlayer::InitializeQueue(unsigned long queueSize) {
   if( ! _queue )
		_queue = new AudioQueue(queueSize);
}

bool AbstractPlayer::IsEndOfSource()
{
	bool is_end = false;
	if( _queue )
	{
		EnterCriticalSection( &_player_lock );
		if( _endOfSource )
			is_end = true;

		LeaveCriticalSection( &_player_lock );	
	}
	return is_end;
}

bool AbstractPlayer::IsEndOfQueue()
{
	bool is_end = false;
	if( _queue )
	{
		EnterCriticalSection( &_player_lock );
		if( _endOfQueue )
			is_end = true;

		LeaveCriticalSection( &_player_lock );	
	}
	return is_end;
}

bool AbstractPlayer::ItemsInQueue()
{
	bool result = false;
	EnterCriticalSection( &_player_lock );
	if( _queue )
	{
		if( _queue->GetQueueCount() > 0 )
			result = true;
	}
	LeaveCriticalSection( &_player_lock );	
	
	return result;
}


void AbstractPlayer::ClearQueue() {
   if( _queue )
	{
		EnterCriticalSection( &_player_lock );
		_queue->Clear();
		_endOfSource = false;
		_endOfQueue = false;

		LeaveCriticalSection( &_player_lock );
	}
}

void AbstractPlayer::FillQueue() {
   if( _queue )
	{
		EnterCriticalSection( &_player_lock );
		
		unsigned long space = _queue->GetQueueSpace();

		AudioSample* buff = new AudioSample[ space ];
		
		// Make sure request is a multiple of channels
		space -= space % Channels();

		unsigned long samplesRead = Previous()->GetSamples( buff,space );

		_queue->WriteToQueue( buff, samplesRead );

		TRACE1( "\nWrote %d to queue", samplesRead );

		if (samplesRead < space)
			_endOfSource = true;

		LeaveCriticalSection( &_player_lock );

		delete [] buff;
	}
}

unsigned long AbstractPlayer::FromQueue(Sample16 *pDest, unsigned long destSize, bool& end_of_queue) 
{	
	unsigned long data_to_read = 0;
	if( _queue )
	{
		EnterCriticalSection( &_player_lock );

		unsigned long data_in_queue = _queue->GetQueueCount();

		data_to_read = data_in_queue;
		if( data_to_read > destSize )
			data_to_read = destSize;

		_queue->ReadFromQueue( pDest, data_to_read );

		if ((data_to_read < destSize) && _endOfSource)
			_endOfQueue = true;		

		end_of_queue = _endOfQueue;
		LeaveCriticalSection( &_player_lock );
	}


   return (data_to_read);
}

unsigned long AbstractPlayer::FromQueue(Sample8 *pDest, unsigned long destSize, bool& end_of_queue) 
{	
	unsigned long data_to_read = 0;
	if( _queue )
	{
		EnterCriticalSection( &_player_lock );

		unsigned long data_in_queue = _queue->GetQueueCount();

		data_to_read = data_in_queue;
		if( data_to_read > destSize )
			data_to_read = destSize;

		_queue->ReadFromQueue( pDest, data_to_read );

		if ((data_to_read < destSize) && _endOfSource)
			_endOfQueue = true;		

		end_of_queue = _endOfQueue;
		LeaveCriticalSection( &_player_lock );
	}


   return (data_to_read);
}