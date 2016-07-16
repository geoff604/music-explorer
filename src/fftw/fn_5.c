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
/* Generated on Sun Nov  7 20:43:47 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -notwiddle 5 */

/*
 * This function contains 32 FP additions, 12 FP multiplications,
 * (or, 26 additions, 6 multiplications, 6 fused multiply/add),
 * 16 stack variables, and 20 memory accesses
 */
static const fftw_real K250000000 = FFTW_KONST(+0.250000000000000000000000000000000000000000000);
static const fftw_real K587785252 = FFTW_KONST(+0.587785252292473129168705954639072768597652438);
static const fftw_real K951056516 = FFTW_KONST(+0.951056516295153572116439333379382143405698634);
static const fftw_real K559016994 = FFTW_KONST(+0.559016994374947424102293417182819058860154590);

/*
 * Generator Id's : 
 * $Id: fn_5.c,v 1.1.1.1 2003/04/07 23:26:50 anonymous Exp $
 * $Id: fn_5.c,v 1.1.1.1 2003/04/07 23:26:50 anonymous Exp $
 * $Id: fn_5.c,v 1.1.1.1 2003/04/07 23:26:50 anonymous Exp $
 */

void fftw_no_twiddle_5(const fftw_complex *input, fftw_complex *output, int istride, int ostride)
{
     fftw_real tmp1;
     fftw_real tmp24;
     fftw_real tmp8;
     fftw_real tmp9;
     fftw_real tmp28;
     fftw_real tmp29;
     fftw_real tmp14;
     fftw_real tmp25;
     fftw_real tmp23;
     fftw_real tmp17;
     ASSERT_ALIGNED_DOUBLE;
     tmp1 = c_re(input[0]);
     tmp24 = c_im(input[0]);
     {
	  fftw_real tmp2;
	  fftw_real tmp3;
	  fftw_real tmp4;
	  fftw_real tmp5;
	  fftw_real tmp6;
	  fftw_real tmp7;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp2 = c_re(input[istride]);
	  tmp3 = c_re(input[4 * istride]);
	  tmp4 = tmp2 + tmp3;
	  tmp5 = c_re(input[2 * istride]);
	  tmp6 = c_re(input[3 * istride]);
	  tmp7 = tmp5 + tmp6;
	  tmp8 = tmp4 + tmp7;
	  tmp9 = K559016994 * (tmp4 - tmp7);
	  tmp28 = tmp2 - tmp3;
	  tmp29 = tmp5 - tmp6;
     }
     {
	  fftw_real tmp12;
	  fftw_real tmp13;
	  fftw_real tmp21;
	  fftw_real tmp15;
	  fftw_real tmp16;
	  fftw_real tmp22;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp12 = c_im(input[istride]);
	  tmp13 = c_im(input[4 * istride]);
	  tmp21 = tmp12 + tmp13;
	  tmp15 = c_im(input[2 * istride]);
	  tmp16 = c_im(input[3 * istride]);
	  tmp22 = tmp15 + tmp16;
	  tmp14 = tmp12 - tmp13;
	  tmp25 = tmp21 + tmp22;
	  tmp23 = K559016994 * (tmp21 - tmp22);
	  tmp17 = tmp15 - tmp16;
     }
     c_re(output[0]) = tmp1 + tmp8;
     {
	  fftw_real tmp18;
	  fftw_real tmp20;
	  fftw_real tmp11;
	  fftw_real tmp19;
	  fftw_real tmp10;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp18 = (K951056516 * tmp14) + (K587785252 * tmp17);
	  tmp20 = (K951056516 * tmp17) - (K587785252 * tmp14);
	  tmp10 = tmp1 - (K250000000 * tmp8);
	  tmp11 = tmp9 + tmp10;
	  tmp19 = tmp10 - tmp9;
	  c_re(output[4 * ostride]) = tmp11 - tmp18;
	  c_re(output[ostride]) = tmp11 + tmp18;
	  c_re(output[2 * ostride]) = tmp19 - tmp20;
	  c_re(output[3 * ostride]) = tmp19 + tmp20;
     }
     c_im(output[0]) = tmp24 + tmp25;
     {
	  fftw_real tmp30;
	  fftw_real tmp31;
	  fftw_real tmp27;
	  fftw_real tmp32;
	  fftw_real tmp26;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp30 = (K951056516 * tmp28) + (K587785252 * tmp29);
	  tmp31 = (K951056516 * tmp29) - (K587785252 * tmp28);
	  tmp26 = tmp24 - (K250000000 * tmp25);
	  tmp27 = tmp23 + tmp26;
	  tmp32 = tmp26 - tmp23;
	  c_im(output[ostride]) = tmp27 - tmp30;
	  c_im(output[4 * ostride]) = tmp30 + tmp27;
	  c_im(output[2 * ostride]) = tmp31 + tmp32;
	  c_im(output[3 * ostride]) = tmp32 - tmp31;
     }
}

fftw_codelet_desc fftw_no_twiddle_5_desc =
{
     "fftw_no_twiddle_5",
     (void (*)()) fftw_no_twiddle_5,
     5,
     FFTW_FORWARD,
     FFTW_NOTW,
     111,
     0,
     (const int *) 0,
};
