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

// SoundException.h: interface for the CSoundException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDEXCEPTION_H__3635E9B6_95F7_4C7E_AAF1_C8B8230965C2__INCLUDED_)
#define AFX_SOUNDEXCEPTION_H__3635E9B6_95F7_4C7E_AAF1_C8B8230965C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>


const int SOUND_EXCEPTION_STRING_RESOURCE = 1000;

class CSoundException : public CException  
{
	DECLARE_DYNAMIC(CSoundException);

public:
	CSoundException();
	CSoundException(int iCause);
	virtual ~CSoundException();
protected:
	int m_cause;
public:
	enum
	{
		causeUnknown = 0,
		causeNegotiateSampling,
		causeChangeSampling,
		causeChangeNumberOfChannels,
		causeMinMaxSamplingRateUndefined,
		causeFFTLengthMustBePowerOfTwo,
		causeSynchronizationError,
		causeUnterminatedVariableInteger,
		causeFirstChunkMustBeMthd,
		causeConnectToAudioServer,
		causeNoAudioOutputDevice,
		causeCreateAudioFlowOnServer,
		causeConfigureFlow,
		causeRegisterEventHandler,
		causeChunkContainedInNonContainer,
		causeChunkTooLargeForContainer,
		causeNoFmtChunkFound,
		causeADPCMRequires4BitsPerSample,
		causeADPCMRequiresDecompressionData,
		causeUnsupportedWAVCompression,
		causeOutermostChunkInWAVNotRIFF,
		causeInputFileNotAU,
		causeAUFormatNotSupported,
		causeNegotiateChannels,
		causeOpenWAVOutputDevice
	};

public:
	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL);

};

#endif // !defined(AFX_SOUNDEXCEPTION_H__3635E9B6_95F7_4C7E_AAF1_C8B8230965C2__INCLUDED_)
