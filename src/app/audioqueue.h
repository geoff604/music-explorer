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

// Programmed by Geoff Peters, 3-23-03

#ifndef AUDIOQUEUE_H_INCLUDED
#define AUDIOQUEUE_H_INCLUDED

#include "audiosample.h"
#include "windows.h"

typedef short Sample16;
typedef signed char Sample8;	

class AudioQueue 
{
public:
   
	AudioQueue( unsigned long queueSize );
   ~AudioQueue();   

	bool IsEmpty();
	// returns true if the queue has been emptied
	
	unsigned long GetQueueSpace();
	// returns the amount of samples
	// that could be added to the queue at 
	// the current moment.

	unsigned long GetQueueCount();
	// returns the number of samples currently queued

	void WriteToQueue( AudioSample* data, unsigned long samples_to_queue );	
	// writes the number of samples specified by samples_to_queue
	// to the queue, reading from the memory pointed to by data
	
	unsigned long ReadFromQueue(Sample16 *pDest, unsigned long destSize);
	unsigned long ReadFromQueue(Sample8 *pDest, unsigned long destSize);
	// returns the number of samples actually read from the queue

	void Clear();
	// clears the queue of all data


private:   
	CRITICAL_SECTION _queue_lock;	
	
	AudioSample* _queue;
	
	unsigned long _queueSize;
	unsigned long _first;
	unsigned long _last;

	// empty if first=last
	// can never be full. max items is _queueSize - 1
	// i.e. when _last + 1 == _first

	void DataFromQueue(Sample16 *pDest, unsigned long copySize);
	void DataFromQueue(Sample8 *pDest, unsigned long copySize);

};

#endif
