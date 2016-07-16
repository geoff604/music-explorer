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


// appView.cpp : implementation of the CAppView class
//

#include "stdafx.h"
#include "app.h"



#include "appDoc.h"
#include "appView.h"

#include "sinewave.h"
#include "aplayer.h"
#include "winplayr.h"
#include "wav.h"
#include <fstream>

#include <math.h>

#include "rfftw.h"

#include "linegraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT SineThreadFunc( LPVOID pParam )
{
	CSinePlayer* sine_player = (CSinePlayer*)pParam;

	TRACE0( "\nCallingPlay");
	sine_player->Play();

	return( 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CAppView

IMPLEMENT_DYNCREATE(CAppView, CView)

BEGIN_MESSAGE_MAP(CAppView, CView)
	//{{AFX_MSG_MAP(CAppView)
	ON_COMMAND(ID_FUNCTIONS_FFTTESTER, OnFunctionsFfttester)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_FUNCTIONS_TESTSINEWAVE, OnFunctionsTestSineWave)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_VIEW_VERTICALZOOMIN, OnViewVerticalZoomIn)
	ON_COMMAND(ID_VIEW_VERTICALZOOMOUT, OnViewVerticalZoomOut)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_NOTE, OnUpdateNote)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppView construction/destruction

CAppView::CAppView()
{
	m_sine_thread = NULL;
}

CAppView::~CAppView()
{
}

BOOL CAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAppView drawing

void CAppView::OnDraw(CDC* pDC)
{
	const int keyboard_height = 50;

	CAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// load keyboard scrollbar values

	SCROLLINFO si;
	si.fMask=SIF_ALL;
	
	m_keyboard_zoom_scrollBar.GetScrollInfo( &si );
	int current_keyboard_range = si.nPos;
	int max_keyboard_range = si.nMax;

	m_keyboard_range_scrollBar.GetScrollInfo( &si );	
	int current_keyboard_pos = si.nPos;	
	int max_keyboard_pos = si.nMax;

	if( max_keyboard_pos - (current_keyboard_pos - 1) < current_keyboard_range )
	{
		current_keyboard_pos = max_keyboard_pos - (current_keyboard_range - 1 );	
		si.nPos = current_keyboard_pos;
		m_keyboard_range_scrollBar.SetScrollInfo( &si );	
	}

	pDoc->keyboard.SetNoteRange( current_keyboard_pos, 
		                          current_keyboard_pos + (current_keyboard_range - 1 ));
	pDoc->music_graph.SetNoteRange( current_keyboard_pos, 
		                          current_keyboard_pos + (current_keyboard_range - 1 ));

	// load waveform scrollbar values

	m_wave_zoom_scrollBar.GetScrollInfo( &si );
	int current_wave_range = si.nPos;
	int max_wave_range = si.nMax;

	m_wave_range_scrollBar.GetScrollInfo( &si );	
	int current_wave_pos = si.nPos;	
	int max_wave_pos = si.nMax;

	if( max_wave_range > 0 )
	{
		if( max_wave_pos - (current_wave_pos - 1) < current_wave_range )
		{
			current_wave_pos = max_wave_pos - (current_wave_range - 1 );	
			si.nPos = current_wave_pos;
			m_wave_range_scrollBar.SetScrollInfo( &si );	
		}

		CAudioTime start;
		start.SetFromSubframes( current_wave_pos );	
		
		CAudioTime end;
		end.SetFromSubframes( current_wave_pos + (current_wave_range - 1 ));

		pDoc->wave_graph.SetHorizRange( start, end );
	}
	// determine the location of everything in the view and store in rects

	CRect scrollRect;
	m_keyboard_zoom_scrollBar.GetClientRect( scrollRect );
	int scroll_height = scrollRect.Height();

	CRect clientRect;
	this->GetClientRect( clientRect );
	int top_height = clientRect.Height() - keyboard_height - (2*scroll_height);
	int graph_height = top_height / 2;
	int wave_height = top_height - graph_height;

	m_waveform_rect = CRect( 0, 0, clientRect.Width(), wave_height );
	m_music_rect = CRect(0, wave_height + scroll_height, clientRect.Width(), 
		wave_height + scroll_height + graph_height);
	m_keyboard_rect = CRect( 0, top_height + scroll_height, clientRect.Width(), top_height 
		+ scroll_height + keyboard_height );

	// draw everything

	if( pDC->RectVisible( &m_keyboard_rect ) )
		pDoc->keyboard.Draw( pDC, m_keyboard_rect.left, m_keyboard_rect.top, 
			m_keyboard_rect.Width(), m_keyboard_rect.Height(), m_music_rect );
	if( pDC->RectVisible( &m_waveform_rect ) )
		pDoc->wave_graph.Draw( pDC, m_waveform_rect.left, m_waveform_rect.top, 
			m_waveform_rect.Width(), m_waveform_rect.Height() );
	if( pDC->RectVisible( &m_music_rect ) )
		pDoc->music_graph.Draw( pDC, m_music_rect.left, m_music_rect.top, 
			m_music_rect.Width(), m_music_rect.Height(), true );
	
	// position the scroll bars

	int first_scrollbar_width = clientRect.Width() / 3;
	int second_scrollbar_width = clientRect.Width() - first_scrollbar_width;

	m_wave_zoom_scrollBar.SetWindowPos( NULL, 0, wave_height, 
		first_scrollbar_width, scroll_height, SWP_NOZORDER | SWP_NOACTIVATE );

	m_wave_range_scrollBar.SetWindowPos( NULL, first_scrollbar_width, 
		wave_height, 
		second_scrollbar_width, scroll_height, SWP_NOZORDER | SWP_NOACTIVATE );

	m_keyboard_zoom_scrollBar.SetWindowPos( NULL, 0, top_height + scroll_height + keyboard_height, 
		first_scrollbar_width, scroll_height, SWP_NOZORDER | SWP_NOACTIVATE );

	m_keyboard_range_scrollBar.SetWindowPos( NULL, first_scrollbar_width, top_height + scroll_height + keyboard_height, 
		second_scrollbar_width, scroll_height, SWP_NOZORDER | SWP_NOACTIVATE );

}

