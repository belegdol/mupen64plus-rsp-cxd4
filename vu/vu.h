/******************************************************************************\
* Project:  MSP Emulation Layer for Vector Unit Computational Operations       *
* Authors:  Iconoclast                                                         *
* Release:  2014.10.14                                                         *
* License:  CC0 Public Domain Dedication                                       *
*                                                                              *
* To the extent possible under law, the author(s) have dedicated all copyright *
* and related and neighboring rights to this software to the public domain     *
* worldwide. This software is distributed without any warranty.                *
*                                                                              *
* You should have received a copy of the CC0 Public Domain Dedication along    *
* with this software.                                                          *
* If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.             *
\******************************************************************************/
#ifndef _VU_H_
#define _VU_H_

#ifdef ARCH_MIN_SSE2
#include <emmintrin.h>
#endif

#include "../my_types.h"

#define N       8
/* N:  number of processor elements in SIMD processor */

/*
 * We are going to need this for vector operations doing scalar things.
 * The divides and VSAW need bit-wise information from the instruction word.
 */
extern u32 inst;

/*
 * RSP virtual registers (of vector unit)
 * The most important are the 32 general-purpose vector registers.
 * The correct way to accurately store these is using big-endian vectors.
 *
 * For ?WC2 we may need to do byte-precision access just as directly.
 * This is amended by using the `VU_S` and `VU_B` macros defined in `rsp.h`.
 */
ALIGNED extern short VR[32][N];

/*
 * The RSP accumulator is a vector of 3 48-bit integers.  Nearly all of the
 * vector operations access it, but it's for multiply-accumulate operations.
 *
 * Access dimensions would be VACC[8][3] but are inverted for SIMD benefits.
 */
ALIGNED extern i16 VACC[3][N];

/*
 * accumulator-indexing macros
 */
#define HI      00
#define MD      01
#define LO      02

#define VACC_L      (VACC[LO])
#define VACC_M      (VACC[MD])
#define VACC_H      (VACC[HI])

#define ACC_L(i)    (VACC_L)[i]
#define ACC_M(i)    (VACC_M)[i]
#define ACC_H(i)    (VACC_H)[i]

#ifdef ARCH_MIN_SSE2
typedef __m128i v16;
#else
typedef short * v16;
#endif

#define VECTOR_OPERATION    v16
#define VECTOR_EXTERN       extern v16

NOINLINE void message(const char* body);

VECTOR_EXTERN (*COP2_C2[64])(v16, v16, v16);

#ifdef ARCH_MIN_SSE2

#define vector_copy(vd, vs) { \
    *(v16 *)(vd) = *(v16 *)(vs); }
#define vector_wipe(vd) { \
    (vd) = _mm_xor_si128(vd, vd); }
#define vector_fill(vd) { \
    (vd) = _mm_cmpeq_epi16(vd, vd); }

#define vector_and(vd, vs) { \
    (vd) = _mm_and_si128    (vd, vs); }
#define vector_or(vd, vs) { \
    (vd) = _mm_or_si128     (vd, vs); }
#define vector_xor(vd, vs) { \
    (vd) = _mm_xor_si128    (vd, vs); }

/*
 * Every competent vector unit should have at least two vector comparison
 * operations:  EQ and LT/GT.  (MMX makes us say GT; SSE's LT is just a GT.)
 *
 * Default examples when compiling for the x86 SSE2 architecture below.
 */
#define vector_cmplt(vd, vs) { \
    (vd) = _mm_cmplt_epi16  (vd, vs); }
#define vector_cmpeq(vd, vs) { \
    (vd) = _mm_cmpeq_epi16  (vd, vs); }
#define vector_cmpgt(vd, vs) { \
    (vd) = _mm_cmpgt_epi16  (vd, vs); }

#else

