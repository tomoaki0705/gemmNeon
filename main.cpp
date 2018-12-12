#include <iostream>
#include <cstdlib>
#include <arm_neon.h>
#include <chrono>
#include <cstring>
#include <vector>
#include <algorithm>
#if HAVE_ASIMDHP
const int maxMethod = 16;
#else
const int maxMethod = 10;
#endif

int main(int argc, char** argv)
{
    int method = 0;
    if (2 <= argc)
        method = std::min(std::max(atoi(argv[1]), 0), maxMethod);
    uint64_t N = 512;
    if (3 <= argc)
        N = std::max(atoi(argv[2]), 8);

    std::cout << "type: " << method << std::endl;
    std::cout << "size: " << N << "x" << N << std::endl; 
    float *A = new float[N*N];
    float *B = new float[N*N];
    float *C = new float[N*N];
    std::vector<double> history;
    for (auto n = 0;n < 10;n++)
    {
        std::memset(C, 0, sizeof(float)*N*N);
        auto start = std::chrono::system_clock::now();
        switch(method)
        {
        case 0:
        default:
            for (int i = 0;i < N;i++)
            for (int j = 0;j < N;j++)
            for (int k = 0;k < N;k++)
                C[i*N+j] += A[i*N+k] * B[k*N+j];
            break;
        case 1:
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 4)
            {
                float Cij[16] = {0};
                for (int k = 0;k < N;k++)
                {
                    float A0k = A[(i0+0)*N+k];
                    float A1k = A[(i0+1)*N+k];
                    float A2k = A[(i0+2)*N+k];
                    float A3k = A[(i0+3)*N+k];
                    float Bk0 = B[k*N+j0+0];
                    float Bk1 = B[k*N+j0+1];
                    float Bk2 = B[k*N+j0+2];
                    float Bk3 = B[k*N+j0+3];

                    Cij[0] += A0k * Bk0;
                    Cij[1] += A0k * Bk1;
                    Cij[2] += A0k * Bk2;
                    Cij[3] += A0k * Bk3;
                    Cij[4] += A1k * Bk0;
                    Cij[5] += A1k * Bk1;
                    Cij[6] += A1k * Bk2;
                    Cij[7] += A1k * Bk3;
                    Cij[8] += A2k * Bk0;
                    Cij[9] += A2k * Bk1;
                    Cij[10] += A2k * Bk2;
                    Cij[11] += A2k * Bk3;
                    Cij[12] += A3k * Bk0;
                    Cij[13] += A3k * Bk1;
                    Cij[14] += A3k * Bk2;
                    Cij[15] += A3k * Bk3;
                }
                C[(i0+0)*N+j0+0] = Cij[0];
                C[(i0+0)*N+j0+1] = Cij[1];
                C[(i0+0)*N+j0+2] = Cij[2];
                C[(i0+0)*N+j0+3] = Cij[3];
                C[(i0+1)*N+j0+0] = Cij[4];
                C[(i0+1)*N+j0+1] = Cij[5];
                C[(i0+1)*N+j0+2] = Cij[6];
                C[(i0+1)*N+j0+3] = Cij[7];
                C[(i0+2)*N+j0+0] = Cij[8];
                C[(i0+2)*N+j0+1] = Cij[9];
                C[(i0+2)*N+j0+2] = Cij[10];
                C[(i0+2)*N+j0+3] = Cij[11];
                C[(i0+3)*N+j0+0] = Cij[12];
                C[(i0+3)*N+j0+1] = Cij[13];
                C[(i0+3)*N+j0+2] = Cij[14];
                C[(i0+3)*N+j0+3] = Cij[15];
            }
            break;
        case 2:
	    std::cout << "22222" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 4)
            {
                float32x4_t vC0 = vdupq_n_f32(0);
                float32x4_t vC1 = vdupq_n_f32(0);
                float32x4_t vC2 = vdupq_n_f32(0);
                float32x4_t vC3 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float v[] = {A[(i0+0)*N+k], A[(i0+1)*N+k], A[(i0+2)*N+k], A[(i0+3)*N+k],};
                    float32x2_t vA0 = vld1_f32(v+0);
                    float32x2_t vA1 = vld1_f32(v+2);
                    float32x4_t vB = vld1q_f32(&B[k*N+j0+0]);

                    vC0 = vmlaq_lane_f32(vC0, vB, vA0, 0);
                    vC1 = vmlaq_lane_f32(vC1, vB, vA0, 1);
                    vC2 = vmlaq_lane_f32(vC2, vB, vA1, 0);
                    vC3 = vmlaq_lane_f32(vC3, vB, vA1, 1);

                }
                vst1q_f32(C+(i0+0)*N+j0, vC0);
                vst1q_f32(C+(i0+1)*N+j0, vC1);
                vst1q_f32(C+(i0+2)*N+j0, vC2);
                vst1q_f32(C+(i0+3)*N+j0, vC3);
            }
            break;
        case 3:
	    std::cout << "33333" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 4)
            {
                float32x4_t vC0 = vdupq_n_f32(0);
                float32x4_t vC1 = vdupq_n_f32(0);
                float32x4_t vC2 = vdupq_n_f32(0);
                float32x4_t vC3 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float32x4_t vA0 = vdupq_n_f32(A[(i0+0)*N+k]);
                    float32x4_t vA1 = vdupq_n_f32(A[(i0+1)*N+k]);
                    float32x4_t vA2 = vdupq_n_f32(A[(i0+2)*N+k]);
                    float32x4_t vA3 = vdupq_n_f32(A[(i0+3)*N+k]);
                    float32x4_t vB = vld1q_f32(&B[k*N+j0]);

                    vC0 = vmlaq_f32(vC0, vB, vA0);
                    vC1 = vmlaq_f32(vC1, vB, vA1);
                    vC2 = vmlaq_f32(vC2, vB, vA2);
                    vC3 = vmlaq_f32(vC3, vB, vA3);

                }
                vst1q_f32(C+(i0+0)*N+j0, vC0);
                vst1q_f32(C+(i0+1)*N+j0, vC1);
                vst1q_f32(C+(i0+2)*N+j0, vC2);
                vst1q_f32(C+(i0+3)*N+j0, vC3);
            }
            break;
        case 4:
	    std::cout << "44444" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 8)
            for (int j0 = 0;j0 < N;j0 += 4)
            {
                float32x4_t vC0 = vdupq_n_f32(0);
                float32x4_t vC1 = vdupq_n_f32(0);
                float32x4_t vC2 = vdupq_n_f32(0);
                float32x4_t vC3 = vdupq_n_f32(0);
                float32x4_t vC4 = vdupq_n_f32(0);
                float32x4_t vC5 = vdupq_n_f32(0);
                float32x4_t vC6 = vdupq_n_f32(0);
                float32x4_t vC7 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float32x4_t vA0 = vdupq_n_f32(A[(i0+0)*N+k]);
                    float32x4_t vA1 = vdupq_n_f32(A[(i0+1)*N+k]);
                    float32x4_t vA2 = vdupq_n_f32(A[(i0+2)*N+k]);
                    float32x4_t vA3 = vdupq_n_f32(A[(i0+3)*N+k]);
                    float32x4_t vB0 = vld1q_f32(&B[(k+0)*N+j0]);
                    float32x4_t vB1 = vld1q_f32(&B[(k+1)*N+j0]);

                    vC0 = vmlaq_f32(vC0, vB0, vA0);
                    vC1 = vmlaq_f32(vC1, vB0, vA1);
                    vC2 = vmlaq_f32(vC2, vB0, vA2);
                    vC3 = vmlaq_f32(vC3, vB0, vA3);
                    vC4 = vmlaq_f32(vC4, vB1, vA0);
                    vC5 = vmlaq_f32(vC5, vB1, vA1);
                    vC6 = vmlaq_f32(vC6, vB1, vA2);
                    vC7 = vmlaq_f32(vC7, vB1, vA3);
                }
                vst1q_f32(C+(i0+0)*N+j0, vC0);
                vst1q_f32(C+(i0+1)*N+j0, vC1);
                vst1q_f32(C+(i0+2)*N+j0, vC2);
                vst1q_f32(C+(i0+3)*N+j0, vC3);
                vst1q_f32(C+(i0+4)*N+j0, vC4);
                vst1q_f32(C+(i0+5)*N+j0, vC5);
                vst1q_f32(C+(i0+6)*N+j0, vC6);
                vst1q_f32(C+(i0+7)*N+j0, vC7);
            }
            break;
        case 5:
	    std::cout << "55555" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 8)
            {
                float32x4_t vC0 = vdupq_n_f32(0);
                float32x4_t vC1 = vdupq_n_f32(0);
                float32x4_t vC2 = vdupq_n_f32(0);
                float32x4_t vC3 = vdupq_n_f32(0);
                float32x4_t vC4 = vdupq_n_f32(0);
                float32x4_t vC5 = vdupq_n_f32(0);
                float32x4_t vC6 = vdupq_n_f32(0);
                float32x4_t vC7 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float32x4_t vA0 = vdupq_n_f32(A[(i0+0)*N+k]);
                    float32x4_t vA1 = vdupq_n_f32(A[(i0+1)*N+k]);
                    float32x4_t vB0 = vld1q_f32(&B[(k+0)*N+j0]);
                    float32x4_t vB1 = vld1q_f32(&B[(k+1)*N+j0]);
                    float32x4_t vB2 = vld1q_f32(&B[(k+2)*N+j0]);
                    float32x4_t vB3 = vld1q_f32(&B[(k+3)*N+j0]);

                    vC0 = vmlaq_f32(vC0, vB0, vA0);
                    vC1 = vmlaq_f32(vC1, vB0, vA1);
                    vC2 = vmlaq_f32(vC2, vB1, vA0);
                    vC3 = vmlaq_f32(vC3, vB1, vA1);
                    vC4 = vmlaq_f32(vC4, vB2, vA0);
                    vC5 = vmlaq_f32(vC5, vB2, vA1);
                    vC6 = vmlaq_f32(vC6, vB3, vA0);
                    vC7 = vmlaq_f32(vC7, vB3, vA1);
                }
                vst1q_f32(C+(i0+0)*N+j0+0, vC0);
                vst1q_f32(C+(i0+1)*N+j0+0, vC1);
                vst1q_f32(C+(i0+2)*N+j0+0, vC2);
                vst1q_f32(C+(i0+3)*N+j0+0, vC3);
                vst1q_f32(C+(i0+0)*N+j0+4, vC4);
                vst1q_f32(C+(i0+1)*N+j0+4, vC5);
                vst1q_f32(C+(i0+2)*N+j0+4, vC6);
                vst1q_f32(C+(i0+3)*N+j0+4, vC7);
            }
            break;
        case 6:
	    std::cout << "66666" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 8)
            for (int j0 = 0;j0 < N;j0 += 8)
            {
                float32x4_t vC0 = vdupq_n_f32(0);
                float32x4_t vC1 = vdupq_n_f32(0);
                float32x4_t vC2 = vdupq_n_f32(0);
                float32x4_t vC3 = vdupq_n_f32(0);
                float32x4_t vC4 = vdupq_n_f32(0);
                float32x4_t vC5 = vdupq_n_f32(0);
                float32x4_t vC6 = vdupq_n_f32(0);
                float32x4_t vC7 = vdupq_n_f32(0);
                float32x4_t vC8 = vdupq_n_f32(0);
                float32x4_t vC9 = vdupq_n_f32(0);
                float32x4_t vC10 = vdupq_n_f32(0);
                float32x4_t vC11 = vdupq_n_f32(0);
                float32x4_t vC12 = vdupq_n_f32(0);
                float32x4_t vC13 = vdupq_n_f32(0);
                float32x4_t vC14 = vdupq_n_f32(0);
                float32x4_t vC15 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float32x4_t vA0 = vdupq_n_f32(A[(i0+0)*N+k]);
                    float32x4_t vA1 = vdupq_n_f32(A[(i0+1)*N+k]);
                    float32x4_t vA2 = vdupq_n_f32(A[(i0+2)*N+k]);
                    float32x4_t vA3 = vdupq_n_f32(A[(i0+3)*N+k]);
                    float32x4_t vB0 = vld1q_f32(&B[(k+0)*N+j0]);
                    float32x4_t vB1 = vld1q_f32(&B[(k+1)*N+j0]);
                    float32x4_t vB2 = vld1q_f32(&B[(k+2)*N+j0]);
                    float32x4_t vB3 = vld1q_f32(&B[(k+3)*N+j0]);

                    vC0 = vmlaq_f32(vC0, vB0, vA0);
                    vC1 = vmlaq_f32(vC1, vB0, vA1);
                    vC2 = vmlaq_f32(vC2, vB0, vA2);
                    vC3 = vmlaq_f32(vC3, vB0, vA3);
                    vC4 = vmlaq_f32(vC4, vB1, vA0);
                    vC5 = vmlaq_f32(vC5, vB1, vA1);
                    vC6 = vmlaq_f32(vC6, vB1, vA2);
                    vC7 = vmlaq_f32(vC7, vB1, vA3);
                    vC8 = vmlaq_f32(vC8, vB2, vA0);
                    vC9 = vmlaq_f32(vC9, vB2, vA1);
                    vC10 = vmlaq_f32(vC10, vB2, vA2);
                    vC11 = vmlaq_f32(vC11, vB2, vA3);
                    vC12 = vmlaq_f32(vC12, vB3, vA0);
                    vC13 = vmlaq_f32(vC13, vB3, vA1);
                    vC14 = vmlaq_f32(vC14, vB3, vA2);
                    vC15 = vmlaq_f32(vC15, vB3, vA3);
                }
                vst1q_f32(C+(i0+0)*N+j0+0, vC0);
                vst1q_f32(C+(i0+1)*N+j0+0, vC1);
                vst1q_f32(C+(i0+2)*N+j0+0, vC2);
                vst1q_f32(C+(i0+3)*N+j0+0, vC3);
                vst1q_f32(C+(i0+4)*N+j0+0, vC4);
                vst1q_f32(C+(i0+5)*N+j0+0, vC5);
                vst1q_f32(C+(i0+6)*N+j0+0, vC6);
                vst1q_f32(C+(i0+7)*N+j0+0, vC7);
                vst1q_f32(C+(i0+0)*N+j0+4, vC8);
                vst1q_f32(C+(i0+1)*N+j0+4, vC9);
                vst1q_f32(C+(i0+2)*N+j0+4, vC10);
                vst1q_f32(C+(i0+3)*N+j0+4, vC11);
                vst1q_f32(C+(i0+0)*N+j0+4, vC12);
                vst1q_f32(C+(i0+1)*N+j0+4, vC13);
                vst1q_f32(C+(i0+2)*N+j0+4, vC14);
                vst1q_f32(C+(i0+3)*N+j0+4, vC15);
            }
            break;
        case 10:
            {
                std::cout << "10" << std::endl;
                float16_t *Ah = (float16_t*)A;
                float16_t *Bh = (float16_t*)B;
                float16_t *Ch = (float16_t*)C;
                for (int i = 0;i < N;i++)
                for (int j = 0;j < N;j++)
                for (int k = 0;k < N;k++)
                    Ch[i*N+j] += Ah[i*N+k] * Bh[k*N+j];
            }
            break;
