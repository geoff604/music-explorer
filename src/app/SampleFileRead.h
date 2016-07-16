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

// SampleFileRead.h: interface for the SampleFileRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEFILEREAD_H__2421D564_80CC_4184_A83B_6B7635D53EF1__INCLUDED_)
#define AFX_SAMPLEFILEREAD_H__2421D564_80CC_4184_A83B_6B7635D53EF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "audio.h"

#include <string>
using namespace std;

const int CURRENT_SAMPLE_FILE_VERSION = 0;

struct SampleFileHeader
{
	char magic_chars[5];
	int version;
	int channels;
	long sampling_rate;

	SampleFileHeader()
	{
		strcpy( magic_chars, "gpgl");
		version = CURRENT_SAMPLE_FILE_VERSION;
	}
};

class SampleFileRead  : public AudioAbstract
{
public:
	SampleFileRead(istream & s) ;
	virtual ~SampleFileRead();

	bool Open( string filename );

	bool Close();
	bool IsValid();

	unsigned long SizeInSamples();
	bool GoToSample( unsigned long sample_number );

   size_t GetSamples(AudioSample *buffer, size_t numSamples);
   size_t ReadBytes(AudioByte *buffer, size_t numBytes);

	void SetSampleRange( unsigned long samples_in_range );

protected:
   void MinMaxSamplingRate(long *min, long *max, long *preferred);
   void MinMaxChannels(int *min, int *max, int *preferred);

private:
	istream & _stream;
	SampleFileHeader _header;
	bool _file_valid;
	unsigned long _size_in_samples;
	long _samples_remaining;

	
};

bool CreateSampleFileFromWave( string wave_filename, string sample_filename );

#endif // !defined(AFX_SAMPLEFILEREAD_H__2421D564_80CC_4184_A83B_6B7635D53EF1__INCLUDED_)