/////////////////////////////////////////////////////////////////////////////
// CAppView diagnostics

#ifdef _DEBUG
void CAppView::AssertValid() const
{
	CView::AssertValid();
}

void CAppView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAppDoc* CAppView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAppDoc)));
	return (CAppDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAppView message handlers

void CAppView::OnUpdateNote(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(); 
	CString strNote;
	if( m_note_text == "" )
		strNote = "";
	else
		strNote.Format( "Clicked: %s", m_note_text.c_str() ); 
	pCmdUI->SetText( strNote ); 

}

void CAppView::OnFunctionsFfttester() 
{
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Wave Files (*.wav)|*.wav||", this);
	
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

		UpdateData( true );
		size_t samples_to_try = 2000;

		AudioSample* samples = new AudioSample[ samples_to_try ];
		samples_read = wav.GetSamples( samples, samples_to_try );

		fftw_real* in = new fftw_real[ samples_read ];
		fftw_real* out = new fftw_real[ samples_read ];

		rfftw_plan plan = rfftw_create_plan( samples_read, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE );

		for( int i = 0; i < samples_read; i++ )
		{
			in[i] = samples[i];
		}

		rfftw_one( plan, in, out );		

		CString output_filename = "d:\\output.csv";
		ofstream output( output_filename );

		output.precision(10);
		output << fixed;
		output.setf(ios_base::fixed, ios_base::floatfield);

		output << out[0] * out[0] << "\n";
		for (int k = 1; k < (samples_read+1)/2; ++k)  
		{
			output << (out[k] * out[k] + out[samples_read-k] * out[samples_read-k]) << "\n";
		}
		if (samples_read % 2 == 0) 
		{
			output << (out[samples_read/2] * out[samples_read/2]) << "\n";
		}
		output.close();

		CString strmsg;
		strmsg = "Output file written to d:\\output.csv";
		AfxMessageBox( strmsg );

		rfftw_destroy_plan( plan );

		delete [] out;
		delete [] in;
		delete [] samples;

		
	}
	else
	{
		CString strerror;
		strerror = "Unable to open file";
		AfxMessageBox( strerror );
	}
	
}

