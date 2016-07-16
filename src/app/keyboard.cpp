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

// keyboard.cpp: implementation of the CKeyboard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "keyboard.h"
#include <strstream>
#include <string>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeyboard::CKeyboard()
{
	m_range_left = 21;
	m_range_right = 108;
	clickedNote = -1;
}

CKeyboard::~CKeyboard()
{

}

//=============================================================================
CKeyboard::Keys CKeyboard::MidiNoteToKey(const int nMidiNote)
{
	int remainder = nMidiNote%12; // 12 notes per octave

	switch (remainder)
	{
		case 0:
			return Keys::C;
		case 1:
			return Keys::Db;
		case 2:
			return Keys::D;
		case 3:
			return Keys::Eb;
		case 4:
			return Keys::E;
		case 5:
			return Keys::F;
		case 6:
			return Keys::Gb;
		case 7:
			return Keys::G;
		case 8:
			return Keys::Ab;
		case 9:
			return Keys::A;
		case 10:
			return Keys::Bb;
		case 11:
			return Keys::B;
		default: // not used, but needed so compiler doesn't complain -> "not all control paths return a value"
			return Keys::C;
	}
}

//=============================================================================
int CKeyboard::MidiNoteToKeyOctave(const int nMidiNote)
{
	return nMidiNote / 12;
}

//=============================================================================
string CKeyboard::EnumToName(const Keys nKey)
{
	string sNoteName;

	switch (nKey)
	{
		case C:
			sNoteName = "C";
			break;
		case Db:
			sNoteName = "Db";
			break;
		case D:
			sNoteName = "D";
			break;
		case Eb:
			sNoteName = "Eb";
			break;
		case E:
			sNoteName = "E";
			break;
		case F:
			sNoteName = "Fb";
			break;
		case Gb:
			sNoteName = "Gb";
			break;
		case G:
			sNoteName = "G";
			break;
		case Ab:
			sNoteName = "Ab";
			break;
		case A:
			sNoteName = "A";
			break;
		case Bb:
			sNoteName = "Bb";
			break;
		case B:
			sNoteName = "B";
			break;
		default: // not used, but needed so compiler doesn't complain -> "not all control paths return a value"
			sNoteName = "C";
	}

	return sNoteName;
}

//=============================================================================
bool CKeyboard::IsBlackKey(const Keys nKey)
{
	switch (nKey)
	{
		case Keys::Db:
		case Keys::Eb:
		case Keys::Gb:
		case Keys::Ab:
		case Keys::Bb:
			return true;
		default:
			return false;
	}
}

//=============================================================================
bool CKeyboard::IsWhiteKey(const Keys nKey)
{
	switch (nKey)
	{
		case Keys::C:
		case Keys::D:
		case Keys::E:
		case Keys::F:
		case Keys::G:
		case Keys::A:
		case Keys::B:
			return true;
		default:
			return false;
	}
}