#if HAVE_ASIMDHP
        case 16:
            {
                std::cout << "16" << std::endl;
                float16_t *Ah = (float16_t*)A;
                float16_t *Bh = (float16_t*)B;
                float16_t *Ch = (float16_t*)C;
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
                        float16x8_t vB0 = vld1q_f16(&Bh[(k+0)*N+j0]);
                        float16x8_t vB1 = vld1q_f16(&Bh[(k+1)*N+j0]);
                        float16x8_t vB2 = vld1q_f16(&Bh[(k+2)*N+j0]);
                        float16x8_t vB3 = vld1q_f16(&Bh[(k+3)*N+j0]);

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
                    vst1q_f16(Ch+(i0+0)*N+j0+4, vC12);
                    vst1q_f16(Ch+(i0+1)*N+j0+4, vC13);
                    vst1q_f16(Ch+(i0+2)*N+j0+4, vC14);
                    vst1q_f16(Ch+(i0+3)*N+j0+4, vC15);
                }
            }
            break;
#endif
        }
        
        auto end = std::chrono::system_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        history.push_back(elapsed);
    }
    std::sort(history.begin(), history.end());
    double median = history[history.size()/2];
    double minimum = history[0];
    double maximum = history[history.size()-1];
    double operations = N*N*N*2;
    std::cout << "time min  :" << minimum << "[ms]" << std::endl;
    std::cout << "time med  :" << median  << "[ms]" << std::endl;
    std::cout << "time max  :" << maximum << "[ms]" << std::endl;
    std::cout << "operations:" << operations << "[FLOP]" << std::endl;
    std::cout << "FLOPS     :" << (operations * 1e-6 / median) << "GFLOPS" << std::endl;
    delete [] A;
    delete [] B;
    delete [] C;
#if HAVE_ASIMDHP
    {
        float16_t value[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, };
	float16_t resultWrite[8];
	float16x8_t a = vld1q_f16(value + 0);
	float16x8_t b = vld1q_f16(value + 8);
	float16x8_t c = vld1q_f16(value + 16);
	float16x8_t result = vfmaq_f16(a, b, c);
	vst1q_f16(resultWrite, result);
	std::cout << resultWrite << std::endl;
    }
#endif
    return 0;
}
