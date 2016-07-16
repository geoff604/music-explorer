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

#if !defined(AFX_SELECTRANGEDIALOG_H__CADC27D9_5A82_4F29_8AAC_AEB094A5CBA4__INCLUDED_)
#define AFX_SELECTRANGEDIALOG_H__CADC27D9_5A82_4F29_8AAC_AEB094A5CBA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectRangeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectRangeDialog dialog

class CSelectRangeDialog : public CDialog
{
// Construction
public:
	CSelectRangeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectRangeDialog)
	enum { IDD = IDD_SELECTRANGEDIALOG };
	int		m_start_hours;
	int		m_start_min;
	int		m_start_sec;
	int		m_start_frames;
	CString	m_length;
	int		m_end_frames;
	int		m_end_hours;
	int		m_end_min;
	int		m_end_sec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectRangeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectRangeDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTRANGEDIALOG_H__CADC27D9_5A82_4F29_8AAC_AEB094A5CBA4__INCLUDED_)
