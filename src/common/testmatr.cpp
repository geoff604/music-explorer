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
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <complex>

// On BSD UNIX systems, you might want to use
// srandom/random instead of srand/rand

// Initialize random number generator
#define INITRANDOM()  (srand(time(0)))
// Random 17-bit signed integer
#define RANDINT17() ((rand() & 0x1FFFF) - 65536)
// Random 16-bit signed integer
#define RANDINT16() ((rand() & 0xFFFF) - 32768)
// Random double between -1 and 1
#define RANDFLOAT() (rand()- (RAND_MAX/2))/(double)(RAND_MAX/2);

double Sfloat[32];
long Sint[32];
double Vfloat[64];
long Vint[64];

// ***************************************************************************

static void IntMatrix(long *V, long *subbandSamples) {
   int numSubbandSamples = 32;

   // N is constant, so it's stored in a static variable
   static long N[64][32];
   static bool initializedN = false;

   if (!initializedN) {
      for(int n=0;n<64;n++) {
         for(int k=0;k<32;k++)
            N[n][k] = static_cast<long>
                  (8192.0*cos((16+n)*(2*k+1)*(3.14159265358979/64.0)));
      }
      initializedN = true;
   }

   for(int n=0; n<64;n++) { // Matrixing
      long t=0; // sum of 32 numbers, each 3.20
      long *samples = subbandSamples; // 1.16
      long *matrix = N[n]; // 2.13
      for(int k=0;k<numSubbandSamples;k++)
         t += (*samples++ * *matrix++) >> 8;
      V[n] = t>>10; // V is .13
   }
}

// ***************************************************************************

static void FloatMatrix(double *V, double *subbandSamples) {
  static const double PI=3.14159265358979323846;

   // N is constant, so it's stored in a static variable
   static double N[64][32];
   static bool initializedN = false;

   if (!initializedN) {
      for(int i=0;i<64;i++) {
         for(int k=0;k<32;k++)
            N[i][k] = cos((16+i)*(2*k+1)*(PI/64.0));
      }
      initializedN = true;
   }

   for(int i=0; i<64;i++) { // Matrixing
      double t=0;
      double *samples = subbandSamples; // 1.16
      double *matrix = N[i]; // 2.13
      for(int k=0;k<32;k++)
         t += *samples++ * *matrix++;
      V[i] = t;
   }
}

// ***************************************************************************

static void FloatFastMatrix(double *V, double *subbandSamples) {
  static const double PI=3.14159265358979323846;
  complex<double> work[64];

  static const int order[] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,
                              1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};

  complex<double> *pWork = work;
  for(int i=0;i<32;i+=2) {
    double a = subbandSamples[order[i] ];
    double b = subbandSamples[order[i+1] ];
    *pWork++ = a+b;
    *pWork++ = complex<double>(a,b);
    *pWork++ = a-b;
    *pWork++ = complex<double>(a,-b);
  }

  static complex<double> phaseShifts[32];
  static bool initializedPhaseShifts = false;
  
  if (!initializedPhaseShifts) { // Initialize it only once
    for(int i=0;i<32;i++)
      phaseShifts[i] = polar(1.0,i*(PI/32.0));
    initializedPhaseShifts = true;
  }

  // In each iteration, I throw out one bit of accuracy
  // This gives me an extra bit of headroom to avoid overflow
  int phaseShiftIndex, phaseShiftStep = 8;

  for(int size=4; size < 64; size <<= 1) {
    phaseShiftIndex = 0;
    for(int fftStep = 0; fftStep < size; fftStep++) {
      complex<double> phaseShift(phaseShifts[phaseShiftIndex]);
      for(int i=fftStep; i < 64; i += 2*size) {
        complex<double> t = phaseShift * work[i+size];
        work[i+size] = work[i] - t;
        work[i] += t;
      }
      phaseShiftIndex += phaseShiftStep;
    }
    phaseShiftStep >>= 1;
  }

  // Build final V values by rotating FFT output
  static complex<double> vShift[64];
  static bool initializedVshift = false;
  
  if (!initializedVshift) { // Initialize it only once
    for(int i=0;i<32;i++)
      vShift[i] = polar(1.0,(32+i)*(PI/64.0));
    initializedVshift = true;
  }

  // V is the real part of a certain calculation
  complex<double> *pcm = work+32;
  for(int i=0;i<32;i++) V[i+16] = (vShift[i] * *pcm++).real();
  V[48] = -work[0].real();

  // Exploit symmetries
  for(int i=0;i<16;i++) V[i] = -V[32-i];
  for(int i=1;i<16;i++) V[48+i] = V[48-i];
}