#define vector_copy(vd, vs) { \
    (vd)[0] = (vs)[0]; \
    (vd)[1] = (vs)[1]; \
    (vd)[2] = (vs)[2]; \
    (vd)[3] = (vs)[3]; \
    (vd)[4] = (vs)[4]; \
    (vd)[5] = (vs)[5]; \
    (vd)[6] = (vs)[6]; \
    (vd)[7] = (vs)[7]; \
}
#define vector_wipe(vd) { \
    (vd)[0] =  0x0000; \
    (vd)[1] =  0x0000; \
    (vd)[2] =  0x0000; \
    (vd)[3] =  0x0000; \
    (vd)[4] =  0x0000; \
    (vd)[5] =  0x0000; \
    (vd)[6] =  0x0000; \
    (vd)[7] =  0x0000; \
}
#define vector_fill(vd) { \
    (vd)[0] = ~0x0000; \
    (vd)[1] = ~0x0000; \
    (vd)[2] = ~0x0000; \
    (vd)[3] = ~0x0000; \
    (vd)[4] = ~0x0000; \
    (vd)[5] = ~0x0000; \
    (vd)[6] = ~0x0000; \
    (vd)[7] = ~0x0000; \
}
#define vector_and(vd, vs) { \
    (vd)[0] &= (vs)[0]; \
    (vd)[1] &= (vs)[1]; \
    (vd)[2] &= (vs)[2]; \
    (vd)[3] &= (vs)[3]; \
    (vd)[4] &= (vs)[4]; \
    (vd)[5] &= (vs)[5]; \
    (vd)[6] &= (vs)[6]; \
    (vd)[7] &= (vs)[7]; \
}
#define vector_or(vd, vs) { \
    (vd)[0] |= (vs)[0]; \
    (vd)[1] |= (vs)[1]; \
    (vd)[2] |= (vs)[2]; \
    (vd)[3] |= (vs)[3]; \
    (vd)[4] |= (vs)[4]; \
    (vd)[5] |= (vs)[5]; \
    (vd)[6] |= (vs)[6]; \
    (vd)[7] |= (vs)[7]; \
}
#define vector_xor(vd, vs) { \
    (vd)[0] ^= (vs)[0]; \
    (vd)[1] ^= (vs)[1]; \
    (vd)[2] ^= (vs)[2]; \
    (vd)[3] ^= (vs)[3]; \
    (vd)[4] ^= (vs)[4]; \
    (vd)[5] ^= (vs)[5]; \
    (vd)[6] ^= (vs)[6]; \
    (vd)[7] ^= (vs)[7]; \
}

#define vector_cmplt(vd, vs) { \
    (vd)[0] = ((vd)[0] < (vs)[0]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[1] < (vs)[1]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[2] < (vs)[2]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[3] < (vs)[3]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[4] < (vs)[4]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[5] < (vs)[5]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[6] < (vs)[6]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[7] < (vs)[7]) ? ~0x0000 :  0x0000; \
}
#define vector_cmpeq(vd, vs) { \
    (vd)[0] = ((vd)[0] == (vs)[0]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[1] == (vs)[1]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[2] == (vs)[2]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[3] == (vs)[3]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[4] == (vs)[4]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[5] == (vs)[5]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[6] == (vs)[6]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[7] == (vs)[7]) ? ~0x0000 :  0x0000; \
}
#define vector_cmpgt(vd, vs) { \
    (vd)[0] = ((vd)[0] > (vs)[0]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[1] > (vs)[1]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[2] > (vs)[2]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[3] > (vs)[3]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[4] > (vs)[4]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[5] > (vs)[5]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[6] > (vs)[6]) ? ~0x0000 :  0x0000; \
    (vd)[0] = ((vd)[7] > (vs)[7]) ? ~0x0000 :  0x0000; \
}

#endif

/*
 * Considering that almost all of the computational vector unit operations
 * concern a shuffled, halfword-swapped target vector, a centralized method
 * for shuffling said vector into a new vector temporary register,
 * ST = VR[vt], should be pretty convenient.
 */
INLINE extern v16 SHUFFLE_VECTOR(v16 VD, i16* VT, const int e);

/*
 * Many vector units have pairs of "vector condition flags" registers.
 * In SGI's vector unit implementation, these are denoted as the
 * "vector control registers" under coprocessor 2.
 *
 * VCF-0 is the carry-out flags register:  $vco.
 * VCF-1 is the compare code flags register:  $vcc.
 * VCF-2 is the compare extension flags register:  $vce.
 * There is no fourth RSP flags register.
 */
unsigned short VCO;
unsigned short VCC;
unsigned char VCE;

ALIGNED extern short ne[8];
ALIGNED extern short co[8];
ALIGNED extern short clip[8];
ALIGNED extern short comp[8];
ALIGNED extern short vce[8];

extern unsigned short get_VCO(void);
extern unsigned short get_VCC(void);
extern unsigned char get_VCE(void);

extern void set_VCO(unsigned short VCO);
extern void set_VCC(unsigned short VCC);
extern void set_VCE(unsigned char VCE);

#endif
