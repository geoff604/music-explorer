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

// MusicGraph.cpp: implementation of the CMusicGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "app.h"
#include "MusicGraph.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMusicGraph::CMusicGraph()
{

}

CMusicGraph::~CMusicGraph()
{

}

bool CMusicGraph::AddFrequency( double frequency, double intensity )
{
	if( frequency < 0.001 )
		return false;

	double new_frequency = 12*(log(frequency) - log(440))/log(2);		

	CLineGraph::AddDataPoint( new_frequency, intensity );

	return true;
}

void CMusicGraph::SetNoteRange( int begin, int end )
{
	int first_note = begin - 69;
	int last_note = end - 69;

	SetHorizDataRange( first_note - 0.5, last_note + 0.5 );

}