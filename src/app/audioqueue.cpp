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

#include "audioqueue.h"
#include "assert.h"

AudioQueue::AudioQueue( unsigned long queueSize )
{
	// create the queue boundaries in memory
   _queue = new AudioSample[queueSize];
   _queueSize = queueSize;

	// start off with an empty queue
	_first = _last = 0;

	InitializeCriticalSection( &_queue_lock );
}

AudioQueue::~AudioQueue()
{
	delete [] _queue;

	DeleteCriticalSection(&_queue_lock);
}

void AudioQueue::Clear()
{
	EnterCriticalSection( &_queue_lock );
	
	_first = _last = 0;

	LeaveCriticalSection( &_queue_lock );

}

bool AudioQueue::IsEmpty()
{
	bool empty = false;

	EnterCriticalSection( &_queue_lock );
	if( _first == _last )
	{
		empty = true;
	}
	LeaveCriticalSection( &_queue_lock );

	return empty;

}

unsigned long AudioQueue::GetQueueSpace()
{
	unsigned long space = 0;

	EnterCriticalSection( &_queue_lock );
	
	if( _first < _last )
	{
		space = (_queueSize - 1) - _last;
		space += _first;
	}
	else if( _first > _last )
	{
		space = (_first - _last) - 1;
	}
	else //if _first == _last
	{
		space = _queueSize - 1;
	}

	LeaveCriticalSection( &_queue_lock );

	return space;
}

unsigned long AudioQueue::GetQueueCount()
// returns the number of samples currently queued
{
	unsigned long count = 0;

	EnterCriticalSection( &_queue_lock );
	
	if( _first < _last )
	{
		count = _last - _first;
	}
	else if( _first > _last )
	{
		count = _queueSize - _first;
		count += _last;
	}
	// else if _first == _last then count = 0

	LeaveCriticalSection( &_queue_lock );

	return count;
}

void AudioQueue::WriteToQueue( AudioSample* data, unsigned long samples_to_queue )
// writes the number of samples specified by samples_to_queue
// to the queue, reading from the memory pointed to by data
{
	assert( samples_to_queue <= GetQueueSpace() );

	int samples_waiting = samples_to_queue;

	EnterCriticalSection( &_queue_lock );

	if( _last >= _first )
	{
		int samples_to_copy;

		// write from last to end

		if( _queueSize - _last <= samples_waiting ) 
		{	
			samples_to_copy = _queueSize - _last;
			samples_waiting -= samples_to_copy;

		}
		else
		{
			samples_to_copy = samples_waiting;
			samples_waiting = 0;
		}
		memcpy( _queue + _last, data, sizeof(AudioSample)*( samples_to_copy ) );
		
		if( samples_to_copy + _last == _queueSize )
		{
			// wrap around
			_last = 0;
		}
		else
		{
			_last = samples_to_copy + _last;
		}		
	}

	// if there is room between last and first
	if( _last < _first )
	{
		if( samples_waiting > 0 )
		{
			int samples_already_copied = samples_to_queue - samples_waiting;

			memcpy( _queue + _last, data + samples_already_copied, 
				sizeof(AudioSample)*( samples_waiting ) );

			_last += samples_waiting;
		}
	}

	LeaveCriticalSection( &_queue_lock );

}

unsigned long AudioQueue::ReadFromQueue(Sample16 *pDest, unsigned long destSize) 
{
   unsigned long destRemaining = destSize;

	EnterCriticalSection( &_queue_lock );

   if (_last < _first) 
	{
      unsigned long copySize = _queueSize - _first; // Number samples avail
      if (copySize > destRemaining)
         copySize = destRemaining;
      DataFromQueue(pDest,copySize);
      destRemaining -= copySize;
      pDest += copySize;
   }

   if ((destRemaining > 0) && (_last > _first)) {
      unsigned long copySize = _last - _first;
      if (copySize > destRemaining)
         copySize = destRemaining;
      DataFromQueue(pDest, copySize);
      destRemaining -= copySize;
      pDest += copySize;
   }

	LeaveCriticalSection( &_queue_lock );

   return (destSize - destRemaining);
};

unsigned long AudioQueue::ReadFromQueue(Sample8 *pDest, unsigned long destSize) 
{
   unsigned long destRemaining = destSize;

	EnterCriticalSection( &_queue_lock );

   if (_last < _first) 
	{
      unsigned long copySize = _queueSize - _first; // Number samples avail
      if (copySize > destRemaining)
         copySize = destRemaining;
      DataFromQueue(pDest,copySize);
      destRemaining -= copySize;
      pDest += copySize;
   }

   if ((destRemaining > 0) && (_last > _first)) {
      unsigned long copySize = _last - _first;
      if (copySize > destRemaining)
         copySize = destRemaining;
      DataFromQueue(pDest, copySize);
      destRemaining -= copySize;
      pDest += copySize;
   }

	LeaveCriticalSection( &_queue_lock );

   return (destSize - destRemaining);
};


void AudioQueue::DataFromQueue(Sample16 *pDest, unsigned long copySize) 
{
   AudioSample* newQueueFirst = _queue + _first;

   for(unsigned long i=0;i<copySize;i++)
      *pDest++ = *newQueueFirst++ 
         >> ((sizeof(*newQueueFirst) - sizeof(*pDest)) * 8 );
   
	assert( newQueueFirst <= (_queue + _queueSize));

	if (newQueueFirst == (_queue + _queueSize))
      _first = 0; 
	else
		_first += copySize;
}


void AudioQueue::DataFromQueue(Sample8 *pDest, unsigned long copySize) 
{
   AudioSample* newQueueFirst = _queue + _first;

   for(unsigned long i=0;i<copySize;i++)
      *pDest++ = *newQueueFirst++ 
         >> ((sizeof(*newQueueFirst) - sizeof(*pDest)) * 8 );
   
	assert( newQueueFirst <= (_queue + _queueSize));

	if (newQueueFirst == (_queue + _queueSize))
      _first = 0; 
	else
		_first += copySize;
}