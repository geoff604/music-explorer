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

// appView.h : interface of the CAppView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPVIEW_H__F7F2D5F2_FC03_4DC3_9030_4171E440AF24__INCLUDED_)
#define AFX_APPVIEW_H__F7F2D5F2_FC03_4DC3_9030_4171E440AF24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxwin.h>

#include <string>
using namespace std;

#include "sineplayer.h"

class CAppView : public CView
{
protected: // create from serialization only
	CAppView();
	DECLARE_DYNCREATE(CAppView)

	void UpdateWaveForm();
	void UpdateKeyboardAndGraph();
	void UpdateBasedOnScrollID( int id );

// Attributes
public:
	CAppDoc* GetDocument();

	CScrollBar m_keyboard_zoom_scrollBar;
	CScrollBar m_keyboard_range_scrollBar;
	CScrollBar m_wave_zoom_scrollBar;
	CScrollBar m_wave_range_scrollBar;

	CRect m_keyboard_rect;
	CRect m_waveform_rect;
	CRect m_music_rect;

	CSinePlayer m_sine_player;

	bool m_playing_sine;

	CWinThread * m_sine_thread;

	string m_note_text;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAppView)
	afx_msg void OnFunctionsFfttester();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFunctionsTestSineWave();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewVerticalZoomIn();
	afx_msg void OnViewVerticalZoomOut();
	//}}AFX_MSG
	afx_msg void OnUpdateNote(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in appView.cpp
inline CAppDoc* CAppView::GetDocument()
   { return (CAppDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPVIEW_H__F7F2D5F2_FC03_4DC3_9030_4171E440AF24__INCLUDED_)
