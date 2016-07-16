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

// MusicGraph.h: interface for the CMusicGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUSICGRAPH_H__E84E9CE9_AAE7_44FC_A5AB_E260A48F4595__INCLUDED_)
#define AFX_MUSICGRAPH_H__E84E9CE9_AAE7_44FC_A5AB_E260A48F4595__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineGraph.h"

class CMusicGraph : public CLineGraph  
{
public:
	CMusicGraph();
	virtual ~CMusicGraph();

	bool AddFrequency( double frequency, double intensity );
	// frequencies must be added from least to greatest
	// returns true if success
	
	void SetNoteRange( int begin, int end );
	// specifies the midi notes to display on the graph
	// where 69 = A5 = A440,
	//       21 = Low A, 
	//      108 = High C
};

#endif // !defined(AFX_MUSICGRAPH_H__E84E9CE9_AAE7_44FC_A5AB_E260A48F4595__INCLUDED_)
