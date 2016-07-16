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
#include "stdafx.h"

#include "ToolTipCtrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolTipCtrl::CToolTipCtrl(HWND hWndParent)
{
	if (hWndParent == NULL)
		return;
	// Parent window Handle Cant be Null.

	//Initialize Common Controls for ToolTip Common Control
	INITCOMMONCONTROLSEX icx;
	icx.dwSize	= sizeof(icx);
	icx.dwICC	= ICC_BAR_CLASSES;
	InitCommonControlsEx(&icx);

	CToolTipCtrl::hWndParent	= hWndParent;
	CToolTipCtrl::pThis			= this;

	DWORD dwStyle				= WS_POPUP | WS_BORDER | TTS_ALWAYSTIP;
	DWORD dwExStyle				= WS_EX_TOOLWINDOW | WS_EX_TOPMOST;

	hToolTip					= CreateWindowEx(dwExStyle,TOOLTIPS_CLASS, 
									NULL,dwStyle,0,0,0,0, 
									hWndParent,	NULL, 
									GetModuleHandle(NULL),NULL);

	if (!hToolTip)
	{
		MessageBox(hWndParent,"Error Creating ToolTip Control","ToolTipClass",MB_OK | MB_ICONSTOP);
		// Close Main Window if Required.
		//PostMessage(hWndParent,WM_CLOSE,0,0);
		return;
	}

	// Install a hook to monitor all messages going to the dialog and its children
	// and pass them on to the tooltip control
	hMsgHook = SetWindowsHookEx(WH_GETMESSAGE,GetMsgProc,NULL, GetCurrentThreadId());
}

CToolTipCtrl * CToolTipCtrl::pThis = NULL;

CToolTipCtrl::~CToolTipCtrl()
{
	UnhookWindowsHookEx(hMsgHook);
}

LRESULT CALLBACK CToolTipCtrl::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 
	if (nCode < 0) return CallNextHookEx(pThis->hMsgHook, nCode, wParam, lParam); 
	
	switch (((MSG*)lParam)->message) 
	{ 
		case WM_MOUSEMOVE:
			{
				if (IsChild(pThis->hWndParent,((MSG*)lParam)->hwnd)) 
				{
					SendMessage(pThis->hToolTip, TTM_RELAYEVENT, 0,lParam); 
				}
			}
		break; 
	default: 
		break; 
    } 
    return (CallNextHookEx(pThis->hMsgHook, nCode, wParam, lParam)); 
}

void CToolTipCtrl::AddToolForWnd(HWND hWnd,LPSTR lpszToolTip)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND;
	ti.hwnd   = hWndParent;
	ti.uId = (UINT) hWnd;
	ti.lpszText = lpszToolTip;

	BOOL B = SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti); 
}

void CToolTipCtrl::SetBkTipColor(COLORREF clr)
{
	SendMessage(hToolTip,TTM_SETTIPBKCOLOR,(WPARAM) clr,0);
}

void CToolTipCtrl::SetTipTextColor(COLORREF clr)
{
	SendMessage(hToolTip,TTM_SETTIPTEXTCOLOR,(WPARAM) clr,0);
}

void CToolTipCtrl::SetMaxTipWidth(int iWidth)
{
	SendMessage(hToolTip,TTM_SETMAXTIPWIDTH,0,(LPARAM)iWidth);
}

void CToolTipCtrl::SetFont(CONST LOGFONT *lplf)
{
	HFONT hfont = CreateFontIndirect(lplf);

	SendMessage (hToolTip, WM_SETFONT, (WPARAM)hfont, 0L);
}
