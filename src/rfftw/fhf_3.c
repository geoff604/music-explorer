/*
 * Copyright (c) 1997-1999 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Nov  7 20:44:43 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2hc-forward 3 */

/*
 * This function contains 25 FP additions, 16 FP multiplications,
 * (or, 17 additions, 8 multiplications, 8 fused multiply/add),
 * 15 stack variables, and 24 memory accesses
 */
static const fftw_real K866025403 = FFTW_KONST(+0.866025403784438646763723170752936183471402627);
static const fftw_real K500000000 = FFTW_KONST(+0.500000000000000000000000000000000000000000000);

/*
 * Generator Id's : 
 * $Id: fhf_3.c,v 1.1.1.1 2003/04/07 23:26:54 anonymous Exp $
 * $Id: fhf_3.c,v 1.1.1.1 2003/04/07 23:26:54 anonymous Exp $
 * $Id: fhf_3.c,v 1.1.1.1 2003/04/07 23:26:54 anonymous Exp $
 */

void fftw_hc2hc_forward_3(fftw_real *A, const fftw_complex *W, int iostride, int m, int dist)
{
     int i;
     fftw_real *X;
     fftw_real *Y;
     X = A;
     Y = A + (3 * iostride);
     {
	  fftw_real tmp24;
	  fftw_real tmp25;
	  fftw_real tmp26;
	  fftw_real tmp27;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp24 = X[0];
	  tmp25 = X[iostride];
	  tmp26 = X[2 * iostride];
	  tmp27 = tmp25 + tmp26;
	  X[iostride] = tmp24 - (K500000000 * tmp27);
	  X[0] = tmp24 + tmp27;
	  Y[-iostride] = K866025403 * (tmp26 - tmp25);
     }
     X = X + dist;
     Y = Y - dist;
     for (i = 2; i < m; i = i + 2, X = X + dist, Y = Y - dist, W = W + 2) {
	  fftw_real tmp4;
	  fftw_real tmp21;
	  fftw_real tmp9;
	  fftw_real tmp17;
	  fftw_real tmp14;
	  fftw_real tmp18;
	  fftw_real tmp15;
	  fftw_real tmp20;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp4 = X[0];
	  tmp21 = Y[-2 * iostride];
	  {
	       fftw_real tmp6;
	       fftw_real tmp8;
	       fftw_real tmp5;
	       fftw_real tmp7;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp6 = X[iostride];
	       tmp8 = Y[-iostride];
	       tmp5 = c_re(W[0]);
	       tmp7 = c_im(W[0]);
	       tmp9 = (tmp5 * tmp6) - (tmp7 * tmp8);
	       tmp17 = (tmp7 * tmp6) + (tmp5 * tmp8);
	  }
	  {
	       fftw_real tmp11;
	       fftw_real tmp13;
	       fftw_real tmp10;
	       fftw_real tmp12;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp11 = X[2 * iostride];
	       tmp13 = Y[0];
	       tmp10 = c_re(W[1]);
	       tmp12 = c_im(W[1]);
	       tmp14 = (tmp10 * tmp11) - (tmp12 * tmp13);
	       tmp18 = (tmp12 * tmp11) + (tmp10 * tmp13);
	  }
	  tmp15 = tmp9 + tmp14;
	  tmp20 = tmp17 + tmp18;
	  {
	       fftw_real tmp16;
	       fftw_real tmp19;
	       fftw_real tmp22;
	       fftw_real tmp23;
	       ASSERT_ALIGNED_DOUBLE;
	       X[0] = tmp4 + tmp15;
	       tmp16 = tmp4 - (K500000000 * tmp15);
	       tmp19 = K866025403 * (tmp17 - tmp18);
	       Y[-2 * iostride] = tmp16 - tmp19;
	       X[iostride] = tmp16 + tmp19;
	       Y[0] = tmp20 + tmp21;
	       tmp22 = tmp21 - (K500000000 * tmp20);
	       tmp23 = K866025403 * (tmp14 - tmp9);
	       X[2 * iostride] = -(tmp22 - tmp23);
	       Y[-iostride] = tmp23 + tmp22;
	  }
     }
     if (i == m) {
	  fftw_real tmp1;
	  fftw_real tmp3;
	  fftw_real tmp2;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp1 = X[iostride];
	  tmp3 = X[0];
	  tmp2 = X[2 * iostride];
	  Y[0] = -(K866025403 * (tmp1 + tmp2));
	  X[iostride] = tmp3 + tmp2 - tmp1;
	  X[0] = tmp3 - (K500000000 * (tmp2 - tmp1));
     }
}

static const int twiddle_order[] =
{1, 2};
fftw_codelet_desc fftw_hc2hc_forward_3_desc =
{
     "fftw_hc2hc_forward_3",
     (void (*)()) fftw_hc2hc_forward_3,
     3,
     FFTW_FORWARD,
     FFTW_HC2HC,
     69,
     2,
     twiddle_order,
};