// ***************************************************************************

// Input is 1.16
// Output is 16 bits
static void IntFastMatrix(long *V, long *subbandSamples) {
   static const double PI=3.14159265358979323846;
   long *workR=V; // Re-use V as work storage
   long workI[64]; // Imaginary part

   static const char order[] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,
                                1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};

   // The initialization step here precalculates the
   // two-point transforms (each two inputs generate four outputs)
   // I'm taking advantage of the fact that my inputs are all real
   long *pWorkR = workR; // 2.16
   long *pWorkI = workI; // 2.16
   const char *next = order;
   for(int i=0;i<16;i++) {
      long a = subbandSamples[*next++];
      long b = subbandSamples[*next++];
      *pWorkR++ = a+b;  *pWorkI++ = 0; 
      *pWorkR++ = a;    *pWorkI++ = b; 
      *pWorkR++ = a-b;  *pWorkI++ = 0; 
      *pWorkR++ = a;    *pWorkI++ = -b;
   }

   // This is a fast version of the transform in the ISO standard.
   // It's derived using the same principles as the FFT,
   // but it's NOT a Fourier Transform. 

   // For speed, precompute all of the phase shift values
   static long phaseShiftsR[32], phaseShiftsI[32]; // 1.14
   static bool initializedPhaseShifts = false;
  
   if (!initializedPhaseShifts) { // Initialize it only once
      for(int i=0;i<32;i++) { // 1.14
         phaseShiftsR[i] = static_cast<long>(16384.0*cos(i*(PI/32.0)));
         phaseShiftsI[i] = static_cast<long>(16384.0*sin(i*(PI/32.0)));
      }
      initializedPhaseShifts = true;
   }

   // In each iteration, I throw out one bit of accuracy
   // This gives me an extra bit of headroom to avoid overflow
   int phaseShiftIndex, phaseShiftStep = 8;
   for(int size=4; size<64; size <<= 1) {
      // Since the first phase shfit value is always 1,
      // I can save a few multiplies by duplicating the loop
      for(int n=0; n < 64; n += 2*size) {
         long tR = workR[n+size];
         workR[n+size] = (workR[n] - tR)>>1;
         workR[n] = (workR[n] + tR)>>1;
         long tI = workI[n+size];
         workI[n+size] = (workI[n] - tI)>>1;
         workI[n] = (workI[n] + tI)>>1;
      }
      phaseShiftIndex = phaseShiftStep;
      for(int fftStep = 1; fftStep < size; fftStep++) {
         long phaseShiftR = phaseShiftsR[phaseShiftIndex];
         long phaseShiftI = phaseShiftsI[phaseShiftIndex];
         phaseShiftIndex += phaseShiftStep;
         for(int n=fftStep; n < 64; n += 2*size) {
            long tR = (phaseShiftR*workR[n+size] 
                     - phaseShiftI*workI[n+size])>>14;
            long tI = (phaseShiftR*workI[n+size] 
                     + phaseShiftI*workR[n+size])>>14;
            workR[n+size] = (workR[n] - tR)>>1;
            workI[n+size] = (workI[n] - tI)>>1;
            workR[n] = (workR[n] + tR)>>1;
            workI[n] = (workI[n] + tI)>>1;
         }
      }
      phaseShiftStep /= 2;
   }

   // Build final V values by massaging transform output
   static long vShiftR[64], vShiftI[64]; // 1.13
   static bool initializedVshift = false;
  
   if (!initializedVshift) { // Initialize it only once
      for(int n=0;n<32;n++) { // 1.14
         vShiftR[n] = static_cast<long>(16384.0*cos((32+n)*(PI/64.0)));
         vShiftI[n] = static_cast<long>(16384.0*sin((32+n)*(PI/64.0)));
      }
      initializedVshift = true;
   }

   // Now build V values from the complex transform output
   long *pcmR = workR+33; // 6.12
   long *pcmI = workI+33; // 6.12
   V[16] = 0;    // V[16] is always 0
   for(int n=1;n<32;n++)    // V is the real part, V is 6.11
      V[n+16] = (vShiftR[n] * *pcmR++ - vShiftI[n] * *pcmI++)>>15;
   V[48] = (-workR[0])>>1;   // vShift[32] is always -1
   // Exploit symmetries in the result
   for(int n=0;n<16;n++) V[n] = -V[32-n];
   for(int n=1;n<16;n++) V[48+n] = V[48-n];
}

