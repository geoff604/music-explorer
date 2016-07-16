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
/* Generated on Sun Nov  7 20:43:54 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -real2hc 7 */

/*
 * This function contains 24 FP additions, 18 FP multiplications,
 * (or, 24 additions, 18 multiplications, 0 fused multiply/add),
 * 13 stack variables, and 14 memory accesses
 */
static const fftw_real K222520933 = FFTW_KONST(+0.222520933956314404288902564496794759466355569);
static const fftw_real K900968867 = FFTW_KONST(+0.900968867902419126236102319507445051165919162);
static const fftw_real K623489801 = FFTW_KONST(+0.623489801858733530525004884004239810632274731);
static const fftw_real K433883739 = FFTW_KONST(+0.433883739117558120475768332848358754609990728);
static const fftw_real K781831482 = FFTW_KONST(+0.781831482468029808708444526674057750232334519);
static const fftw_real K974927912 = FFTW_KONST(+0.974927912181823607018131682993931217232785801);

/*
 * Generator Id's : 
 * $Id: frc_7.c,v 1.1.1.1 2003/04/07 23:26:54 anonymous Exp $
 * $Id: frc_7.c,v 1.1.1.1 2003/04/07 23:26:54 anonymous Exp $
 * $Id: frc_7.c,v 1.1.1.1 2003/04/07 23:26:54 anonymous Exp $
 */

void fftw_real2hc_7(const fftw_real *input, fftw_real *real_output, fftw_real *imag_output, int istride, int real_ostride, int imag_ostride)
{
     fftw_real tmp1;
     fftw_real tmp10;
     fftw_real tmp11;
     fftw_real tmp4;
     fftw_real tmp13;
     fftw_real tmp7;
     fftw_real tmp12;
     fftw_real tmp8;
     fftw_real tmp9;
     ASSERT_ALIGNED_DOUBLE;
     tmp1 = input[0];
     tmp8 = input[istride];
     tmp9 = input[6 * istride];
     tmp10 = tmp8 + tmp9;
     tmp11 = tmp9 - tmp8;
     {
	  fftw_real tmp2;
	  fftw_real tmp3;
	  fftw_real tmp5;
	  fftw_real tmp6;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp2 = input[2 * istride];
	  tmp3 = input[5 * istride];
	  tmp4 = tmp2 + tmp3;
	  tmp13 = tmp3 - tmp2;
	  tmp5 = input[3 * istride];
	  tmp6 = input[4 * istride];
	  tmp7 = tmp5 + tmp6;
	  tmp12 = tmp6 - tmp5;
     }
     imag_output[2 * imag_ostride] = (K974927912 * tmp11) - (K781831482 * tmp12) - (K433883739 * tmp13);
     imag_output[imag_ostride] = (K781831482 * tmp11) + (K974927912 * tmp13) + (K433883739 * tmp12);
     imag_output[3 * imag_ostride] = (K433883739 * tmp11) + (K974927912 * tmp12) - (K781831482 * tmp13);
     real_output[2 * real_ostride] = tmp1 + (K623489801 * tmp7) - (K900968867 * tmp4) - (K222520933 * tmp10);
     real_output[real_ostride] = tmp1 + (K623489801 * tmp10) - (K900968867 * tmp7) - (K222520933 * tmp4);
     real_output[3 * real_ostride] = tmp1 + (K623489801 * tmp4) - (K222520933 * tmp7) - (K900968867 * tmp10);
     real_output[0] = tmp1 + tmp10 + tmp4 + tmp7;
}

fftw_codelet_desc fftw_real2hc_7_desc =
{
     "fftw_real2hc_7",
     (void (*)()) fftw_real2hc_7,
     7,
     FFTW_FORWARD,
     FFTW_REAL2HC,
     156,
     0,
     (const int *) 0,
};
