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

// LineGraph.cpp: implementation of the CLineGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "app.h"
#include "LineGraph.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineGraph::CLineGraph()
{
	m_range_left = 0;
	m_range_right = 100;
}

CLineGraph::~CLineGraph()
{

}

void CLineGraph::SetHorizDataRange(double left, double right)
{
	ASSERT( left <= right );

	m_range_left = left;
	m_range_right = right;
}


void CLineGraph::AddDataPoint(double x, double y)
{
	m_points.push_back( CDoublePoint( x, y ) );
}

void CLineGraph::ClearData()
{
	m_points.clear();
}

void CLineGraph::Draw(CDC *pDC, int x, int y, int width, int height, bool vert_log)
{

	// calculate vertical extent	
	vector<CDoublePoint>::iterator iter;
	double range_top = 0;
	for( iter = m_points.begin(); iter != m_points.end(); iter++ )
	{
		if( m_range_left <= iter->x && iter->x <= m_range_right 
			&& iter->y > range_top )
		{
			range_top = iter->y;
		}		
	}	

	CPen* oldpen;
	CPen pen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
	oldpen = pDC->SelectObject(  &pen );

	CPoint lastpoint;
	lastpoint.x = 0;
	lastpoint.y = y + height;

	double new_range_top;
	if( vert_log )
	{
		new_range_top = 1;
	}
	else
	{
		new_range_top = range_top;
	}

	// draw each point
	for( iter = m_points.begin(); iter != m_points.end(); iter++ )
	{
		// if in range
		if( m_range_left <= iter->x && iter->x <= m_range_right )
		{
			CDoublePoint datapoint = *iter;
			
			if( vert_log ) // if we should use a logarithmic scaling for the verical axis
			{
				int sign = 1;
				if( datapoint.y < 0 )
					sign = -1;
				int u = 255;
				double tempval = datapoint.y / range_top;
				if( tempval < 0 )
					tempval = -tempval;

				datapoint.y = (sign / log(1+u))*log( 1 + u*tempval );				
			}
			
			CPoint newpoint = GetPixelCoord( datapoint, x, y, width, height, new_range_top );
			pDC->MoveTo( lastpoint.x, lastpoint.y );
			pDC->LineTo( newpoint.x, newpoint.y );
			lastpoint = newpoint;
		}		
	}

	// reselect the old pen
	pDC->SelectObject( oldpen );

}

CPoint CLineGraph::GetPixelCoord( CDoublePoint datapoint, int x, int y, int width, int height, double range_top )
{
	CPoint result;
	double fraction = (datapoint.x - m_range_left) / (m_range_right - m_range_left );
	result.x = (int)(fraction * width + x);

	fraction = datapoint.y / range_top;
	result.y = y + (height - (int)(fraction * height));

	return result;

}