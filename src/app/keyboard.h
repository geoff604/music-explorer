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

// keybord.h: interface for the CKeyboard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYBOARD_H__DDE5979A_E12E_433B_9B39_ED238BF3A498__INCLUDED_)
#define AFX_KEYBOARD_H__DDE5979A_E12E_433B_9B39_ED238BF3A498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

using namespace std;

class CKeyboard  
{
	public:
		CKeyboard();
		virtual ~CKeyboard();

		void Draw( CDC * pDC, int left, int top, int width, int height, CRect &musicgraph);

		void SetNoteRange(int left, int right);

		enum Keys {C = 1, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B};		

		int OnClickNote(int left, int top, int width, int height, int x, int y, string &clickedString);
		void OnUnclickNote();

	private:
		Keys MidiNoteToKey(const int nMidiNote);
		string EnumToName(const Keys nKey);
		int MidiNoteToKeyOctave(const int nMidiNote);
		bool IsWhiteKey(const Keys nKey);
		bool IsBlackKey(const Keys nKey);
		
		int m_range_left;
		int m_range_right;
		int clickedNote;
};

#endif // !defined(AFX_KEYBOARD_H__DDE5979A_E12E_433B_9B39_ED238BF3A498__INCLUDED_)
