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

// AudioTime.cpp: implementation of the CAudioTime class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "AudioTime.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SMPTTime::SMPTTime()
{
	frames = 0;
	hours = 0;
	minutes = 0;
	seconds = 0;
}

const unsigned int CAudioTime::subframesPerFrame = 4;

//=============================================================================
CAudioTime::CAudioTime()
{
	subframes = 0;
}

//=============================================================================
CAudioTime::CAudioTime(unsigned int new_hours, unsigned int new_minutes, unsigned int new_seconds, unsigned int new_frames, unsigned int new_subframes)
{
	hours = new_hours;
	minutes = new_minutes;
	seconds =  new_seconds;
	frames = new_frames;
	subframes = new_subframes;
}

//=============================================================================
CAudioTime::~CAudioTime()
{

}

//=============================================================================
unsigned long CAudioTime::CalculateStartingSample( int sampling_rate, int channels )
// given the current time, calculates how many samples
// would have to be read from a wave file to reach the current
// time.
{
	double samples_per_subframe = sampling_rate / (30.0 * subframesPerFrame);

	long num_subframes = this->GetAllInSubframes();

	return (unsigned long)(channels*num_subframes*samples_per_subframe);

}

//=============================================================================
unsigned long CAudioTime::CalculateSamplesInRange( CAudioTime& end_of_range, int sampling_rate,
											int channels )
// in the time range between the current object and the end_of_range,
// calculates how many samples are in that range
{
	unsigned long int start_this = CalculateStartingSample( sampling_rate, channels );
	unsigned long int start_other = end_of_range.CalculateStartingSample( sampling_rate, channels );

	if( start_this > start_other )
		return start_this - start_other;
	
	return start_other - start_this;

}

//=============================================================================
void CAudioTime::SetFromSampleCount( unsigned long sample_count, int sampling_rate,
									 int channels )
{
	unsigned long subframes = (sample_count / channels) / (sampling_rate / (30.0 * subframesPerFrame));

	SetFromSubframes( subframes );

}

//=============================================================================
void CAudioTime::SetFromSubframes( unsigned long num_subframes )
{
	//convert back into SMPTTime
	hours = num_subframes/(108000 * subframesPerFrame);

	int leftoverMinutes = num_subframes - (hours * 108000 * subframesPerFrame);
	minutes = leftoverMinutes / (1800 * subframesPerFrame);

	int leftoverSeconds = leftoverMinutes - (minutes * 1800 * subframesPerFrame);
	seconds =  leftoverSeconds / (30 * subframesPerFrame);

	int leftoverFrames = leftoverSeconds - (seconds * 30 * subframesPerFrame);
	frames = leftoverFrames / (subframesPerFrame);

	subframes = leftoverFrames - (frames * subframesPerFrame);
}

//=============================================================================
long int CAudioTime::GetAllInSubframes()
{
	return (hours * 108000 + minutes * 1800 +
					 seconds * 30 + frames)* subframesPerFrame + subframes;
}

//=============================================================================
CAudioTime CAudioTime::operator-( CAudioTime& time )
{
	CAudioTime diffTime;

	//convert left and right times into number of subframes
	long leftTime = GetAllInSubframes();

	long rightTime = time.GetAllInSubframes();

	if (leftTime > rightTime)
	//if leftTime > rightTime, difference is positive
	//convert difference in frames to SMPTTime
	{
		long diff;

		diff = leftTime - rightTime;

		diffTime.SetFromSubframes(diff);
	}
	else
	//otherwise, difference is negative, so set result to 0
	{
		diffTime.frames = 0;
		diffTime.hours = 0;
		diffTime.minutes = 0;
		diffTime.frames = 0;
		//diffTime.subframes = 0;
	}

	return diffTime;
}

//=============================================================================
CAudioTime CAudioTime::operator+( CAudioTime& time )
{
	CAudioTime sumTime;

	//convert left and right times into number of subframes
	unsigned long leftTime = GetAllInSubframes();

	unsigned long rightTime = time.GetAllInSubframes();

	unsigned long sum;

	sum = leftTime + rightTime;

	sumTime.SetFromSubframes( sum );

	return sumTime;
}

//=============================================================================
bool CAudioTime::operator>=(CAudioTime &time)
{
	if (this->hours > time.hours)
	{
		return true;
	}
	else if (this->hours == time.hours)
	{
		if (this->minutes > time.minutes)
		{
			return true;
		}
		else if (this->minutes == time.minutes)
		{
			if (this->seconds > time.seconds)
			{
				return true;
			}
			else if (this->seconds == time.seconds)
			{
				if (this->frames >/*=*/ time.frames)
				{
					return true;
				}
				else if (this->frames == time.frames)
				{
					if (this->subframes >= time.subframes)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//=============================================================================
bool CAudioTime::operator<(CAudioTime &time)
{
	if (this->hours < time.hours)
	{
		return true;
	}
	else if (this->hours == time.hours)
	{
		if (this->minutes < time.minutes)
		{
			return true;
		}
		else if (this->minutes == time.minutes)
		{
			if (this->seconds < time.seconds)
			{
				return true;
			}
			else if (this->seconds == time.seconds)
			{
				if (this->frames < time.frames)
				{
					return true;
				}
				else if (this->frames == time.frames)
				{
					if (this->subframes < time.subframes)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//=============================================================================
bool CAudioTime::operator<=(CAudioTime &time)
{
	if (this->hours < time.hours)
	{
		return true;
	}
	else if (this->hours == time.hours)
	{
		if (this->minutes < time.minutes)
		{
			return true;
		}
		else if (this->minutes == time.minutes)
		{
			if (this->seconds < time.seconds)
			{
				return true;
			}
			else if (this->seconds == time.seconds)
			{
				if (this->frames < time.frames)
				{
					return true;
				}
				else if (this->frames == time.frames)
				{
					if (this->subframes <= time.subframes)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

//=============================================================================
bool CAudioTime::operator==(CAudioTime &time)
{
	if (this->hours == time.hours && this->minutes == time.minutes &&
		this->seconds == time.seconds && this->frames == time.frames/*)*/ &&
		this->subframes == time.subframes)
		return true;

	return false;
}

//=============================================================================
ostream & operator<<( ostream & os, const CAudioTime &time)
{
	if (time.hours < 10)
	{
		os << "0";
	}

	os << time.hours << ":";

	if (time.minutes < 10)
	{
		os << "0";
	}

	os << time.minutes << ":";

	if (time.seconds < 10)
	{
		os << "0";
	}

	os << time.seconds << ":";

	if (time.frames < 10)
	{
		os << "0";
	}

	os << time.frames;

	return os;
}

///////////////////////////////////////////////////////////////////////////////
CAudioTimer::CAudioTimer()
{
}

CAudioTimer::~CAudioTimer()
{

}

void CAudioTimer::Start()
{

}

void CAudioTimer::Stop()
{

}

void CAudioTimer::Reset()
{

}

CAudioTime CAudioTimer::GetTime()
{
	return CAudioTime();
}

void CAudioTimer::SetTime( CAudioTime& time )
{

}