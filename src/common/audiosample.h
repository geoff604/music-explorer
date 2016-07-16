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

#ifndef AUDIO_SAMPLE_INCLUDED
#define AUDIO_SAMPLE_INCLUDED

#include <typeinfo>
#include <istream>
#include <cstddef>

using namespace std;

typedef short AudioSample; // A single audio sample
typedef unsigned char AudioByte; // an 8-bit unsigned byte

long ReadIntMsb(istream &in, int bytes);
long BytesToIntMsb(void *buff, int bytes);
long ReadIntLsb(istream &in, int bytes);
long BytesToIntLsb(void *buff, int bytes);
void SkipBytes(istream &in, int bytes);
void WriteIntMsb(ostream &out, long l, int bytes);
void WriteIntLsb(ostream &out, long l, int bytes);

#endif
