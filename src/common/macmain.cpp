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
#include "audio.h"
#include "open.h"
#include "aplayer.h"
#include "macplayr.h"
#include <fstream>

static void InitializeSystem();
static void CheckEvent();
static void DoClick(EventRecord *event);
static void PlayFileSpec(FSSpec *fileSpec);

#include <SIOUX.h>

static void InitializeSystem() { // Initialize system components
   InitGraf(&qd.thePort);     // QuickDraw
   InitFonts();               // Font Manager
   FlushEvents(everyEvent,0); // Flush any pending events
   InitWindows();             // Window Manager
   InitMenus();               // Menu Manager
   TEInit();                  // Text Extensions
   InitDialogs(nil);          // Dialog Manager
   InitCursor();              // Cursor

   // Alter SIOUX defaults
   SIOUXSettings.standalone = FALSE;
   SIOUXSettings.setupmenus = FALSE;
   SIOUXSettings.initializeTB = FALSE;
   SIOUXSettings.autocloseonquit = TRUE;
   SIOUXSettings.columns = 80;
   SIOUXSettings.rows = 12;
   SIOUXSettings.asktosaveonclose = 0;
   SIOUXSetTitle("\pPlay Sound");

   // Set up my menus   
   Handle menuBar = GetNewMBar(128); // Pull menu bar from resources
   SetMenuBar(menuBar);
   DisposeHandle(menuBar);
   MenuHandle appleMenu = GetMenuHandle(128); // Get Apple menu
   AppendResMenu(appleMenu,'DRVR'); // Add system entries to Apple menu
   DrawMenuBar(); // Update menu bar
}
static void CheckEvent() {
   EventRecord event;
   Boolean     eventOccured;
   eventOccured = WaitNextEvent(everyEvent, &event, 10, nil);

   if(eventOccured && !SIOUXHandleOneEvent(&event)) {
      switch(event.what) {
      case mouseDown: DoClick(&event); break;
      default:                         break;
      }
   }
}
static void DoClick(EventRecord *event) {
   WindowPtr whichWindow;
   switch(FindWindow(event->where,&whichWindow)) {
   case inMenuBar: 
      long select = MenuSelect(event->where);
      switch(select >> 16) {
      case 128: // Apple Menu
      {
         MenuHandle mh = GetMenuHandle(select >> 16);
         unsigned char itemName[255];
         GetMenuItemText(mh, (select & 0xFFFF), itemName);
         OpenDeskAcc(itemName);
         HiliteMenu(0);
         break;
      }
      case 129: // File menu
         switch(select & 0xFFFF) {
         case 1: // Selected File : Play...
            // Throw up a standard file selection dialog
            SFTypeList typeList;
            StandardFileReply selectedFile;
            StandardGetFile (
               // No filtering
               reinterpret_cast<RoutineDescriptor *>(nil),
               -1, // All types
               typeList, // Not used
               &selectedFile); // return selected file info
            // Disable Play... menu item
            MenuHandle mh = GetMenuHandle(129);
            DisableItem(mh,1);
            HiliteMenu(0);
            DrawMenuBar();
            // Play file
            PlayFileSpec(&selectedFile.sfFile);
            // Re-enable Play... menu item
            EnableItem(mh,1);
            DrawMenuBar();
            break;
         case 2: // Quit entry
            exit(0);
         }
      }
      break;
   }
}
static void PlayFileSpec(FSSpec *fileSpec) {
   char *name = reinterpret_cast<char *>(p2cstr(fileSpec->name));
   TRACE1( "File: %s", name );

   short volumeId;
   OpenWD(fileSpec->vRefNum,fileSpec->parID,NULL, &volumeId);
   SetVol(NULL,volumeId);

   ifstream input(name, ios_base::in | ios_base::binary);
   if (!input.is_open()) {
      cerr << "Couldn't open file " << name << "\n";
      return;
   }
   AudioAbstract *audioRead = OpenFormat(input);
   if (audioRead) {
      MacPlayer player(audioRead);
      player.Play(CheckEvent);
      delete audioRead;
      cerr << "Finished playing " << name << ".\n";
   }
   cerr << "\n";
}

int main() {
   InitializeSystem();
   cout << "Select File : Play.. to play a file.\n";
   while(1)
      CheckEvent();
   return 0;
}
