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
/* Generated on Sun Nov  7 20:44:23 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -hc2real 12 */

/*
 * This function contains 38 FP additions, 10 FP multiplications,
 * (or, 34 additions, 6 multiplications, 4 fused multiply/add),
 * 22 stack variables, and 24 memory accesses
 */
static const fftw_real K1_732050807 = FFTW_KONST(+1.732050807568877293527446341505872366942805254);
static const fftw_real K2_000000000 = FFTW_KONST(+2.000000000000000000000000000000000000000000000);

/*
 * Generator Id's : 
 * $Id: fcr_12.c,v 1.1.1.1 2003/04/07 23:26:53 anonymous Exp $
 * $Id: fcr_12.c,v 1.1.1.1 2003/04/07 23:26:53 anonymous Exp $
 * $Id: fcr_12.c,v 1.1.1.1 2003/04/07 23:26:53 anonymous Exp $
 */

void fftw_hc2real_12(const fftw_real *real_input, const fftw_real *imag_input, fftw_real *output, int real_istride, int imag_istride, int ostride)
{
     fftw_real tmp8;
     fftw_real tmp11;
     fftw_real tmp22;
     fftw_real tmp36;
     fftw_real tmp32;
     fftw_real tmp33;
     fftw_real tmp25;
     fftw_real tmp37;
     fftw_real tmp3;
     fftw_real tmp27;
     fftw_real tmp16;
     fftw_real tmp6;
     fftw_real tmp28;
     fftw_real tmp20;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp9;
	  fftw_real tmp10;
	  fftw_real tmp23;
	  fftw_real tmp24;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp8 = real_input[3 * real_istride];
	  tmp9 = real_input[5 * real_istride];
	  tmp10 = real_input[real_istride];
	  tmp11 = tmp9 + tmp10;
	  tmp22 = (K2_000000000 * tmp8) - tmp11;
	  tmp36 = K1_732050807 * (tmp9 - tmp10);
	  tmp32 = imag_input[3 * imag_istride];
	  tmp23 = imag_input[5 * imag_istride];
	  tmp24 = imag_input[imag_istride];
	  tmp33 = tmp23 + tmp24;
	  tmp25 = K1_732050807 * (tmp23 - tmp24);
	  tmp37 = (K2_000000000 * tmp32) + tmp33;
     }
     {
	  fftw_real tmp15;
	  fftw_real tmp1;
	  fftw_real tmp2;
	  fftw_real tmp13;
	  fftw_real tmp14;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp14 = imag_input[4 * imag_istride];
	  tmp15 = K1_732050807 * tmp14;
	  tmp1 = real_input[0];
	  tmp2 = real_input[4 * real_istride];
	  tmp13 = tmp1 - tmp2;
	  tmp3 = tmp1 + (K2_000000000 * tmp2);
	  tmp27 = tmp13 - tmp15;
	  tmp16 = tmp13 + tmp15;
     }
     {
	  fftw_real tmp19;
	  fftw_real tmp4;
	  fftw_real tmp5;
	  fftw_real tmp17;
	  fftw_real tmp18;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp18 = imag_input[2 * imag_istride];
	  tmp19 = K1_732050807 * tmp18;
	  tmp4 = real_input[6 * real_istride];
	  tmp5 = real_input[2 * real_istride];
	  tmp17 = tmp4 - tmp5;
	  tmp6 = tmp4 + (K2_000000000 * tmp5);
	  tmp28 = tmp17 + tmp19;
	  tmp20 = tmp17 - tmp19;
     }
     {
	  fftw_real tmp7;
	  fftw_real tmp12;
	  fftw_real tmp39;
	  fftw_real tmp40;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp7 = tmp3 + tmp6;
	  tmp12 = K2_000000000 * (tmp8 + tmp11);
	  output[6 * ostride] = tmp7 - tmp12;
	  output[0] = tmp7 + tmp12;
	  {
	       fftw_real tmp31;
	       fftw_real tmp34;
	       fftw_real tmp21;
	       fftw_real tmp26;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp31 = tmp3 - tmp6;
	       tmp34 = K2_000000000 * (tmp32 - tmp33);
	       output[9 * ostride] = tmp31 - tmp34;
	       output[3 * ostride] = tmp31 + tmp34;
	       tmp21 = tmp16 + tmp20;
	       tmp26 = tmp22 - tmp25;
	       output[2 * ostride] = tmp21 - tmp26;
	       output[8 * ostride] = tmp21 + tmp26;
	  }
	  tmp39 = tmp16 - tmp20;
	  tmp40 = tmp37 - tmp36;
	  output[5 * ostride] = tmp39 - tmp40;
	  output[11 * ostride] = tmp39 + tmp40;
	  {
	       fftw_real tmp35;
	       fftw_real tmp38;
	       fftw_real tmp29;
	       fftw_real tmp30;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp35 = tmp27 - tmp28;
	       tmp38 = tmp36 + tmp37;
	       output[ostride] = tmp35 - tmp38;
	       output[7 * ostride] = tmp35 + tmp38;
	       tmp29 = tmp27 + tmp28;
	       tmp30 = tmp22 + tmp25;
	       output[10 * ostride] = tmp29 - tmp30;
	       output[4 * ostride] = tmp29 + tmp30;
	  }
     }
}

fftw_codelet_desc fftw_hc2real_12_desc =
{
     "fftw_hc2real_12",
     (void (*)()) fftw_hc2real_12,
     12,
     FFTW_BACKWARD,
     FFTW_HC2REAL,
     279,
     0,
     (const int *) 0,
};