BOOL CAppView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	
	BOOL result = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	if( result )
	{
		CRect clientRect;
		this->GetClientRect( clientRect );

		m_keyboard_zoom_scrollBar.Create( WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
			clientRect, this, IDC_SBKEYZOOM );
			
		m_keyboard_range_scrollBar.Create( WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
			clientRect, this, IDC_SBKEYRANGE );

		SCROLLINFO si;
		si.fMask=SIF_ALL;
		m_keyboard_range_scrollBar.GetScrollInfo( &si );
		si.nMin = 0;
		si.nMax = 127;
		si.nPos = 21;
		si.nPage = 127/6;
		m_keyboard_range_scrollBar.SetScrollInfo( &si );

		si.fMask=SIF_ALL;
		m_keyboard_zoom_scrollBar.GetScrollInfo( &si );
		si.nMin = 1;
		si.nMax = 128;
		si.nPos = 87;
		si.nPage = 127/6;
		m_keyboard_zoom_scrollBar.SetScrollInfo( &si );
		
		m_wave_zoom_scrollBar.Create( WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
			clientRect, this, IDC_SBWAVEZOOM );
	
		m_wave_range_scrollBar.Create( WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
			clientRect, this, IDC_SBWAVERANGE );

		si.fMask=SIF_ALL;
		m_wave_zoom_scrollBar.GetScrollInfo( &si );
		si.nMin = 0;
		si.nMax = 0;
		si.nPos = 0;
		si.nPage = 0;
		m_wave_zoom_scrollBar.SetScrollInfo( &si );

		si.fMask=SIF_ALL;
		m_wave_range_scrollBar.GetScrollInfo( &si );
		si.nMin = 1;
		si.nMax = 1;
		si.nPos = 1;
		si.nPage = 1;
		m_wave_range_scrollBar.SetScrollInfo( &si );

	}
	return result;
}

void CAppView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	SetCapture();

	if( m_waveform_rect.PtInRect( point ) )
	{
		pDoc->wave_graph.SelectionStart( m_waveform_rect.left, m_waveform_rect.top, 
			m_waveform_rect.Width(), m_waveform_rect.Height(),
			point );

		this->InvalidateRect(this->m_waveform_rect);
	}
	else if( m_keyboard_rect.PtInRect( point ) )
	{
		int note = pDoc->keyboard.OnClickNote( m_keyboard_rect.left, m_keyboard_rect.top, 
			m_keyboard_rect.Width(), m_keyboard_rect.Height(),
			point.x, point.y, m_note_text );

		if( note >= 0 )
		{

			double freq = 440 * pow( 2.0, (note - 69) / 12.0 );

			if( m_sine_thread )
			{
				m_sine_player.Stop();
				::WaitForSingleObject( *m_sine_thread, INFINITE );
				delete m_sine_thread;
				m_sine_thread = NULL;
			}

			m_sine_player.Frequency( freq );
			
			m_sine_thread = AfxBeginThread(SineThreadFunc, &m_sine_player,
				THREAD_PRIORITY_NORMAL,
				0, CREATE_SUSPENDED, NULL);

			m_sine_thread->m_bAutoDelete = FALSE;	
			m_sine_thread->ResumeThread();	

			this->InvalidateRect(this->m_keyboard_rect);
		}
	}

	//pDoc->UpdateAllViews(NULL);

	CView::OnLButtonDown(nFlags, point);	
}

