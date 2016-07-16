/////////////////////////////////////////////////////////////////////////////////////////////////
//				Class		: CStaticHyperLink													   //
//										 													   //
//				Author		: Natarajan T													   //
//																							   //
//				Version		: 1.1															   //
//																							   //
//				Purpose		: C++ Class for creating a Text HyperLink.						   //
//																							   //
//				Disclaimer	: This Source code is given in asis condition and nothimg is 	   //
//								guaranted, The Source code is to be used on own risk.		   //
//								This Source code is distributed in good faith and Author is not//
//								responsible for any damage done, to the user, in any manner    //
//								perceived to be caused by the use of this source code.		   //
//																							   //
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATICHYPERLINK_H__CDDDC540_9FC3_11D5_AA56_AF80EE1F3B3B__INCLUDED_)
#define AFX_STATICHYPERLINK_H__CDDDC540_9FC3_11D5_AA56_AF80EE1F3B3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <commctrl.h>

typedef struct HyperLinkData
{
	LPCSTR	 lpUrl;				// URL
	LPCSTR	 lpDisplayTxt;		// URL Description
	COLORREF cLnkColor;			// Un Visited Link Color
	COLORREF cVisitedLnkColor;	// Visited Link Color
	COLORREF cHoverLnkColor;	// Mouse Over Link Color
	LPCSTR	 lpDisplayFont;		// Font
	int		 iFontSize;			// Font Size
	int		 iFontWeight;		// Font Weight
	DWORD	 dwFontFlags;		// Font Flags
	int      cx;				// X Co-ordinate
	int      cy;				// Y Co-ordinate

} HYPLNKDATA, *PHYPLNKDATA;

#include <mapi.h> //For Simple MAPI API's

#define FONT_REGULAR			0x0000
#define FONT_ITALIC				0x0001
#define FONT_UNDERLINE			0x0002
#define FONT_STRIKEOUT			0x0004

#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER			0x02A1
#define WM_MOUSELEAVE			0x02A3
#endif

#define OSVER_UNKNOWN			0x00000000
#define OSVER_WIN3X				0x00000001
#define OSVER_WIN95				0x00000002
#define OSVER_WIN98				0x00000004
#define OSVER_WINME				0x00000008
#define OSVER_WINNT				0x00000010
	
class CStaticHyperLink  
{
public:
	HWND hHyperLinkWnd;
	CStaticHyperLink(HWND hWndParent,HYPLNKDATA HypLinkData);
	virtual ~CStaticHyperLink();

private:
	HMODULE hWinHlp32Module;
	DWORD GetOSVer();
	HFONT hfont;
	bool bVisited;
	bool bMouseHover;
	HWND hWndParent;
	HYPLNKDATA HypLinkData;
	bool MailTo();

	static LRESULT CALLBACK HyperLinkWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif // !defined(AFX_STATICHYPERLINK_H__CDDDC540_9FC3_11D5_AA56_AF80EE1F3B3B__INCLUDED_)
