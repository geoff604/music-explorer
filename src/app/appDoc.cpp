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

// appDoc.cpp : implementation of the CAppDoc class
//

#include "stdafx.h"
#include "app.h"

#include "appDoc.h"


#include "sinewave.h"
#include "aplayer.h"
#include "winplayr.h"
#include "wav.h"
#include <fstream>
#include <sstream>
using namespace std;

#include "rfftw.h"

#include "linegraph.h"
#include "selectrangedialog.h"
#include "samplefileread.h"
#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppDoc

IMPLEMENT_DYNCREATE(CAppDoc, CDocument)

BEGIN_MESSAGE_MAP(CAppDoc, CDocument)
	//{{AFX_MSG_MAP(CAppDoc)
	ON_COMMAND(ID_FUNCTIONS_TESTLINEGRAPH, OnFunctionsTestlinegraph)
	ON_COMMAND(ID_FUNCTIONS_FFTTOGRAPH, OnFunctionsFfttograph)
	ON_COMMAND(ID_FUNCTIONS_SELECTRANGE, OnFunctionsSelectRange)
	ON_COMMAND(ID_FUNCTIONS_TESTFILECONVERSION, OnFunctionsTestFileConversion)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_PLAY, OnPlay)
	ON_COMMAND(ID_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppDoc construction/destruction

CAppDoc::CAppDoc()
{
	// TODO: add one-time construction code here

}

CAppDoc::~CAppDoc()
{
}

BOOL CAppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAppDoc serialization

void CAppDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAppDoc diagnostics

#ifdef _DEBUG
void CAppDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAppDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAppDoc commands

void CAppDoc::OnFunctionsTestlinegraph() 
{
      music_graph.ClearData();		
		music_graph.SetNoteRange(60, 72);
		keyboard.SetNoteRange(60, 72);		
		
		music_graph.AddFrequency( 369.99, 0);
		music_graph.AddFrequency( 369.99, 10);
		music_graph.AddFrequency( 369.99, 0);

		music_graph.AddFrequency( 440, 0 );
		music_graph.AddFrequency( 440, 10 );		
		
		


	//SetModifiedFlag(TRUE);
	UpdateAllViews(NULL);
	
}

void CAppDoc::OnFunctionsFfttograph() 
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Wave Files (*.wav)|*.wav||", NULL);
	
	if (dlgFile.DoModal() != IDOK)
	{
		return;	

	}
		
	CString input_file_name  = dlgFile.GetPathName();
	
	ifstream file;
	file.open( input_file_name, ios::in|ios::binary );
	if( file.is_open() )
	{
		WaveRead wav( file );
		size_t samples_read = 0;
		
		size_t samples_to_try = 44000; // get lots of samples

		int channels = wav.Channels();

		AudioSample* samples = new AudioSample[ samples_to_try * channels ];
		samples_read = wav.GetSamples( samples, samples_to_try * channels );

		int samples_to_analyze =  samples_read / channels;
		fftw_real* in = new fftw_real[ samples_to_analyze];
		fftw_real* out = new fftw_real[ samples_to_analyze ];

		rfftw_plan plan = rfftw_create_plan( samples_to_analyze, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE );

		for( int i = 0; i < samples_to_analyze; i++ )
		{
			// convert from stereo to mono
			double sum = 0;
			for( int chan = 0; chan < channels; chan++ )
				sum += samples[i * channels + chan];
			sum /= channels;

			in[i] = sum;
		}

		rfftw_one( plan, in, out );		

		music_graph.ClearData();
		//graph.SetHorizDataRange( 27, 4186 );
		music_graph.SetNoteRange(21, 108);
		keyboard.SetNoteRange(21, 108);

		long sampling_rate = wav.SamplingRate();			
		
		music_graph.AddFrequency( 0, out[0] * out[0] );

		for (int k = 1; k < (samples_to_analyze+1)/2; ++k)  
		{
			music_graph.AddFrequency( k/((double)samples_to_analyze) * sampling_rate,
				                (out[k] * out[k] + out[samples_to_analyze-k] * out[samples_to_analyze-k]));
		}
		if (samples_read % 2 == 0) 
		{
			music_graph.AddFrequency( 0.5 * sampling_rate, out[samples_to_analyze/2] * out[samples_to_analyze/2]);
		}
		
		rfftw_destroy_plan( plan );

		delete [] out;
		delete [] in;
		delete [] samples;
		
		//SetModifiedFlag(TRUE);
		UpdateAllViews(NULL);
		
	}
	else
	{
		CString strerror;
		strerror = "Unable to open file";
		AfxMessageBox( strerror );
	}	
}

