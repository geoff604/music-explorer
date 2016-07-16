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
/* Generated on Sun Nov  7 20:44:19 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2real 6 */

/*
 * This function contains 14 FP additions, 4 FP multiplications,
 * (or, 12 additions, 2 multiplications, 2 fused multiply/add),
 * 14 stack variables, and 12 memory accesses
 */
static const fftw_real K2_000000000 = FFTW_KONST(+2.000000000000000000000000000000000000000000000);
static const fftw_real K1_732050807 = FFTW_KONST(+1.732050807568877293527446341505872366942805254);

/*
 * Generator Id's : 
 * $Id: fcr_6.c,v 1.1.1.1 2003/04/07 23:26:53 anonymous Exp $
 * $Id: fcr_6.c,v 1.1.1.1 2003/04/07 23:26:53 anonymous Exp $
 * $Id: fcr_6.c,v 1.1.1.1 2003/04/07 23:26:53 anonymous Exp $
 */

void fftw_hc2real_6(const fftw_real *real_input, const fftw_real *imag_input, fftw_real *output, int real_istride, int imag_istride, int ostride)
{
     fftw_real tmp3;
     fftw_real tmp7;
     fftw_real tmp12;
     fftw_real tmp14;
     fftw_real tmp6;
     fftw_real tmp8;
     fftw_real tmp1;
     fftw_real tmp2;
     fftw_real tmp13;
     fftw_real tmp9;
     ASSERT_ALIGNED_DOUBLE;
     tmp1 = real_input[0];
     tmp2 = real_input[3 * real_istride];
     tmp3 = tmp1 - tmp2;
     tmp7 = tmp1 + tmp2;
     {
	  fftw_real tmp10;
	  fftw_real tmp11;
	  fftw_real tmp4;
	  fftw_real tmp5;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp10 = imag_input[2 * imag_istride];
	  tmp11 = imag_input[imag_istride];
	  tmp12 = K1_732050807 * (tmp10 - tmp11);
	  tmp14 = K1_732050807 * (tmp10 + tmp11);
	  tmp4 = real_input[2 * real_istride];
	  tmp5 = real_input[real_istride];
	  tmp6 = tmp4 - tmp5;
	  tmp8 = tmp4 + tmp5;
     }
     output[3 * ostride] = tmp3 + (K2_000000000 * tmp6);
     tmp13 = tmp3 - tmp6;
     output[ostride] = tmp13 - tmp14;
     output[5 * ostride] = tmp13 + tmp14;
     output[0] = tmp7 + (K2_000000000 * tmp8);
     tmp9 = tmp7 - tmp8;
     output[4 * ostride] = tmp9 - tmp12;
     output[2 * ostride] = tmp9 + tmp12;
}

fftw_codelet_desc fftw_hc2real_6_desc =
{
     "fftw_hc2real_6",
     (void (*)()) fftw_hc2real_6,
     6,
     FFTW_BACKWARD,
     FFTW_HC2REAL,
     147,
     0,
     (const int *) 0,
};
