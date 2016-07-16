/////////////////////////////////////////////////////////////////////////////////////////////////
//				Class		: CStaticHyperLink													   //
//										 													   //
//				Author		: Natarajan T													   //
//																							   //
//				Version		: 1.2															   //
//																							   //
//				Purpose		: C++ Class for creating a Text HyperLink.						   //
//																							   //
//				Disclaimer	: This Source code is given in asis condition and nothimg is 	   //
//								guaranted, The Source code is to be used on own risk.		   //
//								This Source code is distributed in good faith and Author is not//
//								responsible for any damage done, to the user, in any manner    //
//								perceived to be caused by the use of this source code.		   //
//																							   //
//---------------------------------------------------------------------------------------------//
//	Maintenance Log:																		   //
//	----------------																		   //
//	Version	1.0 : 02 Oct 01 : Initial release.												   //
//	Version 1.1 : 25 Oct 01 : Updated, minor Fix											   //
//	Version 1.2 : 09 Mar 02 : Added MailTo() to handle "mailto:" thru Simple MAPI.			   //
//																							   //
//																							   //
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"


#include "StaticHyperLink.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticHyperLink::CStaticHyperLink(HWND hWndParent,HYPLNKDATA HypLinkData)
{
	if (hWndParent == NULL)
		return;
	//Valid Handle to Parent Required.
	
	if (!HypLinkData.lpUrl)
		return; 
	// URL cant be blank !
	
	CStaticHyperLink::HypLinkData.lpUrl = HypLinkData.lpUrl;
	CStaticHyperLink::hWinHlp32Module   = NULL;
	
	if (HypLinkData.lpDisplayTxt == NULL)
		CStaticHyperLink::HypLinkData.lpDisplayTxt = HypLinkData.lpUrl;
	else
		CStaticHyperLink::HypLinkData.lpDisplayTxt = HypLinkData.lpDisplayTxt;
	
	CStaticHyperLink::hWndParent = hWndParent;
	
	if (HypLinkData.cLnkColor == NULL)
		CStaticHyperLink::HypLinkData.cLnkColor    = RGB(0,0,238);	//Blue
	else
		CStaticHyperLink::HypLinkData.cLnkColor    = HypLinkData.cLnkColor;
	
	if (HypLinkData.cHoverLnkColor == NULL)
		CStaticHyperLink::HypLinkData.cHoverLnkColor   = RGB(255,0,0);	//Red
	else 
		CStaticHyperLink::HypLinkData.cHoverLnkColor   = HypLinkData.cHoverLnkColor;
	
	
	if (HypLinkData.cVisitedLnkColor == NULL)
		CStaticHyperLink::HypLinkData.cVisitedLnkColor = RGB(85,26,139);//Purple
	else
		CStaticHyperLink::HypLinkData.cVisitedLnkColor = HypLinkData.cVisitedLnkColor;
	
	if (HypLinkData.lpDisplayFont == NULL)
	{
		CStaticHyperLink::HypLinkData.lpDisplayFont = TEXT("MS Sans Serif");
		CStaticHyperLink::HypLinkData.iFontSize     = 8;
		CStaticHyperLink::HypLinkData.iFontWeight   = FW_NORMAL;
		CStaticHyperLink::HypLinkData.dwFontFlags   = FONT_REGULAR;
	}
	else
	{
		CStaticHyperLink::HypLinkData.lpDisplayFont = HypLinkData.lpDisplayFont;
		
		if (HypLinkData.iFontSize > 0)
			CStaticHyperLink::HypLinkData.iFontSize     = HypLinkData.iFontSize;
		else
			CStaticHyperLink::HypLinkData.iFontSize     = 8;
	}
	
	if ((HypLinkData.iFontWeight > 0) && (HypLinkData.iFontWeight <= 900))
		CStaticHyperLink::HypLinkData.iFontWeight = HypLinkData.iFontWeight;
	else
	{
		CStaticHyperLink::HypLinkData.iFontWeight = FW_NORMAL;
		HypLinkData.dwFontFlags = FONT_REGULAR;
	}
	
	
	CStaticHyperLink::bMouseHover = false;
	CStaticHyperLink::bVisited = false;
	
	CStaticHyperLink::hfont = CreateFont((-CStaticHyperLink::HypLinkData.iFontSize)*GetDeviceCaps(GetDC(GetDesktopWindow()), 
								LOGPIXELSY)/72, 
								0, 0, 0, 
								CStaticHyperLink::HypLinkData.iFontWeight,
								((HypLinkData.dwFontFlags & FONT_ITALIC)== FONT_ITALIC), //Italic
								((HypLinkData.dwFontFlags & FONT_UNDERLINE)== FONT_UNDERLINE), //UnderLine
								((HypLinkData.dwFontFlags & FONT_STRIKEOUT) == FONT_STRIKEOUT), //Strikeout
								ANSI_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, 
								PROOF_QUALITY, 
								VARIABLE_PITCH | FF_SWISS, 
								CStaticHyperLink::HypLinkData.lpDisplayFont);
	
	WNDCLASSEX			wcex;
	DWORD				dwExStyle;
	DWORD				dwStyle;
	SIZE				size;
	
	{
		HDC hdc = GetDC(GetDesktopWindow());
		SelectObject(hdc,hfont);
		GetTextExtentPoint32(hdc,CStaticHyperLink::HypLinkData.lpDisplayTxt,strlen(CStaticHyperLink::HypLinkData.lpDisplayTxt),&size);
		ReleaseDC(GetDesktopWindow(),hdc);
	}
	
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)HyperLinkWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon			= NULL;
    {
		if (GetOSVer() == OSVER_WIN95)
		{
			// Get the windows directory
			char szWinDir[MAX_PATH];
			GetWindowsDirectory(szWinDir, MAX_PATH);
			
			strcat(szWinDir, TEXT("\\winhlp32.exe"));
			// This retrieves cursor #106 from winhlp32.exe, which is a Hand Cursor !
			hWinHlp32Module = LoadLibrary(szWinDir);
			if (hWinHlp32Module) 
			{
				wcex.hCursor		= LoadCursor(hWinHlp32Module, MAKEINTRESOURCE(106));
			}
			
		}
		else
		{
			wcex.hCursor		= LoadCursor(NULL, MAKEINTRESOURCE(32649)); // Hand Cursor !
		}
    }
	wcex.hbrBackground	= (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "HyperLinkWnd";
	wcex.hIconSm		= NULL;
	
	RegisterClassEx(&wcex);
	
	dwExStyle			= 0l;
	dwStyle				= WS_CHILD;
	
	hHyperLinkWnd			= CreateWindowEx(dwExStyle,
		wcex.lpszClassName,
		"",
		dwStyle,
		HypLinkData.cx,HypLinkData.cy,
		size.cx,size.cy,
		hWndParent,
		NULL,
		wcex.hInstance,
		(LPVOID)this);
	
	if (hHyperLinkWnd == NULL)
	{
		MessageBox(hWndParent,"Error Creating HyperLink Window","",MB_OK);
		return;
	}
	
	ShowWindow(hHyperLinkWnd,SW_NORMAL);
	UpdateWindow(hHyperLinkWnd);
}

