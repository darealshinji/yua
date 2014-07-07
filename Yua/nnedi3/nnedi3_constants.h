#ifndef NNEDI3_CONSTANTS_H
#define NNEDI3_CONSTANTS_H

#include <float.h>
#include <inttypes.h>

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define CB2(n) MAX(MIN((n),254),0)

const float sse_half[4] __attribute__((aligned(16))) = { 0.5f, 0.5f, 0.5f, 0.5f };

const float ones_f[4] __attribute__((aligned(16))) = { 1.0f, 1.0f, 1.0f, 1.0f };

const long long sign_bits_f[2] __attribute__((aligned(16))) = { 0x7FFFFFFF7FFFFFFF, 0x7FFFFFFF7FFFFFFF };
const long long sign_bits_f_zero_l[2] __attribute__((aligned(16))) = { 0x7FFFFFFF00000000, 0x7FFFFFFF7FFFFFFF };

const float e0_mult[4] __attribute__((aligned(16))) = { // (1.0/ln(2))*(2^23)
        12102203.161561486f, 12102203.161561486f, 12102203.161561486f, 12102203.161561486f };
const float e0_bias[4] __attribute__((aligned(16))) = { // (2^23)*127.0-486411.0
        1064866805.0f, 1064866805.0f, 1064866805.0f, 1064866805.0f };

const float exp_lo[4] __attribute__((aligned(16))) = { -80.0f, -80.0f, -80.0f, -80.0f };
const float exp_hi[4] __attribute__((aligned(16))) = { +80.0f, +80.0f, +80.0f, +80.0f };

const float am_0p5[4] __attribute__((aligned(16))) = { 0.5f, 0.5f, 0.5f, 0.5f };
const float am_1[4] __attribute__((aligned(16))) = { 1.0f, 1.0f, 1.0f, 1.0f };
const float exp_rln2[4] __attribute__((aligned(16))) = { 1.442695041f, 1.442695041f, 1.442695041f, 1.442695041f };
const float exp_p0[4] __attribute__((aligned(16))) = { 1.261771931e-4f, 1.261771931e-4f, 1.261771931e-4f, 1.261771931e-4f };
const float exp_p1[4] __attribute__((aligned(16))) = { 3.029944077e-2f, 3.029944077e-2f, 3.029944077e-2f, 3.029944077e-2f };
const float exp_q0[4] __attribute__((aligned(16))) = { 3.001985051e-6f, 3.001985051e-6f, 3.001985051e-6f, 3.001985051e-6f };
const float exp_q1[4] __attribute__((aligned(16))) = { 2.524483403e-3f, 2.524483403e-3f, 2.524483403e-3f, 2.524483403e-3f };
const float exp_q2[4] __attribute__((aligned(16))) = { 2.272655482e-1f, 2.272655482e-1f, 2.272655482e-1f, 2.272655482e-1f };
const float exp_q3[4] __attribute__((aligned(16))) = { 2.000000000f, 2.000000000f, 2.000000000f, 2.000000000f };
const float exp_c1[4] __attribute__((aligned(16))) = { 6.931457520e-1f, 6.931457520e-1f, 6.931457520e-1f, 6.931457520e-1f };
const float exp_c2[4] __attribute__((aligned(16))) = { 1.428606820e-6f, 1.428606820e-6f, 1.428606820e-6f, 1.428606820e-6f };
const int32_t epi32_1[4] __attribute__((aligned(16))) = { 1, 1, 1, 1 };
const int32_t epi32_0x7f[4] __attribute__((aligned(16))) = { 0x7f, 0x7f, 0x7f, 0x7f };
const float min_weight_sum[4] __attribute__((aligned(16))) = { 1e-10f, 1e-10f, 1e-10f, 1e-10f };
const float five_f[4] __attribute__((aligned(16))) = { 5.0f, 5.0f, 5.0f, 5.0f };

const unsigned char ub_1[16] __attribute__((aligned(16))) = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
const int16_t w_19[8] __attribute__((aligned(16))) = { 19, 19, 19, 19, 19, 19, 19, 19 };
const int16_t w_3[8] __attribute__((aligned(16))) = { 3, 3, 3, 3, 3, 3, 3, 3 };
const int16_t w_254[8] __attribute__((aligned(16))) = { 254, 254, 254, 254, 254, 254, 254, 254 };
const uint16_t uw_16[8] __attribute__((aligned(16))) = { 16, 16, 16, 16, 16, 16, 16, 16 };

const float flt_epsilon_sse[4] __attribute__((aligned(16))) = { FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON };

const float e1_scale[4] __attribute__((aligned(16))) = { // 1/ln(2)
        1.4426950409f, 1.4426950409f, 1.4426950409f, 1.4426950409f };
const float e1_bias[4] __attribute__((aligned(16))) = { // 3<<22
        12582912.0f, 12582912.0f, 12582912.0f, 12582912.0f };
const float e1_c0[4] __attribute__((aligned(16))) = { 1.00035f, 1.00035f, 1.00035f, 1.00035f };
const float e1_c1[4] __attribute__((aligned(16))) = { 0.701277797f, 0.701277797f, 0.701277797f, 0.701277797f };
const float e1_c2[4] __attribute__((aligned(16))) = { 0.237348593f, 0.237348593f, 0.237348593f, 0.237348593f };

#endif
