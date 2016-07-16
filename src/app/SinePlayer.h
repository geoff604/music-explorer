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

// SinePlayer.h: interface for the CSinePlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINEPLAYER_H__07D2A545_E7C7_41EA_B8F1_F086267A86BC__INCLUDED_)
#define AFX_SINEPLAYER_H__07D2A545_E7C7_41EA_B8F1_F086267A86BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "winplayr.h"
#include "audio.h"
#include "sinewave.h"

#include <afxmt.h>

class CSinePlayer  
{
public:
	CSinePlayer();
	virtual ~CSinePlayer();

	void Play();
	// only one thread should use Play at a time!
	// If two threads attempt to use it at once, then
	// the second one will have to wait a second
	// while the first one is kicked out.

	void Stop();
	// may be called by multiple threads

	void Frequency( double freq );

private:
	CCriticalSection _play_lock;
	WinPlayer* _player;
	SineWave* _sinewave;
	double _frequency;
};

#endif // !defined(AFX_SINEPLAYER_H__07D2A545_E7C7_41EA_B8F1_F086267A86BC__INCLUDED_)
