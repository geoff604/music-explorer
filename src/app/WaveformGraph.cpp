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

// WaveformGraph.cpp: implementation of the CWaveformGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "app.h"
#include "WaveformGraph.h"
#include <limits.h>
#include <strstream>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WavePulse::WavePulse(CAudioTime &new_time, int new_magnitude)
{
	this->time = new_time;
	this->magnitude = new_magnitude;
}

//=============================================================================
CWaveformGraph::CWaveformGraph()
{
	this->m_Vert = SHRT_MAX;
	this->m_bSelectionOpen = false;
}

//=============================================================================
CWaveformGraph::~CWaveformGraph()
{

}

//=============================================================================
void CWaveformGraph::SetHorizRange(CAudioTime &left, CAudioTime &right) {
	this->m_LeftTime = left;
	this->m_RightTime = right;
}

//=============================================================================
void CWaveformGraph::SetVertExtent(int vert) {
	if( vert > 0 && vert < SHRT_MAX )
		this->m_Vert = vert;
}

//=============================================================================
short CWaveformGraph::GetVertExtent()
{
	return m_Vert;
}

//=============================================================================
void CWaveformGraph::ScrollLeft(CAudioTime &amount) {
	this->m_LeftTime = this->m_LeftTime - amount;
	this->m_RightTime = this->m_RightTime - amount;
}

//=============================================================================
void CWaveformGraph::ScrollRight(CAudioTime &amount) {
	this->m_LeftTime = this->m_LeftTime + amount;
	this->m_RightTime = this->m_RightTime + amount;
}

//=============================================================================
void CWaveformGraph::AddPulse(WavePulse &pulse) {
	this->m_Pulses.push_back(pulse);
}

//=============================================================================
void CWaveformGraph::Draw(CDC *pDC, int x, int y, int width, int height) {
	CPen *oldPen;
	CPen bluePen( PS_SOLID, 1, RGB( 0, 0, 255 ) );
	oldPen = pDC->SelectObject(&bluePen);

	double zero = y + (height/2.0); //halfway point of waveform area acts as zero
	
	//draw horizontal axis
	pDC->MoveTo(x, (int)zero);
	pDC->LineTo(x + width, (int)zero);

	//if (this->m_bDrawWaveform) {
	vector<WavePulse>::iterator iter;

	CAudioTime iterTime;
	int iterMag;

	for (iter = this->m_Pulses.begin(); iter != this->m_Pulses.end(); iter++) {
	//find the first pulse stored which occurs at at least the same time as
	//m_LeftTime and is less than m_RightTime
		if (iter->time >= this->m_LeftTime && iter->time < this->m_RightTime) {
			iterTime = iter->time;
			iterMag = iter->magnitude;
			break; //leave iter at firstTime
		}
	}	

	CBrush brush(RGB(0, 0, 255));
	CBrush *oldBrush;

	oldBrush = pDC->SelectObject(&brush);	

	while(iter != this->m_Pulses.end() && iter->time <= this->m_RightTime)
	//draw each pulse within the horizontal range
	{
		CPoint pulsePt = this->GetPixelCoord(*iter, x, y, width, height);

		
		int drawWidth;

		if ((iter+1) != this->m_Pulses.end()) {
		//if a next pulse exists in the vector, use it to find the width of
		//the pulse rectangle to draw.
			CPoint nextPulsePt = this->GetPixelCoord(*(iter+1), x, y, width, height);
			drawWidth = nextPulsePt.x;
		}
		else {
		//otherwise, width is the next subframe
			CAudioTime nextFrameTime = CAudioTime(iter->time.hours,
										iter->time.minutes, iter->time.seconds,
										iter->time.frames, iter->time.subframes + 1);
			int mag = iter->magnitude;

			CPoint nextFrame = this->GetPixelCoord(WavePulse(nextFrameTime, mag),
									x, y, width, height);
			drawWidth = nextFrame.x;
		}

		CRect pulseRect;
		pulseRect = CRect(pulsePt.x, pulsePt.y, drawWidth, (zero - pulsePt.y) + zero);
		pDC->Rectangle(&pulseRect);

		iter++;
	}

	pDC->SelectObject(oldBrush);

	if (this->m_bSelectionOpen) {
		CPen startPen(PS_DASHDOTDOT, 1, RGB(255, 0, 0));
		oldPen = pDC->SelectObject(&startPen);

		this->m_SelectionStartPt = this->GetPixelCoord(WavePulse(this->m_SelectionStartTime, 0), x, y, width, height);
		//update selection start point is needed in case scrolling is done.

		pDC->MoveTo(this->m_SelectionStartPt.x, y);
		pDC->LineTo(this->m_SelectionStartPt.x, y + height);
	}
	else {
		CPen endPen(PS_DASHDOTDOT, 1, RGB(0, 0, 0));
		oldPen = pDC->SelectObject(&endPen);

		this->m_SelectionStartPt = this->GetPixelCoord(WavePulse(this->m_SelectionStartTime, 0), x, y, width, height);
		//update selection start point is needed in case select range dialog used

		this->m_SelectionEndPt = this->GetPixelCoord(WavePulse(this->m_SelectionEndTime, 0), x, y, width, height);
		//update selection end point is needed in case scrolling is done.

		CRect selectedRect(this->m_SelectionStartPt.x, y, this->m_SelectionEndPt.x, y + height);

		pDC->InvertRect(selectedRect);
	}

	static char displayed[200];
	static ostrstream oStrStrm(displayed, sizeof(displayed));

	oStrStrm.seekp(0);

	oStrStrm << "Time Displayed: " << this->m_LeftTime << " to " << this->m_RightTime
		<< ends;

	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen = pDC->SelectObject(&blackPen);

	pDC->TextOut(x, y + 5, displayed, strlen(displayed));

	oStrStrm.seekp(0);
	oStrStrm << "Selected Range: " << this->m_SelectionStartTime << " to "
		<< this->m_SelectionEndTime	<< ends;

	pDC->TextOut(x, y + 25, displayed, strlen(displayed));

	// reselect the old pen
	pDC->SelectObject(oldPen);
}

