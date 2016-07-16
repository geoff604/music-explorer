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

// SinePlayer.cpp: implementation of the CSinePlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "app.h"
#include "SinePlayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSinePlayer::CSinePlayer()
{

	const int sampling_rate = 44100;

	_frequency = 440;
	
	_sinewave = new SineWave();
	_sinewave->SamplingRate( sampling_rate );

	_player = new WinPlayer( _sinewave );
	
}

CSinePlayer::~CSinePlayer()
{
	delete _player;
	delete _sinewave;

}

void CSinePlayer::Play( )
{

	_play_lock.Lock();
	_sinewave->Frequency( _frequency );
	_player->Play();
	_play_lock.Unlock();
	

}

void CSinePlayer::Stop()
{
	_player->Stop();

}

void CSinePlayer::Frequency( double freq )
{
	_frequency = freq;
}