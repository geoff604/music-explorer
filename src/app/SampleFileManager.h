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

// SampleFileManager.h: interface for the CSampleFileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEFILEMANAGER_H__D76CC6B3_EF69_4A2E_B554_7BC98C1F103A__INCLUDED_)
#define AFX_SAMPLEFILEMANAGER_H__D76CC6B3_EF69_4A2E_B554_7BC98C1F103A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "samplefileread.h"
#include "audiotime.h"
#include "musicgraph.h"
#include "waveformgraph.h"

#include "winplayr.h"

#include <afxmt.h>

#include <string>
using namespace std;

#include <fstream>

class CSampleFileManager  
{
public:
	CSampleFileManager();
	virtual ~CSampleFileManager();

	bool Open( string sample_file_name );
	bool OpenWave( string wave_file_name, string new_sample_file_name );

	bool Close();

	bool IsValid();
	CAudioTime GetLength();
	
	CAudioTime GetSelectionStart();
	CAudioTime GetSelectionEnd();
	bool SetSelectionStart(CAudioTime start_time);
	bool SetSelectionEnd(CAudioTime end_time);
	bool SetSelection( CAudioTime start_time, CAudioTime end_time );
	void ClearSelection();
	
	bool SetPosition(CAudioTime position_time );
	CAudioTime GetPosition();

	void SetRepeat( bool repeat );
	void Play();
	void Stop();

	bool AnalyzeSelectionToGraph( CMusicGraph& graph );	
	bool FileToWaveformGraph( CWaveformGraph& graph );

private:
	SampleFileRead* _reader;

	CAudioTime _selection_start;
	CAudioTime _selection_end;
	CAudioTime _position;
	CAudioTime _file_end;

	bool _repeat;

	ifstream * _file;

	WinPlayer * _player;

	CWinThread * _play_thread;

	friend UINT SamplePlayThreadFunc( LPVOID pParam );

	void PlayThread();

	CCriticalSection _playing_lock;

	int NearestPowerOfTwo(int samples);

};

#endif // !defined(AFX_SAMPLEFILEMANAGER_H__D76CC6B3_EF69_4A2E_B554_7BC98C1F103A__INCLUDED_)