//=============================================================================
CPoint CWaveformGraph::GetPixelCoord(WavePulse &pulseData, int x, int y, int width, int height) {
	CPoint result;

	CAudioTime distFromLeft = pulseData.time - this->m_LeftTime;
	CAudioTime range = this->m_RightTime - this->m_LeftTime;

	long distInSubframes = distFromLeft.GetAllInSubframes();
	long rangeInSubframes = range.GetAllInSubframes();

	double fraction = distInSubframes / (double)rangeInSubframes;
	result.x = (int)(fraction*width + x);

	int mag = pulseData.magnitude;

	// if the magnitude exceeds the vertical range, then clip the magnitude
	// to the range
	if (mag != 0 && this->m_Vert != 0 ) {
		int sign;
		if( mag < 0 )
			sign = -1;
		else
			sign = 1;

		if( abs(mag) > m_Vert )
			mag = sign*m_Vert;		

		fraction = mag / (double)this->m_Vert;
	}
	else {
		fraction = 0.f;
	}

	result.y = (int)(y + (height/2.0) - (int)(fraction*(height/2.0)));

	return result;
}

//=============================================================================
void CWaveformGraph::SelectionStart(int x, int y, int width, int height, CPoint &point) {
	this->m_SelectionStartTime = this->PixelCoordToTime(x, y, width, height, point);
	this->m_SelectionStartPt = this->GetPixelCoord(WavePulse(this->m_SelectionStartTime, 0), x, y, width, height);

	this->m_bSelectionOpen = true;
}

//=============================================================================
void CWaveformGraph::SelectionEnd(int x, int y, int width, int height, CPoint &point) {
	this->m_SelectionEndTime = this->PixelCoordToTime(x, y, width, height, point);
	this->m_SelectionEndPt = this->GetPixelCoord(WavePulse(this->m_SelectionEndTime, 0), x, y, width, height);

	this->CheckSelection();

	this->m_bSelectionOpen = false;
}



//=============================================================================
void CWaveformGraph::CheckSelection() {
	if (this->m_SelectionEndPt.x < this->m_SelectionStartPt.x) {
	//if the selection end is less than the selection start, swap them.
		CPoint temp = this->m_SelectionEndPt;
		this->m_SelectionEndPt = this->m_SelectionStartPt;
		this->m_SelectionStartPt = temp;

		CAudioTime tempTime = this->m_SelectionEndTime;		
		this->m_SelectionEndTime = this->m_SelectionStartTime;
		this->m_SelectionStartTime = tempTime;
	}
}

//=============================================================================
CAudioTime CWaveformGraph::PixelCoordToTime(int x, int y, int width, int height, CPoint &point) {
	CAudioTime resultTime;

	double distFromLeft = point.x - x;
	double fraction = distFromLeft / width;

	long timeRange = this->m_RightTime.GetAllInSubframes() - this->m_LeftTime.GetAllInSubframes();

	double tempTime = timeRange * fraction;

	resultTime.SetFromSubframes(tempTime); //now resultTime is the time difference from left

	resultTime = resultTime + this->m_LeftTime;

	return resultTime;
}

//=============================================================================
void CWaveformGraph::ClearData() {
	this->m_Pulses.clear();
}

//=============================================================================
CAudioTime CWaveformGraph::GetSelectionStart() {
	return this->m_SelectionStartTime;
}

//=============================================================================
CAudioTime CWaveformGraph::GetSelectionEnd() {
	return this->m_SelectionEndTime;
}

//=============================================================================
bool CWaveformGraph::SelectionIsOpen() {
	return this->m_bSelectionOpen;
}

//=============================================================================
void CWaveformGraph::SetSelectionRange(CAudioTime &startTime, CAudioTime &endTime) {
	this->m_SelectionStartTime = startTime;
	this->m_SelectionEndTime = endTime;
}

//=============================================================================
void CWaveformGraph::ClearSelection() {
	this->m_SelectionStartTime = CAudioTime(0, 0, 0, 0, 0);
	this->m_SelectionEndTime = CAudioTime(0, 0, 0, 0, 0);
}