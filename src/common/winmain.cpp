/*
   Copyright 1997 Tim Kientzle.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
      This product includes software developed by Tim Kientzle
      and published in ``The Programmer's Guide to Sound.''
4. Neither the names of Tim Kientzle nor Addison-Wesley
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL TIM KIENTZLE OR ADDISON-WESLEY BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <fstream>
#include "audio.h"
#include "open.h"
#include "aplayer.h"
#include "winplayr.h"
#include <commdlg.h>
#include <winbase.h>

#include <crtdbg.h>
#include <cassert>

void PlayFile(char *fileName) {
   cerr << "File: " << fileName << "\n";
   ifstream input(fileName,ios::in|ios::binary);
   if (!input.is_open()) {
      cerr << "Couldn't open file " << fileName << "\n";
      return;
   }
   AudioAbstract *audioFile = OpenFormat(input);
   if (audioFile) {
      WinPlayer player(audioFile);
      player.Play();
      delete audioFile;
      cerr << "Finished playing " << fileName << ".\n";
   }
   cerr << "\n";
   // Paranoia: Check that the Heap is still okay
   assert(_CrtCheckMemory());
}
void Interactive() {
   while (1) {
      OPENFILENAME ofn;
      char fileName[512] = "";
      memset(&ofn,0,sizeof(ofn));
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.lpstrFile = fileName;
      ofn.lpstrTitle = "Play Sound File";
      ofn.nMaxFile =       512;
      if (GetOpenFileName(&ofn)) // Throw up dialog
         PlayFile(fileName);
      else
         return;
   }
}
void Batch(char *filePattern) {
   WIN32_FIND_DATA fileData;
   bool finished = false;
   HANDLE hSearch = FindFirstFile(filePattern,&fileData);
   if (hSearch == INVALID_HANDLE_VALUE) {
      cerr << "Can't find " << filePattern << "\n";
      finished = true;
   }
   while (!finished) {
      // Skip directories, especially . and ..
      if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
         char fileName[2048];
         strcpy(fileName,filePattern);
         char *p=fileName + strlen(fileName) - 1;
         while ((p > fileName) && (*p != '\\') && (*p != '/'))
            p--;
         strcpy(p+1,fileData.cFileName);
         PlayFile(fileName);
      }
      finished = !FindNextFile(hSearch,&fileData);
   }
   FindClose(hSearch);
}

int main(int argc, char **argv) {
   // For testing, you can hard-wire a collection of files by
   // uncommenting and editing these two lines:
   // Batch("C:\\samples\\*.*");
   // exit(0);

   if (argc == 1)
      Interactive();
   else {
      while (--argc)
         Batch(*++argv);
   }
   return 0;
}
