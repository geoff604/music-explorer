/////////////////////////////////////////////////////////////////////////////////////////////////
//				Class		: CToolTipCtrl													   //
//										 													   //
//				Author		: Natarajan T													   //
//																							   //
//				Version		: 1.0															   //
//																							   //
//				Purpose		: C++ Class to encapsulate Windows ToolTip Control.				   //
//																							   //
//				Disclaimer	: This Source code is given in asis condition and nothimg is 	   //
//								guaranted, The Source code is to be used on own risk.		   //
//								This Source code is distributed in good faith and Author is not//
//								responsible for any damage done, to the user, in any manner    //
//								perceived to be caused by the use of this source code.		   //
//																							   //
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLTIPCTRL_H__40E84E40_A166_11D5_AA56_F34FF958073B__INCLUDED_)
#define AFX_TOOLTIPCTRL_H__40E84E40_A166_11D5_AA56_F34FF958073B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment (lib, "comctl32.lib")

#include <Windows.h>
#include <commctrl.h>

class CToolTipCtrl  
{
public:
	CToolTipCtrl(HWND hWndParent);
	virtual ~CToolTipCtrl();
	void SetFont(CONST LOGFONT *lplf);
	void SetMaxTipWidth(int iWidth);
	void SetTipTextColor(COLORREF clr);
	void SetBkTipColor(COLORREF clr);
	void AddToolForWnd(HWND hWnd,LPSTR lpszToolTip);
	HWND hToolTip;

private:
	static CToolTipCtrl * pThis;
	static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
	HHOOK hMsgHook;
	HWND hWndParent;
};

#endif // !defined(AFX_TOOLTIPCTRL_H__40E84E40_A166_11D5_AA56_F34FF958073B__INCLUDED_)
