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

// LineGraph.h: interface for the CLineGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEGRAPH_H__5260F9F3_9EBA_4C01_A17D_9ADFAB6BE1B2__INCLUDED_)
#define AFX_LINEGRAPH_H__5260F9F3_9EBA_4C01_A17D_9ADFAB6BE1B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

#include <afxwin.h>

struct CDoublePoint
{
	double x;
	double y;
	CDoublePoint() { x=0; y=0; }
	CDoublePoint( double nx, double ny ) : x( nx ), y( ny ) {}
};

class CLineGraph  
{
public:	
	
	virtual void Draw( CDC * pDC, int x, int y, int width, int height, bool vert_log = false );
	void ClearData();
	void AddDataPoint(double x, double y);
	// data points must be added in order

	CLineGraph();
	virtual ~CLineGraph();

	void SetHorizDataRange(double left, double right);

private:
	double m_range_left;
	double m_range_right;	

	vector<CDoublePoint> m_points;

	CPoint GetPixelCoord( CDoublePoint datapoint, int x, int y, int width, int height, double range_top );
};

#endif // !defined(AFX_LINEGRAPH_H__5260F9F3_9EBA_4C01_A17D_9ADFAB6BE1B2__INCLUDED_)
