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

// WaveformGraph.h: interface for the CWaveformGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEFORMGRAPH_H__9CB8147A_075D_4F06_AF17_CF3384C27043__INCLUDED_)
#define AFX_WAVEFORMGRAPH_H__9CB8147A_075D_4F06_AF17_CF3384C27043__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AudioTime.h"
#include <vector>

using namespace std;

struct WavePulse {
	CAudioTime time;
	short magnitude;

	WavePulse(CAudioTime &new_time, int new_magnitude);
};

class CWaveformGraph  
{
public:
	CWaveformGraph();
	virtual ~CWaveformGraph();

	void SetHorizRange(CAudioTime &left, CAudioTime &right);
	void SetVertExtent(int vert);
	void ScrollRight(CAudioTime &amount);
	void ScrollLeft(CAudioTime &amount);

	void AddPulse(WavePulse &pulse);

	void Draw(CDC * pDC, int x, int y, int width, int height);

	void SelectionStart(int x, int y, int width, int height, CPoint &point);
	void SelectionEnd(int x, int y, int width, int height, CPoint &point);
	void SetSelectionRange(CAudioTime &startTime, CAudioTime &endTime);
	void ClearSelection();

	short GetVertExtent();

	CAudioTime GetSelectionStart();
	CAudioTime GetSelectionEnd();

	bool SelectionIsOpen();

	void ClearData();

private:
	vector<WavePulse> m_Pulses;

	CAudioTime m_LeftTime;
	CAudioTime m_RightTime;

	short m_Vert;

	CAudioTime m_SelectionStartTime;
	CPoint m_SelectionStartPt;
	
	CAudioTime m_SelectionEndTime;
	CPoint m_SelectionEndPt;

	bool m_bSelectionOpen;

	CPoint GetPixelCoord(WavePulse &pulseData, int x, int y, int width, int height);
	void CheckSelection();
	CAudioTime PixelCoordToTime(int x, int y, int width, int height, CPoint &point);
};

#endif // !defined(AFX_WAVEFORMGRAPH_H__9CB8147A_075D_4F06_AF17_CF3384C27043__INCLUDED_)