CStaticHyperLink::~CStaticHyperLink()
{
	if (hWinHlp32Module) FreeLibrary(hWinHlp32Module);
	DeleteObject(hfont);
}

LRESULT CALLBACK CStaticHyperLink::HyperLinkWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CStaticHyperLink * pThis;
	switch (uMsg)
	{
	case WM_CREATE:
		{
			SetWindowLong(hWnd,GWL_USERDATA,(LONG)((LPCREATESTRUCT) lParam)->lpCreateParams);
		}
		return 0;
	case WM_PAINT :
		{
			pThis = (CStaticHyperLink *) GetWindowLong(hWnd,GWL_USERDATA);
			PAINTSTRUCT ps;
			RECT rect;
			HDC hdc = BeginPaint (hWnd, &ps) ;
			
			GetClientRect (hWnd, &rect) ;
			SetBkMode(hdc,TRANSPARENT);
			if (pThis->bMouseHover)
				SetTextColor(hdc,(pThis->HypLinkData.cHoverLnkColor));
			else if (pThis->bVisited)
				SetTextColor(hdc,(pThis->HypLinkData.cVisitedLnkColor));
			else
				SetTextColor(hdc,(pThis->HypLinkData.cLnkColor));
			
			SelectObject(hdc,pThis->hfont);
			DrawText (hdc, pThis->HypLinkData.lpDisplayTxt, -1, &rect,DT_CENTER | DT_VCENTER) ;
			EndPaint (hWnd, &ps) ;
		}
		return 0 ;
	case WM_MOUSEHOVER:
		{
			InvalidateRect(hWnd,NULL,TRUE);
		}
		return 0;
	case WM_MOUSELEAVE:
		{
			InvalidateRect(hWnd,NULL,TRUE);
		}
		return 0;
	case WM_MOUSEMOVE:
		{
			pThis = (CStaticHyperLink *) GetWindowLong(hWnd,GWL_USERDATA);
			if(!pThis->bMouseHover)
			{
				pThis->bMouseHover = true;
				SetTimer(hWnd,1000,100,NULL);
				SendMessage(hWnd,WM_MOUSEHOVER,0,0);
			}
		}
		return 0;
	case WM_TIMER:
		{
			pThis = (CStaticHyperLink *) GetWindowLong(hWnd,GWL_USERDATA);
			RECT rt;
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd,&pt);
			GetClientRect(hWnd,&rt);
			if (!PtInRect(&rt,pt))
			{
				pThis->bMouseHover = false;
				KillTimer(hWnd,1000);
				SendMessage(hWnd,WM_MOUSELEAVE,0,0);
			}
		}
		return 0;
	case WM_LBUTTONUP:
		{
			pThis = (CStaticHyperLink *) GetWindowLong(hWnd,GWL_USERDATA);
			pThis->bVisited = true;
			
			if (strnicmp(pThis->HypLinkData.lpUrl,"mailto:",7))
			{
				HINSTANCE hInst;
				hInst = ShellExecute(NULL,
					"open",
					pThis->HypLinkData.lpUrl,
					NULL,
					NULL,
					SW_SHOW);
				
				if ((UINT) hInst < 32)
				{
					MessageBox(pThis->hWndParent,"Error Opening the Browser",
						"Error",MB_OK | MB_ICONSTOP);
				}
				InvalidateRect(hWnd,NULL,TRUE);
			}
			else
			{
				pThis->MailTo();
			}
		}
		return 0;
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

