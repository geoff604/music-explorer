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

// appDoc.h : interface of the CAppDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPDOC_H__EAF8580F_7C10_497F_92E3_FD4B86CEFE17__INCLUDED_)
#define AFX_APPDOC_H__EAF8580F_7C10_497F_92E3_FD4B86CEFE17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "musicgraph.h"
#include "keyboard.h"
#include "audiotime.h"

#include "samplefilemanager.h"

const int UPDATE_NEW_FILE = 1;

class CAppDoc : public CDocument
{
protected: // create from serialization only
	CAppDoc();
	DECLARE_DYNCREATE(CAppDoc)

// Attributes
public:
	CMusicGraph music_graph;
	CKeyboard keyboard;
	CSampleFileManager sample_manager;
	CWaveformGraph wave_graph;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAppDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAppDoc)
	afx_msg void OnFunctionsTestlinegraph();
	afx_msg void OnFunctionsFfttograph();
	afx_msg void OnFunctionsSelectRange();
	afx_msg void OnFunctionsTestFileConversion();
	afx_msg void OnFileOpen();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPDOC_H__EAF8580F_7C10_497F_92E3_FD4B86CEFE17__INCLUDED_)
