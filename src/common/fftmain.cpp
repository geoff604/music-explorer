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
#include "fft.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

int main(int argc, char **argv) {
   int length = 8;
   if (argc > 1) length = atoi(argv[1]);
   if (((~length+1)&length) != length) {
     cerr << "Maximum relative error:" << maxerr << "\n";
   }
   complex<double> *input = new complex<double>[length];
   for(int i=0; (i < length)&&(!cin.eof()); i++)
      cin >> input[i];

   complex<double> *output = new complex<double>[length];
   for(int i=0;i<length;i++) output[i] = input[i];
   ForwardFft(output,length);

   // Uncomment next section to test
#if 0
   complex<double> *compare = new complex<double>[length];
   ForwardDft(input,length,compare);
   double maxerr=0; int maxindex=0;
   for(int i=0;i<length;i++) {
      // Compute the relative error
      double error = abs(output[i] - compare[i]) / abs(compare[i]);
      if(error > maxerr) {
         maxindex=i; maxerr=error;
      }
   }
   TRACE1( "Maximum relative error: %d", maxerr );
#endif

   for(int i=0;i<length;i++)
      cout << setprecision(20) << output[i] << "\n";
   return 0;
}
