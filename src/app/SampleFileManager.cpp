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

// SampleFileManager.cpp: implementation of the CSampleFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "app.h"
#include "SampleFileManager.h"

#include <fstream>
using namespace std;
#include <assert.h>

#include <math.h>

#include "rfftw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

UINT SamplePlayThreadFunc( LPVOID pParam )
{
	CSampleFileManager* sample_manager = (CSampleFileManager*)pParam;

	//TRACE0( "\nCallingPlay");
	sample_manager->PlayThread();


	return( 0 );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleFileManager::CSampleFileManager()
{
	_reader = NULL;
	_repeat = false;
	_play_thread = NULL;
	_player = NULL;
	_file = NULL;
}

CSampleFileManager::~CSampleFileManager()
{
	if( _play_thread )
	{
		_player->Stop();
		::WaitForSingleObject( *_play_thread, INFINITE );
		delete _play_thread;
		_play_thread = NULL;
	}
	
	if( _player )
		delete _player;
	
	if( _reader )	
	{	
		delete _reader;
	}

	if( _file )
		delete _file;
	
}

bool CSampleFileManager::Open( string sample_file_name )
{
	if( _file )
	{
		return false;
	}

	_file = new ifstream( sample_file_name.c_str(), ios::in|ios::binary );
	
	if( !_file->is_open() )
	{
		delete _file;
		_file = NULL;
		return false;
	}

	_reader = new SampleFileRead( *_file );
	if( ! _reader->IsValid() )
	{
		delete _reader;
		_reader = NULL;
	
		delete _file;
		_file = NULL;
		return false;

		::MessageBox
			(
				NULL,
				_T("Sample file is not valid"),
				_T("Open"),
				MB_OK | MB_ICONWARNING
			);

		return false;
	}

	_file_end.SetFromSampleCount( _reader->SizeInSamples(), _reader->SamplingRate(),
											_reader->Channels() );

	_selection_start.SetFromSubframes( 0 );
	_selection_end.SetFromSubframes( 0 );
	
	_position.SetFromSubframes( 0 );
	
	_player = new WinPlayer( _reader );

	return true;
}


bool CSampleFileManager::OpenWave( string wave_file_name, string new_sample_file_name )
{
	if( _reader )
		return false;

	bool converted = CreateSampleFileFromWave( wave_file_name, new_sample_file_name );
	if( !converted )
		return false;

	return Open( new_sample_file_name );
}

bool CSampleFileManager::Close()
{
	if( _player )
	{
		if( _play_thread )
		{
			_player->Stop();
			::WaitForSingleObject( *_play_thread, INFINITE );
			delete _play_thread;
			_play_thread = NULL;
		}

		delete _player;
		_player = NULL;
	}

	if( _reader )
	{
		delete _reader;
		_reader = NULL;
	}

	if( _file )
	{
		delete _file;
		_file = NULL;
	}

	return true;
}

bool CSampleFileManager::IsValid()
{
	if( _reader && _reader->IsValid() )
		return true;

	return false;
}

CAudioTime CSampleFileManager::GetLength()
{
	assert( _reader );

	CAudioTime time;

	time.SetFromSampleCount( _reader->SizeInSamples(), _reader->SamplingRate(), _reader->Channels() );

	return time;

}
	
CAudioTime CSampleFileManager::GetSelectionStart()
{
	return _selection_start;

}

CAudioTime CSampleFileManager::GetSelectionEnd()
{
	return _selection_end;

}
	
bool CSampleFileManager::SetSelectionStart(CAudioTime start_time)
{
	if( start_time <= _selection_end )
	{
		_selection_start = start_time;
		return true;
	}
	return false;
}

bool CSampleFileManager::SetSelectionEnd(CAudioTime end_time)
{
	if( end_time >= _selection_start )
	{
		_selection_end = end_time;
		return true;
	}
	return false;
}

bool CSampleFileManager::SetSelection( CAudioTime start_time, CAudioTime end_time )
{
	if( start_time <= end_time )
	{
		_selection_start = start_time;
		_selection_end = end_time;
		return true;
	}
	return false;
}
	
void CSampleFileManager::ClearSelection()
{
	_selection_end.SetFromSubframes(0);
	_selection_start.SetFromSubframes(0);
}

bool CSampleFileManager::SetPosition(CAudioTime position_time )
{
	if( position_time <= _file_end )
	{
		_position = position_time;
		return true;
	}
	return false;
}

CAudioTime CSampleFileManager::GetPosition()
{
	return _position;
}

void CSampleFileManager::SetRepeat( bool repeat )
{
	_repeat = repeat;
}

void CSampleFileManager::Play()
{
	assert( _reader );
	
	if( _play_thread )
	{
		if( ::WaitForSingleObject( *_play_thread, 0 ) == WAIT_TIMEOUT )
			_player->Stop();		
		::WaitForSingleObject( *_play_thread, INFINITE );
		delete _play_thread;
		_play_thread = NULL;
	}
	
	_playing_lock.Lock();

	_reader->GoToSample( _selection_start.CalculateStartingSample( _player->SamplingRate(), _player->Channels() ));
	
	if( ! (_selection_start == _selection_end) )
		_reader->SetSampleRange( _selection_start.CalculateSamplesInRange( _selection_end, _player->SamplingRate(), _player->Channels()));
	else
		_reader->SetSampleRange( -1 );

	_play_thread = AfxBeginThread(SamplePlayThreadFunc, this,
		THREAD_PRIORITY_NORMAL,
		0, CREATE_SUSPENDED, NULL);

	_play_thread->m_bAutoDelete = FALSE;	
	_play_thread->ResumeThread();	
}
	
void CSampleFileManager::Stop()
{

	if( _play_thread )
	{
		if( ::WaitForSingleObject( *_play_thread, 0 ) == WAIT_TIMEOUT )
			_player->Stop();				
		::WaitForSingleObject( *_play_thread, INFINITE );
		delete _play_thread;
		_play_thread = NULL;
	}

}

void CSampleFileManager::PlayThread()
{
	_player->Play();

	_reader->SetSampleRange(-1);
	_playing_lock.Unlock();
}

bool CSampleFileManager::AnalyzeSelectionToGraph( CMusicGraph& graph )
// precondition: note range of graph should be set, but it can actually
// be set later
{
	assert( _reader );	
	
	size_t samples_read = 0;
	
	
	size_t samples_to_try = _selection_start.CalculateSamplesInRange( _selection_end,
		_reader->SamplingRate(), _reader->Channels() );

	int channels = _reader->Channels();

	AudioSample* samples = new AudioSample[ samples_to_try * channels ];
	
	bool success = _reader->GoToSample( _selection_start.CalculateStartingSample( 
		                       _reader->SamplingRate(), _reader->Channels() 
									                                      ));
	if( !success )
	{
			::MessageBox
			(
				NULL,
				_T("Unable to seek to first sample"),
				_T("AnalyzeSelectionToGraph"),
				MB_OK | MB_ICONWARNING
			);

			delete [] samples;
			return false;
	}

	samples_read = _reader->GetSamples( samples, samples_to_try * channels );

	int samples_to_analyze =  samples_read / channels;
	fftw_real* in = new fftw_real[ samples_to_analyze];
	fftw_real* out = new fftw_real[ samples_to_analyze ];

	if (samples_to_analyze > 5000)
	{
		int power = this->NearestPowerOfTwo(samples_to_analyze);

		samples_to_analyze = pow(2, power);
	}

	rfftw_plan plan = rfftw_create_plan( samples_to_analyze, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE );

	if( channels > 1 )
	{
		for( int i = 0; i < samples_to_analyze; i++ )
		{
			// convert from stereo to mono
			double sum = 0;		
			for( int chan = 0; chan < channels; chan++ )
				sum += samples[i * channels + chan];
			sum /= channels;

			in[i] = sum;
		}
	}
	else // channels == 1
	{
		for( int i = 0; i < samples_to_analyze; i++ )
		{
			in[i] = samples[i];
		}
	}

	// do the fourier transform, storing results into out
	rfftw_one( plan, in, out );		

	// put the results into the graph
	graph.ClearData();

	//graph.SetNoteRange(21, 108);	

	long sampling_rate = _reader->SamplingRate();			
	
	graph.AddFrequency( 0, out[0] * out[0] );

	for (int k = 1; k < (samples_to_analyze+1)/2; ++k)  
	{
		graph.AddFrequency( k/((double)samples_to_analyze) * sampling_rate,
				             (out[k] * out[k] + out[samples_to_analyze-k] * out[samples_to_analyze-k]));
	}
	if (samples_read % 2 == 0) 
	{
		graph.AddFrequency( 0.5 * sampling_rate, out[samples_to_analyze/2] * out[samples_to_analyze/2]);
	}
	
	rfftw_destroy_plan( plan );

	delete [] out;
	delete [] in;
	delete [] samples;



	return true;
}

int CSampleFileManager::NearestPowerOfTwo(int samples) {
	int result = log(samples) / (double)log(2);

	return result;
}

bool CSampleFileManager::FileToWaveformGraph( CWaveformGraph& graph )
// precondition: viewing range of graph should be set, but it can actually
// be set later
{
	assert( _reader );	
	
	size_t samples_read = 0;

	CAudioTime start;

	size_t samples_to_try = start.CalculateSamplesInRange( _file_end,
		_reader->SamplingRate(), _reader->Channels() );

	int channels = _reader->Channels();
	long sampling_rate = _reader->SamplingRate();		

	AudioSample* samples = new AudioSample[ samples_to_try * channels ];
	int starting_offset = start.CalculateStartingSample( 
		                       _reader->SamplingRate(), _reader->Channels() );

	bool success = _reader->GoToSample( starting_offset );
	if( !success )
	{
			::MessageBox
			(
				NULL,
				_T("Unable to seek to first sample"),
				_T("FileToWaveformGraph"),
				MB_OK | MB_ICONWARNING
			);

			delete [] samples;
			return false;
	}

	samples_read = _reader->GetSamples( samples, samples_to_try * channels );

	int samples_to_analyze =  samples_read / channels;

	graph.ClearData();

	if( channels > 1 )
	{
		assert( false );
	}
	else // channels == 1, we are in mono
	{
		CAudioTime last;
		int running_total = 0;
		int running_count = 0;
		for( int i = 0; i < samples_to_analyze; i++ )
		{
			CAudioTime time;
			time.SetFromSampleCount( i + starting_offset, sampling_rate, channels );
			if( time == last )
			{
				running_total += abs( samples[i] );
				running_count++;
			}
			else // we have moved on to the new time, so write
			{    // the value for the previous time

				// set the pulse to be the average of the magnitudes for the
				// previous time
				if( running_count > 0 )
				{
					WavePulse pulse( time, running_total / running_count );
					graph.AddPulse( pulse );
				}
				running_total = abs( samples[i] );
				running_count = 1;
				last = time;
				
			}			
		}
	}

	delete [] samples;

	return true;
}