// ***************************************************************************

static void testit() {
  const long REPEATS = 10;
  printf ("Testing for Accuracy...\n");
  printf ("(Using %ld random sets of data)\n",REPEATS);

  double maxError=0;
  long maxInput=0, minInput=0;
  long maxOutput=0, minOutput=0;
  int maxAt = 0;

  // Initialize to alternating +1, -1, etc.
  Sint[0]=-65536; Sint[1]=65535;
  for(int i=2;i<32;i++) Sint[i] = Sint[i-2];
  // Initialize float to match
  for(int i=0;i<32;i++) Sfloat[i] = Sint[i]/65536.0;

  for(long repeat=0;repeat<REPEATS;repeat++)
  {
    if ((repeat % 1000) == 0) {
      printf("%ld ",REPEATS-repeat);
      fflush(stdout);
    }

    // Track max and min input values
    for(int i=0;i<32;i++) {
      if (Sint[i]>maxInput) maxInput = Sint[i];
      if (Sint[i]<minInput) minInput = Sint[i];
    }

    // Compare fast integer version to float version
    FloatMatrix(Vfloat,Sfloat);
    IntFastMatrix(Vint,Sint);
    
    // Find largest absolute error
    for(int i=0; i<64; i++) {
      // Track largest error
      double error = Vfloat[i]-Vint[i]/2048.0;
      if (error < 0) error = -error;
      if (error > maxError) { maxError = error; maxAt = i; }

      // Largest/smallest output
      if(Vint[i]>maxOutput) maxOutput=Vint[i];
      if(Vint[i]<minOutput) minOutput=Vint[i];
    }
    
    // Next loop uses random values
    for(int i=0;i<32;i++) Sint[i] = RANDINT17();
    for(int i=0;i<32;i++) Sfloat[i] = Sint[i]/65536.0;
  }
  printf("\n");
  printf("Max Error: %13.9f\n",maxError);
  printf("Approx Accuracy: %6.2f bits\n",log(16.0/maxError)/log(2.0));
  printf("Input--Max: %ld  Min:%ld\n",maxInput, minInput);
  printf("Output--Max: %ld  Min:%ld\n",maxOutput, minOutput);
}

// ***************************************************************************

static void timeit() {
  static const long REPEATS = 100000;
  printf ("\n\nTesting for Speed...\n");
  printf ("Please do not do anything while this is running...\n");
  printf ("Timing is accurate to %5.2f microseconds at best.\n",
          (float)(1000000.0/REPEATS));
  printf ("(Using %ld iterations, please be patient!)\n",REPEATS);

  // Fill S arrays with random values
  for(int i=0;i<32;i++) Sfloat[i] = RANDFLOAT();
  for(int i=0;i<32;i++) Sint[i] = RANDINT16();

  // Time fast integer version
  printf("Fast Integer Version: ");
  fflush(stdout);
  time_t start = time(0);
  for(long repeat=0;repeat<REPEATS;repeat++) IntFastMatrix(Vint,Sint);
  printf("%8.2f microseconds\n",
         difftime(time(0),start)/REPEATS * 1000000);

  // Time slow integer version
  printf("Slow Integer Version: ");
  fflush(stdout);
  start = time(0);
  for(long repeat=0;repeat<REPEATS;repeat++) IntMatrix(Vint,Sint);
  printf("%8.2f microseconds\n",
         difftime(time(0),start)/REPEATS * 1000000);

  // Time fast float version
  printf("Fast Float Version: ");
  fflush(stdout);
  start = time(0);
  for(long repeat=0;repeat<REPEATS;repeat++) FloatFastMatrix(Vfloat,Sfloat);
  printf("%8.2f microseconds\n",
         difftime(time(0),start)/REPEATS * 1000000);

  // Time slow float version
  printf("Slow Float Version: ");
  fflush(stdout);
  start = time(0);
  for(long repeat=0;repeat<REPEATS;repeat++) FloatMatrix(Vfloat,Sfloat);
  printf("%8.2f microseconds\n",
         difftime(time(0),start)/REPEATS * 1000000);
}

// ***************************************************************************

int main() {
  INITRANDOM();
  testit();
  timeit();
  return 0;
}