DWORD CStaticHyperLink::GetOSVer()
{
	OSVERSIONINFO OSVerInfo;
	
	OSVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OSVerInfo);
	
	switch (OSVerInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		return OSVER_WIN3X;
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			if (OSVerInfo.dwMajorVersion == 4) 
			{
				if (OSVerInfo.dwMinorVersion  >= 90)
					return OSVER_WINME;
				else 
					if (OSVerInfo.dwMinorVersion  > 0)
						return OSVER_WIN98;
					else
						return OSVER_WIN95;
			}
			return OSVER_WINME;
		}
	case VER_PLATFORM_WIN32_NT:
		return OSVER_WINNT;
	default: 
		return OSVER_UNKNOWN;
	}
}

bool CStaticHyperLink::MailTo()
{
	// MAPI Funtions are not exported, hence one cannot do "Static" Link.
	// so "Dynamically" load the MAPI32 dll, map the address of the API
	// & use it.

	// Load MAPI library.
	HINSTANCE hMapilib = LoadLibrary("MAPI32.DLL");

	// Map the address of MAPISendMail API.
	LPMAPISENDMAIL lpfnMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(hMapilib, "MAPISendMail");

	// Create the Mail Message
	MapiMessage MailMsg;
	MapiRecipDesc MailSendTo;
	
	MailSendTo.ulReserved		= 0;
	MailSendTo.ulRecipClass		= MAPI_TO;
	MailSendTo.lpszName			= (LPTSTR)HypLinkData.lpDisplayTxt;
	MailSendTo.lpszAddress		= (LPTSTR)HypLinkData.lpUrl;
	MailSendTo.ulEIDSize		= 0;
	MailSendTo.lpEntryID		= NULL;
	
	MailMsg.ulReserved			= 0;			//Always 0
	MailMsg.lpszSubject			= NULL;			//No Subject
	MailMsg.lpszNoteText		= NULL;			//No Message Text
	MailMsg.lpszMessageType		= NULL;
	MailMsg.lpszDateReceived	= NULL;
	MailMsg.lpszConversationID	= NULL;
	MailMsg.flFlags				= 0l;
	MailMsg.lpOriginator		= NULL;			//Mail Sender's Info
	MailMsg.nRecipCount			= 1;			//mail to be sent to # of people
	MailMsg.lpRecips			= &MailSendTo;	//Details of recipiant.
	MailMsg.nFileCount			= 0;			//No attachments
	MailMsg.lpFiles				= NULL;			//Details of Attachments.

	// Send mail.
	ULONG Error = lpfnMAPISendMail(0,(ULONG)hWndParent, &MailMsg, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	// Unload MAPI lib.
	FreeLibrary(hMapilib);

	if (Error != SUCCESS_SUCCESS)
	{
		MessageBox(hWndParent,"MAPI Failed","Error",MB_OK);
		return false;
	}
	return true;
}
