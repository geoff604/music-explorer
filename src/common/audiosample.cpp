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

#include "audiosample.h"

long ReadIntMsb(istream &in, int size) {
   if (size <= 0) return 0;
   long l = ReadIntMsb(in,size-1) << 8;
   l |= static_cast<long>(in.get()) & 255;
   return l;
}

long BytesToIntMsb(void *vBuff, int size) {
   unsigned char *buff = reinterpret_cast<unsigned char *>(vBuff);
   if (size <= 0) return 0;
   long l = BytesToIntMsb(buff,size-1) << 8;
   l |= static_cast<long>(buff[size-1]) & 255;
   return l;
}

long ReadIntLsb(istream &in, int size) {
   if (size <= 0) return 0;
   long l = static_cast<long>(in.get()) & 255;
   l |= ReadIntLsb(in,size-1)<<8;
   return l;
}

long BytesToIntLsb(void *vBuff, int size) {
   unsigned char *buff = reinterpret_cast<unsigned char *>(vBuff);
   if (size <= 0) return 0;
   long l = static_cast<long>(*buff) & 255;
   l |= BytesToIntLsb(buff+1,size-1)<<8;
   return l;
}

void SkipBytes(istream &in, int size) {
   while (size-- > 0)
      in.get();
}
void WriteIntMsb(ostream &out, long l, int size) {
   if (size <= 0) return;
   WriteIntMsb(out, l>>8, size-1); // Write MS Bytes
   out.put(l&255); // Write LS Byte
}

void WriteIntLsb(ostream &out, long l, int size) {
   if (size <= 0) return;
   out.put(l&255);  // Write LS Byte
   WriteIntLsb(out, l>>8, size-1); // Write rest
}