//=============================================================================
void CKeyboard::Draw( CDC * pDC, int left, int top, int width, int height, CRect &musicgraph)
{
	CRect keyboardArea(left, top, left+width, top+height);
	pDC->Rectangle(keyboardArea);

	CPoint topLeftPoint = keyboardArea.TopLeft(); //retrieve top-left of Rect

	CPen blackPen(PS_SOLID, 1, RGB(0, 0, 0));

	CBrush blackBrush(RGB(0, 0, 0));
	CBrush whiteBrush(RGB(255, 255, 255));
	CBrush greenBrush(RGB(0, 255, 0));
	CBrush *pOldBrush;
	
	//count # of notes drawn per octave
	int count = MidiNoteToKey(this->m_range_left);

	int numberOfKeys = this->m_range_right - this->m_range_left + 1;
	double widthOfKey =  keyboardArea.Width() / (double)numberOfKeys;	

	bool nonWhiteKey = false;

	for (int i = this->m_range_left; i <= this->m_range_right; i++)
	{
		double left = topLeftPoint.x + (i - m_range_left)*widthOfKey;

		if (IsBlackKey((Keys)count))
		{
		//if current key is black, use a black brush
			pOldBrush = pDC->SelectObject(&blackBrush);

			nonWhiteKey = true;
		}
		else if (IsWhiteKey((Keys)count))
		{
			//pOldBrush = pDC->SelectObject(&whiteBrush);
			
			nonWhiteKey = false;
		}		

		if (nonWhiteKey)
		{
		//draw rectangle if non-white key
			CRect drawRect((int)left, topLeftPoint.y, (int)(left+widthOfKey),
				(int)(topLeftPoint.y + (keyboardArea.Height()/2.0)));
			pDC->Rectangle(drawRect);

			pDC->SelectObject(pOldBrush);
		}
		else if (!nonWhiteKey)
		//draw left-bounding line for white key
		{
			if (count == Keys::C || count == Keys::F)
			{
				pDC->MoveTo((int)left, topLeftPoint.y);
				pDC->LineTo((int)left, topLeftPoint.y + keyboardArea.Height());
			}

			else
			{
				pDC->MoveTo((int)(left - (widthOfKey/2.0)), (int)(topLeftPoint.y + keyboardArea.Height()/2.0));
				pDC->LineTo((int)(left - (widthOfKey/2.0)), topLeftPoint.y + keyboardArea.Height());
			}
		}

		if (i == clickedNote)
		{
			pOldBrush = pDC->SelectObject(&greenBrush);

			if (IsBlackKey(MidiNoteToKey(i)))
			//if clicked key is black, put dot on upper half of keyboard
			{
				pDC->Ellipse((int)(left + widthOfKey/4.0), (int)(topLeftPoint.y + keyboardArea.Height()/4.0),
						(int)(left + 3*widthOfKey/4.0),
						(int)(topLeftPoint.y + keyboardArea.Height()/2.0));
			}
			else
			//otherwise, clicked key is white, so put dot in lower half of keyboard
			{
				pDC->Ellipse((int)(left + widthOfKey/4.0), (int)(topLeftPoint.y + 3*keyboardArea.Height()/4.0),
								(int)(left + 3*widthOfKey/4.0),
								(int)(topLeftPoint.y + keyboardArea.Height()));
			}

			pDC->SelectObject(pOldBrush);
		}
		
		count++;

		if (count > Keys::B)
		{
			count = Keys::C; //reset count for next octave
		}
	}


	//pDC->SetTextColor(RGB(0 ,0 ,0));
	//pDC->TextOut(musicgraph.left, musicgraph.top, sText, strlen(sText));

	static char sText_2[4];
	static ostrstream s_2(sText_2, sizeof(sText_2));

	//output labels for Cs
	for (int j = this->m_range_left; j <= this->m_range_right; j++)
	{
		if (MidiNoteToKey(j) == Keys::C)
		{
			s_2.seekp(0); //reset to start of output string
			s_2 << EnumToName(MidiNoteToKey(j));
			s_2 << MidiNoteToKeyOctave(j);
			s_2 << ends;

			double left = topLeftPoint.x + (j - m_range_left)*widthOfKey;
			
			pDC->SetBkMode(TRANSPARENT); //set background to transparent
			pDC->SetTextColor(RGB(255, 0, 255));
			pDC->TextOut((int)(left + 2), topLeftPoint.y + 2, sText_2, strlen(sText_2));
		}
	}


}

//=============================================================================
void CKeyboard::SetNoteRange(int left, int right)
{
	this->m_range_left = left;
	this->m_range_right = right;
}

//=============================================================================
int CKeyboard::OnClickNote(int left, int top, int width, int height, int x, int y, string &clickedString)
{
	if (y < top || y > (top + height) || x < left || x > (left + width))
	//if location clicked is outside of the keyboard's bound, return -1
		return -1;
	else
	{
		int numberOfKeys = this->m_range_right - this->m_range_left + 1;
		double widthOfKey =  width / (double)numberOfKeys;

		int clickedX = x - left; //user's click in number of pixels away from
										 //the leftmost edge

		double offset = clickedX / widthOfKey;
		//offset is the number of key lengths away from the leftmost key
		//that the user clicked

		int keyboardHalfMark = top + height/2.0; //halfway mark of keyboard

		if (y <= keyboardHalfMark)
		//if user clicked in top half of keyboard, value of offset is reliable
		{
			this->clickedNote = this->m_range_left + (int)offset;
		}
		else
		{
		//otherwise, user clicked in bottom half of keyboard
			if (IsBlackKey(MidiNoteToKey(this->m_range_left + (int)offset)))
			{
			//interpolate if user clicked in an area which corresponds to a black
			//key in the upper half of the keyboard.
				int test = (int)(((int)offset)*widthOfKey + 0.5*widthOfKey);
				//halfway mark of corresponding black key

				if (clickedX < test)
				//if user's clicked location is less than halfway mark of black key,
					offset--; //decrement offset (previous white key).
				else
				//otherwise, greater than or equal to halfway mark of black key,
					offset++; //so increment offset (next white key).
			}

			this->clickedNote = this->m_range_left + (int)offset;
		}

		static char sText[30];
		static ostrstream s(sText, sizeof(sText));
		s.seekp(0); //reset to start of output string

		//format data to display in sText buffer
		//s << "Note clicked: ";
		s << clickedNote;
		s << " (";
		s << EnumToName(MidiNoteToKey(clickedNote));
		s << MidiNoteToKeyOctave(clickedNote);
		s << ")";
		s << ends;

		clickedString = sText;

		return clickedNote;
	}

}

//=============================================================================
void CKeyboard::OnUnclickNote()
{
	this->clickedNote = -1; // no note is clicked
}