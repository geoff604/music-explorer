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

// SelectRangeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "app.h"
#include "SelectRangeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectRangeDialog dialog


CSelectRangeDialog::CSelectRangeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectRangeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectRangeDialog)
	m_start_hours = 0;
	m_start_min = 0;
	m_start_sec = 0;
	m_start_frames = 0;
	m_length = _T("");
	m_end_frames = 0;
	m_end_hours = 0;
	m_end_min = 0;
	m_end_sec = 0;
	//}}AFX_DATA_INIT
}


void CSelectRangeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectRangeDialog)
	DDX_Text(pDX, IDC_EDITSTARTHOURS, m_start_hours);
	DDV_MinMaxInt(pDX, m_start_hours, 0, 32767);
	DDX_Text(pDX, IDC_EDITSTARTMIN, m_start_min);
	DDV_MinMaxInt(pDX, m_start_min, 0, 59);
	DDX_Text(pDX, IDC_EDITSTARTSEC, m_start_sec);
	DDV_MinMaxInt(pDX, m_start_sec, 0, 59);
	DDX_Text(pDX, IDC_EDITSTARTFRAMES, m_start_frames);
	DDV_MinMaxInt(pDX, m_start_frames, 0, 29);
	DDX_Text(pDX, IDC_EDITLENGTH, m_length);
	DDX_Text(pDX, IDC_EDITENDFRAMES, m_end_frames);
	DDV_MinMaxInt(pDX, m_end_frames, 0, 29);
	DDX_Text(pDX, IDC_EDITENDHOURS, m_end_hours);
	DDV_MinMaxInt(pDX, m_end_hours, 0, 32767);
	DDX_Text(pDX, IDC_EDITENDMIN, m_end_min);
	DDV_MinMaxInt(pDX, m_end_min, 0, 59);
	DDX_Text(pDX, IDC_EDITENDSEC, m_end_sec);
	DDV_MinMaxInt(pDX, m_end_sec, 0, 59);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectRangeDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectRangeDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectRangeDialog message handlers
