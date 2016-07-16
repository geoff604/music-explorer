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

// SoundException.cpp: implementation of the CSoundException class.
//
//////////////////////////////////////////////////////////////////////

#include "SoundException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundException::CSoundException()
{

}

CSoundException::~CSoundException()
{

}

CSoundException::CSoundException(int iCause)
{
	m_cause = iCause;
}

IMPLEMENT_DYNAMIC( CSoundException, CException );

BOOL CSoundException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
	if( pnHelpContext != NULL )
		*pnHelpContext = m_cause + SOUND_EXCEPTION_STRING_RESOURCE;

	CString strMessage;
	strMessage.LoadString( m_cause + SOUND_EXCEPTION_STRING_RESOURCE );
	lstrcpyn( lpszError, strMessage, nMaxError);

	return TRUE;

}