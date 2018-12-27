#include <iostream>
#include <cstdlib>
#include <arm_neon.h>
#include <chrono>
#include <cstring>
#include <vector>
#include <algorithm>

void kernel18(float16_t* A, float16_t* B, float16_t* C, uint64_t N)
{
    float16_t *Ah = A;
    float16_t *Bh = B;
    float16_t *Ch = C;
    for (int i0 = 0;i0 < N;i0 += 8)
    for (int j0 = 0;j0 < N;j0 += 8)
    {
        float16x8_t vC0 = vdupq_n_f16(0);
        float16x8_t vC1 = vdupq_n_f16(0);
        float16x8_t vC2 = vdupq_n_f16(0);
        float16x8_t vC3 = vdupq_n_f16(0);
        float16x8_t vC4 = vdupq_n_f16(0);
        float16x8_t vC5 = vdupq_n_f16(0);
        float16x8_t vC6 = vdupq_n_f16(0);
        float16x8_t vC7 = vdupq_n_f16(0);
        float16x8_t vC8 = vdupq_n_f16(0);
        float16x8_t vC9 = vdupq_n_f16(0);
        float16x8_t vC10 = vdupq_n_f16(0);
        float16x8_t vC11 = vdupq_n_f16(0);
        float16x8_t vC12 = vdupq_n_f16(0);
        float16x8_t vC13 = vdupq_n_f16(0);
        float16x8_t vC14 = vdupq_n_f16(0);
        float16x8_t vC15 = vdupq_n_f16(0);
        for (int k = 0;k < N;k++)
        {
            float16x8_t vA0 = vdupq_n_f16(Ah[(i0+0)*N+k]);
            float16x8_t vA1 = vdupq_n_f16(Ah[(i0+1)*N+k]);
            float16x8_t vA2 = vdupq_n_f16(Ah[(i0+2)*N+k]);
            float16x8_t vA3 = vdupq_n_f16(Ah[(i0+3)*N+k]);
            register float16x8_t vB0 __asm__("v0") = vld1q_f16(&Bh[(k+0)*N+j0]);
            register float16x8_t vB1 __asm__("v1") = vld1q_f16(&Bh[(k+1)*N+j0]);
            register float16x8_t vB2 __asm__("v2") = vld1q_f16(&Bh[(k+2)*N+j0]);
            register float16x8_t vB3 __asm__("v3") = vld1q_f16(&Bh[(k+3)*N+j0]);

            vC0 = vfmaq_f16(vC0, vB0, vA0);
            vC1 = vfmaq_f16(vC1, vB0, vA1);
            vC2 = vfmaq_f16(vC2, vB0, vA2);
            vC3 = vfmaq_f16(vC3, vB0, vA3);
            vC4 = vfmaq_f16(vC4, vB1, vA0);
            vC5 = vfmaq_f16(vC5, vB1, vA1);
            vC6 = vfmaq_f16(vC6, vB1, vA2);
            vC7 = vfmaq_f16(vC7, vB1, vA3);
            vC8 = vfmaq_f16(vC8, vB2, vA0);
            vC9 = vfmaq_f16(vC9, vB2, vA1);
            vC10 = vfmaq_f16(vC10, vB2, vA2);
            vC11 = vfmaq_f16(vC11, vB2, vA3);
            vC12 = vfmaq_f16(vC12, vB3, vA0);
            vC13 = vfmaq_f16(vC13, vB3, vA1);
            vC14 = vfmaq_f16(vC14, vB3, vA2);
            vC15 = vfmaq_f16(vC15, vB3, vA3);
        }
        vst1q_f16(Ch+(i0+0)*N+j0+0, vC0);
        vst1q_f16(Ch+(i0+1)*N+j0+0, vC1);
        vst1q_f16(Ch+(i0+2)*N+j0+0, vC2);
        vst1q_f16(Ch+(i0+3)*N+j0+0, vC3);
        vst1q_f16(Ch+(i0+4)*N+j0+0, vC4);
        vst1q_f16(Ch+(i0+5)*N+j0+0, vC5);
        vst1q_f16(Ch+(i0+6)*N+j0+0, vC6);
        vst1q_f16(Ch+(i0+7)*N+j0+0, vC7);
        vst1q_f16(Ch+(i0+0)*N+j0+4, vC8);
        vst1q_f16(Ch+(i0+1)*N+j0+4, vC9);
        vst1q_f16(Ch+(i0+2)*N+j0+4, vC10);
        vst1q_f16(Ch+(i0+3)*N+j0+4, vC11);
        vst1q_f16(Ch+(i0+4)*N+j0+4, vC12);
        vst1q_f16(Ch+(i0+5)*N+j0+4, vC13);
        vst1q_f16(Ch+(i0+6)*N+j0+4, vC14);
        vst1q_f16(Ch+(i0+7)*N+j0+4, vC15);
    }
}

