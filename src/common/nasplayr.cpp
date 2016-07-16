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
#include <typeinfo>
#include <iostream>
#include <fstream>
#include "audio.h"
#include "aplayer.h"
#include "nasplayr.h"

void NasPlayer::Play(){
   OpenServer();   // Establish a connection with the server
   InitializeQueue(128*1024L); // Initialize 128k queue
   
   AuStartFlow(_server,_flow,NULL);   // Unpause the flow
   
   while (!_finished) {   // Handle events until we're done
      AuEvent ev;
      AuNextEvent(_server, AuTrue, &ev);
      AuDispatchEvent(_server, &ev);
   }
   AuCloseServer(_server); // Close connection to server
}
AuBool NasEventHandler(AuServer *,  // Unused
                        AuEvent *event,
                        AuEventHandlerRec *eventData)
{
   NasPlayer *me = reinterpret_cast<NasPlayer *>(eventData -> data);
   switch (event->type) {
   case AuEventTypeElementNotify:
      return me->Notify(reinterpret_cast<AuElementNotifyEvent *>(event));
   default:  // Some other event occurred.
      break;
   }
   return AuTrue;
}
AuBool NasPlayer::Notify(AuElementNotifyEvent *notifyEvent) {
   switch (notifyEvent->kind) {
   case AuElementNotifyKindLowWater:  // Hit low water mark
      SendData(notifyEvent->num_bytes); // Send more data
      break;
   case AuElementNotifyKindState:
      // Server paused because of too little data?
      if (  (notifyEvent->cur_state == AuStatePause)
          &&(notifyEvent->reason == AuReasonUnderrun))
          SendData(notifyEvent->num_bytes);
      else // Any other state change: just end
         _finished = true;
      break;
   }
   return AuTrue;
}
void NasPlayer::SendData(AuUint32 numBytes){
   unsigned long bytesRead 
      = FromQueue(_buffer,numBytes/sizeof(Sample16))
       * sizeof(Sample16);
   bool allDone = ((bytesRead < numBytes) && _endOfSource);
   AuWriteElement(_server, _flow, 0, bytesRead, _buffer,
                  allDone, NULL);
   FillQueue();
}
unsigned char NasFormatCode(int sampleBits)
{
   if (sampleBits == 8)
      return AuFormatLinearSigned8;

   int lsb = 0; // default: MSB format
   { // test native storage for LSB format
      union { short int sixteen; // 16-bit value
         struct { char a, b; } eight; // two 8-bit bytes
      } lsbTest ;
      lsbTest.sixteen = 1; // Set low-order byte, clear high-order
      if (lsbTest.eight.a) { lsb = 1; } // check first byte
   }
   
   if (lsb) return AuFormatLinearSigned16LSB;
   else return AuFormatLinearSigned16MSB;
}
void NasPlayer::OpenServer(){
   AuDeviceID deviceID = AuNone;
   
   // Open a connection to the server
   const char * const NullString = (const char *)NULL;
   char *server_message;
   _server = AuOpenServer(
          NullString,0,NullString,0,NullString, // Use defaults
                          &server_message); // Error message
   if (server_message != (char *)NULL) {
      TRACE0( "Could not connect to audio server." );
      TRACE1( "Server: %s", server_message );
	  throw new CSoundException( CSoundException::causeConnectToAudioServer );
   }
   if (!_server) {
      throw new CSoundException( CSoundException::causeConnectToAudioServer );
   }
   
   // Select the first suitable output device
   for (int i=0; i< AuServerNumDevices(_server); i++) {
      AuDeviceAttributes *device = AuServerDevice(_server,i);
      if ((AuDeviceKind(device) == AuComponentKindPhysicalOutput) &&
          (AuDeviceNumTracks(device) == Channels())) {
         deviceID = AuDeviceIdentifier(device);
         break;
      }
   }
   if (deviceID == AuNone) {
	  throw new CSoundException( CSoundException::causeNoAudioOutputDevice );
   }
   
   // Request a new flow on the server
   _flow = AuGetScratchFlow(_server,NULL);
   if (_flow == AuNone) {
	  throw new CSoundException( CSoundException::causeCreateAudioFlowOnServer );		
   }
   
   // We need to tell the NAS server how to size its buffer
   long requestSize = (nasBufferSize)/Channels()/2;

   // Create elements for our flow
   AuElement elements[2];

   // First element is an Import element (I supply the data)
   AuMakeElementImportClient(&elements[0], // First element
      SamplingRate(),             // sample rate
      NasFormatCode(16),            // get format code
      Channels(),                 // number of Channels
      AuTrue,                     // Start paused
      requestSize,                // Samples to request
      requestSize/2,              // Low-water mark
      0,NULL);                    // No actions
   
   // Last element is the output device
   AuMakeElementExportDevice(&elements[1],
      0,                    // Get input from prev
      deviceID,             // ID of output device
      SamplingRate(),       // sample rate
      AuUnlimitedSamples,   // play forever
      0, NULL);             // No actions

   // Register this flow with the server
   AuStatus status;
   AuSetElements(_server,_flow,AuTrue,2,elements,&status);
   if (status) {
		throw new CSoundException( CSoundException::causeConfigureFlow );
   }
   
   // Register a handler for events from the import client
   AuEventHandlerRec *handler =
      AuRegisterEventHandler(_server,
           AuEventHandlerIDMask, // Handle events from a particular element
           0,                    // ... element zero (import client)
           _flow,                 // ... in this flow
           NasEventHandler,      // This is the handler
           reinterpret_cast<AuPointer>(this)); // This is the aux data

   if (!handler) {
      AuReleaseScratchFlow(_server,_flow,NULL);
	  throw new CSoundException( CSoundException::causeRegisterEventHandler );
   }
}
