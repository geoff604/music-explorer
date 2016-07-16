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

// SampleFileRead.cpp: implementation of the SampleFileRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SampleFileRead.h"
#include "wav.h"

#include <assert.h>

#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


bool CreateSampleFileFromWave( string wave_filename, string sample_filename )
{
	ifstream file;

	file.open( wave_filename.c_str(), ios::in|ios::binary );
	if( file.is_open() )
	{
		WaveRead wav( file );
		size_t samples_read = 0;
		
		size_t samples_to_try = 44000; // get lots of samples at a time

		int channels = wav.Channels();

		SampleFileHeader header;
		header.channels = 1;
		header.sampling_rate = wav.SamplingRate();
		
		ofstream outfile;
		outfile.open( sample_filename.c_str(), ios::out|ios::binary );
		if( !outfile.is_open() )
		{
			::MessageBox
			(
				NULL,
				_T("Failed to open output file"),
				_T("Wave Open"),
				MB_OK | MB_ICONWARNING
			);
			return false;
		}
		
		// write the header
		outfile.write((char*)&header, sizeof(header) );

		bool keep_reading = true;

		AudioSample* samples = new AudioSample[ samples_to_try * channels ];
		AudioSample* samples_converted = new AudioSample[ samples_to_try ];

		while( keep_reading )
		{
			samples_read = wav.GetSamples( samples, samples_to_try * channels );
			
			if( samples_read < samples_to_try * channels )
				keep_reading = false;

			int samples_to_analyze =  samples_read / channels;

			for( int i = 0; i < samples_to_analyze; i++ )
			{
				// convert from stereo to mono
				double sum = 0;
				for( int chan = 0; chan < channels; chan++ )
					sum += samples[i * channels + chan];
				sum /= channels;

				samples_converted[i] = sum;
			}

			// write the converted samples
			outfile.write( (char*)samples_converted, 
				             sizeof( AudioSample ) * samples_to_analyze );
		}

		delete [] samples;
		delete [] samples_converted;
	}
	else
	{
		::MessageBox
			(
				NULL,
				_T("Failed to open input file"),
				_T("Wave Open"),
				MB_OK | MB_ICONWARNING
			);
		return false;
	}

	return true;
}	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SampleFileRead::SampleFileRead(istream & s): _stream(s) 
{
	// read the header

	_stream.read(reinterpret_cast<char *>(&_header),sizeof(_header));
	
	int count = _stream.gcount();

	if( sizeof( _header) == count
   		&& strcmp( _header.magic_chars, "gpgl" ) == 0 )
	{
		_file_valid = true;
	}
	else
	{
		_file_valid = false;
	}

	if( _file_valid )
	{
		// calculate size in samples
		_stream.seekg( 0, ios::end );
		unsigned long length = _stream.tellg();
		length -= sizeof(_header);
		length /= sizeof( AudioSample );

		_size_in_samples = length;
		_samples_remaining = -1;

	}
}

SampleFileRead::~SampleFileRead()
{

}

void SampleFileRead::SetSampleRange( unsigned long samples_in_range )
{
	_samples_remaining = samples_in_range;

}

bool SampleFileRead::IsValid()
{
	return _file_valid;
}

void SampleFileRead::MinMaxChannels(int *min, int *max, int *preferred) 
{  
   *min = *max = *preferred = 1;
}

void SampleFileRead::MinMaxSamplingRate(long *min, long *max, long *preferred) 
{
   assert( _file_valid );
	unsigned long samplingRate = _header.sampling_rate;
   *max = *min = *preferred = samplingRate;
}

unsigned long SampleFileRead::SizeInSamples()
{
	return _size_in_samples;
}

bool SampleFileRead::GoToSample( unsigned long sample_number )
{
	if( !_file_valid || sample_number > _size_in_samples )
		return false;

	_stream.clear();

	_stream.seekg( sizeof( _header ) + sample_number * sizeof( AudioSample ), ios::beg );


	return true;

}

size_t SampleFileRead::GetSamples(AudioSample *buffer, size_t numSamples)
{
	AudioByte *byteBuff = reinterpret_cast<AudioByte *>(buffer);
   size_t read = ReadBytes(byteBuff,numSamples * sizeof(AudioSample));
	size_t amount = read / sizeof(AudioSample);

	if( _samples_remaining != -1 )
	{
		_samples_remaining -= amount;

		if( _samples_remaining < 0 )
		{
			size_t reported_amount;
			size_t extra_samples;

			extra_samples = abs(_samples_remaining );

			reported_amount = amount - extra_samples;
			_samples_remaining = 0;

			return reported_amount;
		}
	}

	return amount;
}

size_t SampleFileRead::ReadBytes(AudioByte *buffer, size_t numBytes)
{
	
	_stream.read(reinterpret_cast<char *>(buffer), numBytes);
  
	return _stream.gcount();
}