void CAppView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if( pDoc->wave_graph.SelectionIsOpen() )
	{
		CRect clientRect;
		this->GetClientRect( clientRect ); //retrieve client rectangle

		if (clientRect.PtInRect(point))
		//if point is in client area, then point is valid
			pDoc->wave_graph.SelectionEnd( m_waveform_rect.left, m_waveform_rect.top, 
				m_waveform_rect.Width(), m_waveform_rect.Height(),
				point );
		else if (point.x < clientRect.left)
		//if point's x value is less than client's leftmost point, use topleft point
		//of waveform area as selection end point
			pDoc->wave_graph.SelectionEnd( m_waveform_rect.left, m_waveform_rect.top,
				m_waveform_rect.Width(), m_waveform_rect.Height(),
				CPoint(m_waveform_rect.left, m_waveform_rect.top) );
		else if (point.x > clientRect.right)
		//if point's x value is less than client's rightmost point, use bottomright point
		//of waveform area as selection end point
			pDoc->wave_graph.SelectionEnd( m_waveform_rect.left, m_waveform_rect.top,
				m_waveform_rect.Width(), m_waveform_rect.Height(),
				CPoint(m_waveform_rect.right, m_waveform_rect.bottom) );
		else if (point.y > clientRect.top)
		//if point's x value is valid, but the y value is greater than client's topmost,
		//use point's x value and waveform area's topmost point as x and y coordinates
		//of selection end point
			pDoc->wave_graph.SelectionEnd( m_waveform_rect.left, m_waveform_rect.top,
				m_waveform_rect.Width(), m_waveform_rect.Height(),
				CPoint(point.x, m_waveform_rect.top) );
		else if (point.y < clientRect.bottom)
		//if point's x value is valid, but the y value is less than client's bottommost,
		//use point's x value and waveform area's bottommost point as x and y coordinates
		//of selection end point
			pDoc->wave_graph.SelectionEnd( m_waveform_rect.left, m_waveform_rect.top,
				m_waveform_rect.Width(), m_waveform_rect.Height(),
				CPoint(point.x, m_waveform_rect.bottom) );


			CAudioTime start = pDoc->wave_graph.GetSelectionStart();
			CAudioTime end = pDoc->wave_graph.GetSelectionEnd();
			pDoc->sample_manager.SetSelection( start, end );

			// update the music graph
			if( start < end && pDoc->sample_manager.IsValid() )
			{
				pDoc->sample_manager.AnalyzeSelectionToGraph( pDoc->music_graph );
			}
			else if (start == end )
			{
				pDoc->music_graph.ClearData();
			}

			this->InvalidateRect(this->m_music_rect);
			this->InvalidateRect(this->m_waveform_rect);			
			
	}

	if( m_sine_thread )
	{
		m_sine_player.Stop();
		::WaitForSingleObject( *m_sine_thread, INFINITE );
		delete m_sine_thread;
		m_sine_thread = NULL;
	
		pDoc->keyboard.OnUnclickNote();

		this->InvalidateRect(this->m_keyboard_rect);
		m_note_text = "";
	
	}

	ReleaseCapture();
	
	CView::OnLButtonUp(nFlags, point);

}

void CAppView::OnFunctionsTestSineWave() 
{
	//CSinePlayer player;
	//player.Play();

		
	SineWave sinewave;

	WinPlayer player( &sinewave );
	sinewave.Frequency( 440 );
	player.Play();
	
}

void CAppView::UpdateWaveForm()
{
	InvalidateRect( m_waveform_rect );
}

void CAppView::UpdateKeyboardAndGraph()
{
	InvalidateRect( m_keyboard_rect );
	InvalidateRect( m_music_rect );
}

void CAppView::UpdateBasedOnScrollID( int id )
{
	if( id == IDC_SBKEYZOOM || id == IDC_SBKEYRANGE )
		UpdateKeyboardAndGraph();
	else if( id == IDC_SBWAVEZOOM || id == IDC_SBWAVERANGE )
		UpdateWaveForm();

}