void CAppDoc::OnFunctionsSelectRange() 
{
	CSelectRangeDialog dlg;
	
	CAudioTime file_end;
	CAudioTime selection_start;
	CAudioTime selection_end;

	if( sample_manager.IsValid() )
	{
		file_end = sample_manager.GetLength();
		selection_start = sample_manager.GetSelectionStart();
		selection_end = sample_manager.GetSelectionEnd();
	}

	ostringstream str;
	str << file_end.hours << ":" << file_end.minutes << ":" << file_end.seconds
		  << ":" << file_end.frames;

	dlg.m_length = str.str().c_str();

	dlg.m_start_hours = selection_start.hours;
	dlg.m_start_min = selection_start.minutes;
	dlg.m_start_sec = selection_start.seconds;
	dlg.m_start_frames = selection_start.frames;

	dlg.m_end_hours = selection_end.hours;
	dlg.m_end_min = selection_end.minutes;
	dlg.m_end_sec = selection_end.seconds;
	dlg.m_end_frames = selection_end.frames;

	if( IDOK == dlg.DoModal() )
	{
		selection_start.hours = dlg.m_start_hours;
		selection_start.minutes = dlg.m_start_min;
		selection_start.seconds = dlg.m_start_sec;
		selection_start.frames = dlg.m_start_frames;

		selection_end.hours = dlg.m_end_hours;
		selection_end.minutes = dlg.m_end_min;
		selection_end.seconds = dlg.m_end_sec;
		selection_end.frames = dlg.m_end_frames;

		if( sample_manager.IsValid() )
		{
			sample_manager.SetSelectionStart( selection_start );
			sample_manager.SetSelectionEnd( selection_end );
			sample_manager.AnalyzeSelectionToGraph( music_graph );
			
			wave_graph.SetSelectionRange( selection_start, selection_end );
		
			UpdateAllViews(NULL);			
		}
	}
}

void CAppDoc::OnFunctionsTestFileConversion() 
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Wave Files (*.wav)|*.wav||", NULL);
	
	if (dlgFile.DoModal() != IDOK)
	{
		return;	

	}
		
	string input_file_name  = dlgFile.GetPathName();

	CreateSampleFileFromWave( input_file_name, "output.sam" );
}

void CAppDoc::OnFileOpen() 
{
	if( sample_manager.IsValid() )
	{
		sample_manager.Close();
	}

	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Wave Files (*.wav)|*.wav||", NULL);
	
	if (dlgFile.DoModal() != IDOK)
	{
		return;	
	}

	string input_file_name  = dlgFile.GetPathName();

	const int buff_size = 200;
	char buff[buff_size];
	DWORD length = GetTempPath( buff_size - 1, buff );
	string temp_filename = buff;
	temp_filename = temp_filename + "temp.sam";

	BeginWaitCursor();
	bool open_success = sample_manager.OpenWave( input_file_name, temp_filename );
	EndWaitCursor();
	if( open_success )
	{
		music_graph.ClearData();
		music_graph.SetNoteRange(21, 108);
		keyboard.SetNoteRange(21, 108);

		BeginWaitCursor();
		sample_manager.FileToWaveformGraph( wave_graph );
		EndWaitCursor();
		CAudioTime beginning;
		wave_graph.SetHorizRange( beginning, sample_manager.GetLength() );
		wave_graph.ClearSelection();
		this->UpdateAllViews(NULL, UPDATE_NEW_FILE);
	}
	else
	{
		::MessageBox
		(
			NULL,
			_T("Unable to open file"),
			_T("OnFileOpen"),
			MB_OK | MB_ICONWARNING
		);
		this->UpdateAllViews(NULL);
	}

	
}

void CAppDoc::OnPlay() 
{
	if( sample_manager.IsValid() )
		sample_manager.Play();
}

void CAppDoc::OnStop() 
{
	if( sample_manager.IsValid() )
		sample_manager.Stop();	
}
