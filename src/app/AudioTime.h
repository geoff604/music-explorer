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

// AudioTime.h: interface for the CAudioTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOTIME_H__3F7E8E67_9F1F_4F6D_916E_C9598395F862__INCLUDED_)
#define AFX_AUDIOTIME_H__3F7E8E67_9F1F_4F6D_916E_C9598395F862__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>

using namespace std;

struct SMPTTime
{
	unsigned int hours;
	unsigned int minutes;
	unsigned int seconds;
	unsigned int frames;

	SMPTTime();
};

class CAudioTime : public SMPTTime
{
public:
	CAudioTime();
	CAudioTime(unsigned int new_hours, unsigned int new_minutes,
		unsigned int new_seconds, unsigned int new_frames/*);*/, unsigned int new_subframes);
	virtual ~CAudioTime();

	unsigned long CalculateStartingSample( int sampling_rate, int channels );
	// given the current time, calculates how many samples
	// would have to be read from a wave file to reach the current
	// time.

	unsigned long CalculateSamplesInRange( CAudioTime& end_of_range, int sampling_rate,
												int channels );
	// in the time range between the current object and the end_of_range,
	// calculates how many samples are in that range

	void SetFromSampleCount( unsigned long sample_count, int sampling_rate,
									 int channels );
	// sets the time based on a count of samples, given a sampling rate
	// and the number of channels.

	void SetFromSubframes( unsigned long num_subframes );

	long int GetAllInSubframes();

	CAudioTime operator-( CAudioTime& time );

	CAudioTime operator+( CAudioTime& time );

	bool operator>=(CAudioTime &time);

	bool operator<(CAudioTime &time);
	
	bool operator<=(CAudioTime &time);

	bool operator==(CAudioTime &time);

	friend ostream & operator<<( ostream & os, const CAudioTime &time);

	unsigned int subframes;

private:	
	static const unsigned int subframesPerFrame; //number of subframes in a frame

};

ostream & operator<<( ostream & os, const CAudioTime &time);

class CAudioTimer
{
public:
	CAudioTimer();
	virtual ~CAudioTimer();

	void Start();
	void Stop();
	void Reset();

	CAudioTime GetTime();
	void SetTime( CAudioTime& time );

};

#endif // !defined(AFX_AUDIOTIME_H__3F7E8E67_9F1F_4F6D_916E_C9598395F862__INCLUDED_)