void CAppView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( pScrollBar )
	{
		int id = pScrollBar->GetDlgCtrlID();
		
		if( id == IDC_SBKEYZOOM || id == IDC_SBKEYRANGE || id == IDC_SBWAVEZOOM 
			|| id == IDC_SBWAVERANGE )
		{
		
			bool update = false;

			// Get the minimum and maximum scroll-bar positions.
			int minpos;
			int maxpos;
			pScrollBar->GetScrollRange(&minpos, &maxpos); 
			maxpos = pScrollBar->GetScrollLimit();

			// Get the current position of scroll box.
			int curpos = pScrollBar->GetScrollPos();

			// Determine the new position of scroll box.
			switch (nSBCode)
			{
			case SB_LEFT:      // Scroll to far left.
				curpos = minpos;
				update = true;
				break;

			case SB_RIGHT:      // Scroll to far right.
				curpos = maxpos;
				update = true;
				break;

			case SB_ENDSCROLL:   // End scroll.
				break;

			case SB_LINELEFT:      // Scroll left.
				if (curpos > minpos)
					curpos--;
				update = true;
				break;

			case SB_LINERIGHT:   // Scroll right.
				if (curpos < maxpos)
					curpos++;
				update = true;
				break;

			case SB_PAGELEFT:    // Scroll one page left.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);
   
				if (curpos > minpos)
				{
					curpos = max(minpos, curpos - (int) info.nPage);
					update = true;
				}
			}
				break;

			case SB_PAGERIGHT:      // Scroll one page right.
			{
				// Get the page size. 
				SCROLLINFO   info;
				pScrollBar->GetScrollInfo(&info, SIF_ALL);

				if (curpos < maxpos)
				{
					curpos = min(maxpos, curpos + (int) info.nPage);
					update = true;
				}
			}
				break;

			case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
				curpos = nPos;      // of the scroll box at the end of the drag operation.
				update = true;
				break;

			case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
				curpos = nPos;     // position that the scroll box has been dragged to.
				update = true;
				break;
			}

			// Set the new position of the thumb (scroll box).
			pScrollBar->SetScrollPos(curpos);
			if( update )
				UpdateBasedOnScrollID( id );
		}
	}

	
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAppView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	
	if( lHint == UPDATE_NEW_FILE )
	{
		CAppDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		
		if( pDoc->sample_manager.IsValid() )
		{

			// set the extents on the scroll bar and the waveform graph.
			CAudioTime length = pDoc->sample_manager.GetLength();
			long subframes = length.GetAllInSubframes();

			SCROLLINFO si;
			si.fMask=SIF_ALL;
			m_wave_range_scrollBar.GetScrollInfo( &si );
			si.nMin = 0;
			si.nMax = subframes-1;
			si.nPos = 0;
			si.nPage = (subframes-1)/6;
			m_wave_range_scrollBar.SetScrollInfo( &si );

			CAudioTime three_seconds;
			three_seconds.seconds = 3;
			long show_subframes = three_seconds.GetAllInSubframes();

			if( show_subframes > subframes )
				show_subframes = subframes;

			m_wave_zoom_scrollBar.GetScrollInfo( &si );
			si.nMin = 1;
			si.nMax = subframes;
			si.nPos = show_subframes;
			si.nPage = (subframes)/6;
			m_wave_zoom_scrollBar.SetScrollInfo( &si );
		}
	}

	CView::OnUpdate( pSender, lHint, pHint );	
}

const short adjust_vertical_zoom = 5000;

void CAppView::OnViewVerticalZoomIn() 
{
	CAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if( pDoc->sample_manager.IsValid() )
	{
		
		short extent = pDoc->wave_graph.GetVertExtent();
		extent = (2*extent - adjust_vertical_zoom)/2;
		pDoc->wave_graph.SetVertExtent(extent);
		UpdateWaveForm();
	}
}

void CAppView::OnViewVerticalZoomOut() 
{
	CAppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if( pDoc->sample_manager.IsValid() )
	{
		short extent = pDoc->wave_graph.GetVertExtent();
		extent = (2*extent + adjust_vertical_zoom)/2;
		pDoc->wave_graph.SetVertExtent(extent);
		UpdateWaveForm();
	}
	
}
