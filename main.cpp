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
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 16)
            {
                float32x4_t vC00 = vdupq_n_f32(0);
                float32x4_t vC01 = vdupq_n_f32(0);
                float32x4_t vC02 = vdupq_n_f32(0);
                float32x4_t vC03 = vdupq_n_f32(0);
                float32x4_t vC10 = vdupq_n_f32(0);
                float32x4_t vC11 = vdupq_n_f32(0);
                float32x4_t vC12 = vdupq_n_f32(0);
                float32x4_t vC13 = vdupq_n_f32(0);
                float32x4_t vC20 = vdupq_n_f32(0);
                float32x4_t vC21 = vdupq_n_f32(0);
                float32x4_t vC22 = vdupq_n_f32(0);
                float32x4_t vC23 = vdupq_n_f32(0);
                float32x4_t vC30 = vdupq_n_f32(0);
                float32x4_t vC31 = vdupq_n_f32(0);
                float32x4_t vC32 = vdupq_n_f32(0);
                float32x4_t vC33 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float32x2_t vA0 = vld1_f32(&A[(i0+0)*N+k]);
                    float32x2_t vA1 = vld1_f32(&A[(i0+1)*N+k]);
                    float32x2_t vA2 = vld1_f32(&A[(i0+2)*N+k]);
                    float32x2_t vA3 = vld1_f32(&A[(i0+3)*N+k]);
                    float32x4_t vB0 = vld1q_f32(&B[(k+0)*N+j0+0 ]);
                    float32x4_t vB1 = vld1q_f32(&B[(k+0)*N+j0+4 ]);
                    float32x4_t vB2 = vld1q_f32(&B[(k+0)*N+j0+8 ]);
                    float32x4_t vB3 = vld1q_f32(&B[(k+0)*N+j0+12]);

                    vC00 = vmlaq_lane_f32(vC00, vB0, vA0, 0);
                    vC01 = vmlaq_lane_f32(vC01, vB0, vA1, 0);
                    vC02 = vmlaq_lane_f32(vC02, vB0, vA2, 0);
                    vC03 = vmlaq_lane_f32(vC03, vB0, vA3, 0);
                    vC10 = vmlaq_lane_f32(vC10, vB1, vA0, 0);
                    vC11 = vmlaq_lane_f32(vC11, vB1, vA1, 0);
                    vC12 = vmlaq_lane_f32(vC12, vB1, vA2, 0);
                    vC13 = vmlaq_lane_f32(vC13, vB1, vA3, 0);
                    vC20 = vmlaq_lane_f32(vC20, vB2, vA0, 0);
                    vC21 = vmlaq_lane_f32(vC21, vB2, vA1, 0);
                    vC22 = vmlaq_lane_f32(vC22, vB2, vA2, 0);
                    vC23 = vmlaq_lane_f32(vC23, vB2, vA3, 0);
                    vC30 = vmlaq_lane_f32(vC30, vB3, vA0, 0);
                    vC31 = vmlaq_lane_f32(vC31, vB3, vA1, 0);
                    vC32 = vmlaq_lane_f32(vC32, vB3, vA2, 0);
                    vC33 = vmlaq_lane_f32(vC33, vB3, vA3, 0);
                }
                vst1q_f32(C+(i0+0)*N+j0+0,  vC00);
                vst1q_f32(C+(i0+0)*N+j0+4,  vC01);
                vst1q_f32(C+(i0+0)*N+j0+8,  vC02);
                vst1q_f32(C+(i0+0)*N+j0+12, vC03);
                vst1q_f32(C+(i0+1)*N+j0+0,  vC10);
                vst1q_f32(C+(i0+1)*N+j0+4,  vC11);
                vst1q_f32(C+(i0+1)*N+j0+8,  vC12);
                vst1q_f32(C+(i0+1)*N+j0+12, vC13);
                vst1q_f32(C+(i0+2)*N+j0+0,  vC20);
                vst1q_f32(C+(i0+2)*N+j0+4,  vC21);
                vst1q_f32(C+(i0+2)*N+j0+8,  vC22);
                vst1q_f32(C+(i0+2)*N+j0+12, vC23);
                vst1q_f32(C+(i0+3)*N+j0+0,  vC30);
                vst1q_f32(C+(i0+3)*N+j0+4,  vC31);
                vst1q_f32(C+(i0+3)*N+j0+8,  vC32);
                vst1q_f32(C+(i0+3)*N+j0+12, vC33);
            }
            break;
        case 7:
            std::cout << "7777" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 16)
            {
		volatile float32x4_t aaaa = vld1q_dup_f32(A);
                float32x4_t vC00 = vdupq_n_f32(0);
                float32x4_t vC01 = vdupq_n_f32(0);
                float32x4_t vC02 = vdupq_n_f32(0);
                float32x4_t vC03 = vdupq_n_f32(0);
                float32x4_t vC10 = vdupq_n_f32(0);
                float32x4_t vC11 = vdupq_n_f32(0);
                float32x4_t vC12 = vdupq_n_f32(0);
                float32x4_t vC13 = vdupq_n_f32(0);
                float32x4_t vC20 = vdupq_n_f32(0);
                float32x4_t vC21 = vdupq_n_f32(0);
                float32x4_t vC22 = vdupq_n_f32(0);
                float32x4_t vC23 = vdupq_n_f32(0);
                float32x4_t vC30 = vdupq_n_f32(0);
                float32x4_t vC31 = vdupq_n_f32(0);
                float32x4_t vC32 = vdupq_n_f32(0);
                float32x4_t vC33 = vdupq_n_f32(0);
                for (int k = 0;k < N;k++)
                {
                    float* pointer0 = &A[(i0+0)*N+k];
                    float* pointer1 = &A[(i0+1)*N+k];
                    float* pointer2 = &A[(i0+2)*N+k];
                    float* pointer3 = &A[(i0+3)*N+k];
		    float32x4_t vA0, vA1, vA2, vA3;

                    asm(
                        "ld1r {%[a0].4s}, [%[pa0]]\n\t\t"
                        "ld1r {%[a1].4s}, [%[pa1]]\n\t\t"
                        "ld1r {%[a2].4s}, [%[pa2]]\n\t\t"
                        "ld1r {%[a3].4s}, [%[pa3]]\n\t\t"
			:[a0]"=w"(vA0) ,[a1]"=w" (vA1),[a2]"=w" (vA2),[a3]"=w"(vA3)
			:[pa0]"r"(pointer0) ,[pa1]"r"(pointer1) ,[pa2]"r"(pointer2) ,[pa3]"r"(pointer3)
		       );
                    float32x4_t vB0 = vld1q_f32(&B[(k+0)*N+j0+0 ]);
                    float32x4_t vB1 = vld1q_f32(&B[(k+0)*N+j0+4 ]);
                    float32x4_t vB2 = vld1q_f32(&B[(k+0)*N+j0+8 ]);
                    float32x4_t vB3 = vld1q_f32(&B[(k+0)*N+j0+12]);

                    asm(
                        "fmla %[c00].4s, %[b0].4s, %[a0].4s\n\t\t"
                        "fmla %[c01].4s, %[b0].4s, %[a1].4s\n\t\t"
                        "fmla %[c02].4s, %[b0].4s, %[a2].4s\n\t\t"
                        "fmla %[c03].4s, %[b0].4s, %[a3].4s\n\t\t"
                        "fmla %[c10].4s, %[b1].4s, %[a0].4s\n\t\t"
                        "fmla %[c11].4s, %[b1].4s, %[a1].4s\n\t\t"
                        "fmla %[c12].4s, %[b1].4s, %[a2].4s\n\t\t"
                        "fmla %[c13].4s, %[b1].4s, %[a3].4s\n\t\t"
                        :[c00]"+w"(vC00),[c01]"+w"(vC01),[c02]"+w"(vC02),[c03]"+w"(vC03) 
		   	,[c10]"+w"(vC10),[c11]"+w"(vC11),[c12]"+w"(vC12),[c13]"+w"(vC13) 
                        :[b0]"w"(vB0),[b1]"w"(vB1)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
		       );
		    asm(
                        "fmla %[c20].4s, %[b2].4s, %[a0].4s\n\t\t"
                        "fmla %[c21].4s, %[b2].4s, %[a1].4s\n\t\t"
                        "fmla %[c22].4s, %[b2].4s, %[a2].4s\n\t\t"
                        "fmla %[c23].4s, %[b2].4s, %[a3].4s\n\t\t"
                        "fmla %[c30].4s, %[b3].4s, %[a0].4s\n\t\t"
                        "fmla %[c31].4s, %[b3].4s, %[a1].4s\n\t\t"
                        "fmla %[c32].4s, %[b3].4s, %[a2].4s\n\t\t"
                        "fmla %[c33].4s, %[b3].4s, %[a3].4s\n\t\t"
                        :[c20]"+w"(vC20),[c21]"+w"(vC21),[c22]"+w"(vC22),[c23]"+w"(vC23)
			,[c30]"+w"(vC30),[c31]"+w"(vC31),[c32]"+w"(vC32),[c33]"+w"(vC33)
                        :[b2]"w"(vB2),[b3]"w"(vB3)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
                    );

                }
                vst1q_f32(C+(i0+0)*N+j0+0,  vC00);
                vst1q_f32(C+(i0+0)*N+j0+4,  vC01);
                vst1q_f32(C+(i0+0)*N+j0+8,  vC02);
                vst1q_f32(C+(i0+0)*N+j0+12, vC03);
                vst1q_f32(C+(i0+1)*N+j0+0,  vC10);
                vst1q_f32(C+(i0+1)*N+j0+4,  vC11);
                vst1q_f32(C+(i0+1)*N+j0+8,  vC12);
                vst1q_f32(C+(i0+1)*N+j0+12, vC13);
                vst1q_f32(C+(i0+2)*N+j0+0,  vC20);
                vst1q_f32(C+(i0+2)*N+j0+4,  vC21);
                vst1q_f32(C+(i0+2)*N+j0+8,  vC22);
                vst1q_f32(C+(i0+2)*N+j0+12, vC23);
                vst1q_f32(C+(i0+3)*N+j0+0,  vC30);
                vst1q_f32(C+(i0+3)*N+j0+4,  vC31);
                vst1q_f32(C+(i0+3)*N+j0+8,  vC32);
                vst1q_f32(C+(i0+3)*N+j0+12, vC33);
            }
            break;
        case 8:
            std::cout << "8888" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 16)
            {
                float32x4_t vC00 = vdupq_n_f32(0);
                float32x4_t vC01 = vdupq_n_f32(0);
                float32x4_t vC02 = vdupq_n_f32(0);
                float32x4_t vC03 = vdupq_n_f32(0);
                float32x4_t vC10 = vdupq_n_f32(0);
                float32x4_t vC11 = vdupq_n_f32(0);
                float32x4_t vC12 = vdupq_n_f32(0);
                float32x4_t vC13 = vdupq_n_f32(0);
                float32x4_t vC20 = vdupq_n_f32(0);
                float32x4_t vC21 = vdupq_n_f32(0);
                float32x4_t vC22 = vdupq_n_f32(0);
                float32x4_t vC23 = vdupq_n_f32(0);
                float32x4_t vC30 = vdupq_n_f32(0);
                float32x4_t vC31 = vdupq_n_f32(0);
                float32x4_t vC32 = vdupq_n_f32(0);
                float32x4_t vC33 = vdupq_n_f32(0);
	        float32x4_t vA0, vA1, vA2, vA3;
	        float32x4_t vB0, vB1, vB2, vB3;
		float32x4_t vA0n, vA1n, vA2n, vA3n;
                float32x4_t vB0n, vB1n, vB2n, vB3n;
		// pre-load
		{
                    float* pointer0 = &A[(i0+0)*N];
                    float* pointer1 = &A[(i0+1)*N];
                    float* pointer2 = &A[(i0+2)*N];
                    float* pointer3 = &A[(i0+3)*N];

                    asm(
                        "ld1r {%[a0].4s}, [%[pa0]]\n\t\t"
                        "ld1r {%[a1].4s}, [%[pa1]]\n\t\t"
                        "ld1r {%[a2].4s}, [%[pa2]]\n\t\t"
                        "ld1r {%[a3].4s}, [%[pa3]]\n\t\t"
			:[a0]"=w"(vA0) ,[a1]"=w" (vA1),[a2]"=w" (vA2),[a3]"=w"(vA3)
			:[pa0]"r"(pointer0) ,[pa1]"r"(pointer1) ,[pa2]"r"(pointer2) ,[pa3]"r"(pointer3)
		       );
                    vB0 = vld1q_f32(&B[j0+0 ]);
                    vB1 = vld1q_f32(&B[j0+4 ]);
                    vB2 = vld1q_f32(&B[j0+8 ]);
                    vB3 = vld1q_f32(&B[j0+12]);
		}
                for (int k = 0;k < N-1;k++)
                {
                    float* pointer0 = &A[(i0+0)*N+k+1];
                    float* pointer1 = &A[(i0+1)*N+k+1];
                    float* pointer2 = &A[(i0+2)*N+k+1];
                    float* pointer3 = &A[(i0+3)*N+k+1];

                    asm(
                        "ld1r {%[a0].4s}, [%[pa0]]\n\t\t"
                        "ld1r {%[a1].4s}, [%[pa1]]\n\t\t"
                        "ld1r {%[a2].4s}, [%[pa2]]\n\t\t"
                        "ld1r {%[a3].4s}, [%[pa3]]\n\t\t"
			:[a0]"=w"(vA0n) ,[a1]"=w" (vA1n),[a2]"=w" (vA2n),[a3]"=w"(vA3n)
			:[pa0]"r"(pointer0) ,[pa1]"r"(pointer1) ,[pa2]"r"(pointer2) ,[pa3]"r"(pointer3)
		       );
                    vB0n = vld1q_f32(&B[(k+1)*N+j0+0 ]);
                    vB1n = vld1q_f32(&B[(k+1)*N+j0+4 ]);
                    vB2n = vld1q_f32(&B[(k+1)*N+j0+8 ]);
                    vB3n = vld1q_f32(&B[(k+1)*N+j0+12]);

                    asm(
                        "fmla %[c00].4s, %[b0].4s, %[a0].4s\n\t\t"
                        "fmla %[c01].4s, %[b0].4s, %[a1].4s\n\t\t"
                        "fmla %[c02].4s, %[b0].4s, %[a2].4s\n\t\t"
                        "fmla %[c03].4s, %[b0].4s, %[a3].4s\n\t\t"
                        "fmla %[c10].4s, %[b1].4s, %[a0].4s\n\t\t"
                        "fmla %[c11].4s, %[b1].4s, %[a1].4s\n\t\t"
                        "fmla %[c12].4s, %[b1].4s, %[a2].4s\n\t\t"
                        "fmla %[c13].4s, %[b1].4s, %[a3].4s\n\t\t"
                        :[c00]"+w"(vC00),[c01]"+w"(vC01),[c02]"+w"(vC02),[c03]"+w"(vC03) 
		   	,[c10]"+w"(vC10),[c11]"+w"(vC11),[c12]"+w"(vC12),[c13]"+w"(vC13) 
                        :[b0]"w"(vB0),[b1]"w"(vB1)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
		       );
		    asm(
                        "fmla %[c20].4s, %[b2].4s, %[a0].4s\n\t\t"
                        "fmla %[c21].4s, %[b2].4s, %[a1].4s\n\t\t"
                        "fmla %[c22].4s, %[b2].4s, %[a2].4s\n\t\t"
                        "fmla %[c23].4s, %[b2].4s, %[a3].4s\n\t\t"
                        "fmla %[c30].4s, %[b3].4s, %[a0].4s\n\t\t"
                        "fmla %[c31].4s, %[b3].4s, %[a1].4s\n\t\t"
                        "fmla %[c32].4s, %[b3].4s, %[a2].4s\n\t\t"
                        "fmla %[c33].4s, %[b3].4s, %[a3].4s\n\t\t"
                        :[c20]"+w"(vC20),[c21]"+w"(vC21),[c22]"+w"(vC22),[c23]"+w"(vC23)
			,[c30]"+w"(vC30),[c31]"+w"(vC31),[c32]"+w"(vC32),[c33]"+w"(vC33)
                        :[b2]"w"(vB2),[b3]"w"(vB3)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
                    );
		    vA0 = vA0n;vB0 = vB0n;
		    vA1 = vA1n;vB1 = vB1n;
		    vA2 = vA2n;vB2 = vB2n;
		    vA3 = vA3n;vB3 = vB3n;
                }
		{
		    // last loop
                    asm(
                        "fmla %[c00].4s, %[b0].4s, %[a0].4s\n\t\t"
                        "fmla %[c01].4s, %[b0].4s, %[a1].4s\n\t\t"
                        "fmla %[c02].4s, %[b0].4s, %[a2].4s\n\t\t"
                        "fmla %[c03].4s, %[b0].4s, %[a3].4s\n\t\t"
                        "fmla %[c10].4s, %[b1].4s, %[a0].4s\n\t\t"
                        "fmla %[c11].4s, %[b1].4s, %[a1].4s\n\t\t"
                        "fmla %[c12].4s, %[b1].4s, %[a2].4s\n\t\t"
                        "fmla %[c13].4s, %[b1].4s, %[a3].4s\n\t\t"
                        :[c00]"+w"(vC00),[c01]"+w"(vC01),[c02]"+w"(vC02),[c03]"+w"(vC03) 
		   	,[c10]"+w"(vC10),[c11]"+w"(vC11),[c12]"+w"(vC12),[c13]"+w"(vC13) 
                        :[b0]"w"(vB0),[b1]"w"(vB1)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
		       );
		    asm(
                        "fmla %[c20].4s, %[b2].4s, %[a0].4s\n\t\t"
                        "fmla %[c21].4s, %[b2].4s, %[a1].4s\n\t\t"
                        "fmla %[c22].4s, %[b2].4s, %[a2].4s\n\t\t"
                        "fmla %[c23].4s, %[b2].4s, %[a3].4s\n\t\t"
                        "fmla %[c30].4s, %[b3].4s, %[a0].4s\n\t\t"
                        "fmla %[c31].4s, %[b3].4s, %[a1].4s\n\t\t"
                        "fmla %[c32].4s, %[b3].4s, %[a2].4s\n\t\t"
                        "fmla %[c33].4s, %[b3].4s, %[a3].4s\n\t\t"
                        :[c20]"+w"(vC20),[c21]"+w"(vC21),[c22]"+w"(vC22),[c23]"+w"(vC23)
			,[c30]"+w"(vC30),[c31]"+w"(vC31),[c32]"+w"(vC32),[c33]"+w"(vC33)
                        :[b2]"w"(vB2),[b3]"w"(vB3)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
                    );
		}
                vst1q_f32(C+(i0+0)*N+j0+0,  vC00);
                vst1q_f32(C+(i0+0)*N+j0+4,  vC01);
                vst1q_f32(C+(i0+0)*N+j0+8,  vC02);
                vst1q_f32(C+(i0+0)*N+j0+12, vC03);
                vst1q_f32(C+(i0+1)*N+j0+0,  vC10);
                vst1q_f32(C+(i0+1)*N+j0+4,  vC11);
                vst1q_f32(C+(i0+1)*N+j0+8,  vC12);
                vst1q_f32(C+(i0+1)*N+j0+12, vC13);
                vst1q_f32(C+(i0+2)*N+j0+0,  vC20);
                vst1q_f32(C+(i0+2)*N+j0+4,  vC21);
                vst1q_f32(C+(i0+2)*N+j0+8,  vC22);
                vst1q_f32(C+(i0+2)*N+j0+12, vC23);
                vst1q_f32(C+(i0+3)*N+j0+0,  vC30);
                vst1q_f32(C+(i0+3)*N+j0+4,  vC31);
                vst1q_f32(C+(i0+3)*N+j0+8,  vC32);
                vst1q_f32(C+(i0+3)*N+j0+12, vC33);
            }
            break;
        case 9:
            std::cout << "9999" << std::endl;
            for (int i0 = 0;i0 < N;i0 += 4)
            for (int j0 = 0;j0 < N;j0 += 20)
            {
                float32x4_t vC00 = vdupq_n_f32(0);
                float32x4_t vC01 = vdupq_n_f32(0);
                float32x4_t vC02 = vdupq_n_f32(0);
                float32x4_t vC03 = vdupq_n_f32(0);
                float32x4_t vC10 = vdupq_n_f32(0);
                float32x4_t vC11 = vdupq_n_f32(0);
                float32x4_t vC12 = vdupq_n_f32(0);
                float32x4_t vC13 = vdupq_n_f32(0);
                float32x4_t vC20 = vdupq_n_f32(0);
                float32x4_t vC21 = vdupq_n_f32(0);
                float32x4_t vC22 = vdupq_n_f32(0);
                float32x4_t vC23 = vdupq_n_f32(0);
                float32x4_t vC30 = vdupq_n_f32(0);
                float32x4_t vC31 = vdupq_n_f32(0);
                float32x4_t vC32 = vdupq_n_f32(0);
                float32x4_t vC33 = vdupq_n_f32(0);
                float32x4_t vC40 = vdupq_n_f32(0);
                float32x4_t vC41 = vdupq_n_f32(0);
                float32x4_t vC42 = vdupq_n_f32(0);
                float32x4_t vC43 = vdupq_n_f32(0);
	        float32x4_t vA0, vA1, vA2, vA3;
	        float32x4_t vB0, vB1, vB2, vB3;
		float32x4_t vA0n, vA1n, vA2n, vA3n;
                float32x4_t vB0n, vB1n, vB2n, vB3n;
		// pre-load
		{
                    float* pointer0 = &A[(i0+0)*N];
                    float* pointer1 = &A[(i0+1)*N];
                    float* pointer2 = &A[(i0+2)*N];
                    float* pointer3 = &A[(i0+3)*N];

                    asm(
                        "ld1r {%[a0].4s}, [%[pa0]]\n\t\t"
                        "ld1r {%[a1].4s}, [%[pa1]]\n\t\t"
                        "ld1r {%[a2].4s}, [%[pa2]]\n\t\t"
                        "ld1r {%[a3].4s}, [%[pa3]]\n\t\t"
			:[a0]"=w"(vA0) ,[a1]"=w" (vA1),[a2]"=w" (vA2),[a3]"=w"(vA3)
			:[pa0]"r"(pointer0) ,[pa1]"r"(pointer1) ,[pa2]"r"(pointer2) ,[pa3]"r"(pointer3)
		       );
                    vB0 = vld1q_f32(&B[j0+0 ]);
                    vB1 = vld1q_f32(&B[j0+4 ]);
                    vB2 = vld1q_f32(&B[j0+8 ]);
                    vB3 = vld1q_f32(&B[j0+12]);
		}
                for (int k = 0;k < N-1;k++)
                {
                    float* pointer0 = &A[(i0+0)*N+k+1];
                    float* pointer1 = &A[(i0+1)*N+k+1];
                    float* pointer2 = &A[(i0+2)*N+k+1];
                    float* pointer3 = &A[(i0+3)*N+k+1];

                    asm(
                        "ld1r {%[a0].4s}, [%[pa0]]\n\t\t"
                        "ld1r {%[a1].4s}, [%[pa1]]\n\t\t"
                        "ld1r {%[a2].4s}, [%[pa2]]\n\t\t"
                        "ld1r {%[a3].4s}, [%[pa3]]\n\t\t"
			:[a0]"=w"(vA0n) ,[a1]"=w" (vA1n),[a2]"=w" (vA2n),[a3]"=w"(vA3n)
			:[pa0]"r"(pointer0) ,[pa1]"r"(pointer1) ,[pa2]"r"(pointer2) ,[pa3]"r"(pointer3)
		       );
                    vB0n = vld1q_f32(&B[(k+1)*N+j0+0 ]);
                    vB1n = vld1q_f32(&B[(k+1)*N+j0+4 ]);
                    vB2n = vld1q_f32(&B[(k+1)*N+j0+8 ]);
                    vB3n = vld1q_f32(&B[(k+1)*N+j0+12]);

                    asm(
                        "fmla %[c00].4s, %[b0].4s, %[a0].4s\n\t\t"
                        "fmla %[c01].4s, %[b0].4s, %[a1].4s\n\t\t"
                        "fmla %[c02].4s, %[b0].4s, %[a2].4s\n\t\t"
                        "fmla %[c03].4s, %[b0].4s, %[a3].4s\n\t\t"
                        "fmla %[c10].4s, %[b1].4s, %[a0].4s\n\t\t"
                        "fmla %[c11].4s, %[b1].4s, %[a1].4s\n\t\t"
                        "fmla %[c12].4s, %[b1].4s, %[a2].4s\n\t\t"
                        "fmla %[c13].4s, %[b1].4s, %[a3].4s\n\t\t"
                        :[c00]"+w"(vC00),[c01]"+w"(vC01),[c02]"+w"(vC02),[c03]"+w"(vC03) 
		   	,[c10]"+w"(vC10),[c11]"+w"(vC11),[c12]"+w"(vC12),[c13]"+w"(vC13) 
                        :[b0]"w"(vB0),[b1]"w"(vB1)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
		       );
		    asm(
                        "fmla %[c20].4s, %[b2].4s, %[a0].4s\n\t\t"
                        "fmla %[c21].4s, %[b2].4s, %[a1].4s\n\t\t"
                        "fmla %[c22].4s, %[b2].4s, %[a2].4s\n\t\t"
                        "fmla %[c23].4s, %[b2].4s, %[a3].4s\n\t\t"
                        "fmla %[c30].4s, %[b3].4s, %[a0].4s\n\t\t"
                        "fmla %[c31].4s, %[b3].4s, %[a1].4s\n\t\t"
                        "fmla %[c32].4s, %[b3].4s, %[a2].4s\n\t\t"
                        "fmla %[c33].4s, %[b3].4s, %[a3].4s\n\t\t"
                        :[c20]"+w"(vC20),[c21]"+w"(vC21),[c22]"+w"(vC22),[c23]"+w"(vC23)
			,[c30]"+w"(vC30),[c31]"+w"(vC31),[c32]"+w"(vC32),[c33]"+w"(vC33)
                        :[b2]"w"(vB2),[b3]"w"(vB3)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
                    );
		    vA0 = vA0n;vB0 = vB0n;
		    vA1 = vA1n;vB1 = vB1n;
		    vA2 = vA2n;vB2 = vB2n;
		    vA3 = vA3n;vB3 = vB3n;
                }
		{
		    // last loop
                    asm(
                        "fmla %[c00].4s, %[b0].4s, %[a0].4s\n\t\t"
                        "fmla %[c01].4s, %[b0].4s, %[a1].4s\n\t\t"
                        "fmla %[c02].4s, %[b0].4s, %[a2].4s\n\t\t"
                        "fmla %[c03].4s, %[b0].4s, %[a3].4s\n\t\t"
                        "fmla %[c10].4s, %[b1].4s, %[a0].4s\n\t\t"
                        "fmla %[c11].4s, %[b1].4s, %[a1].4s\n\t\t"
                        "fmla %[c12].4s, %[b1].4s, %[a2].4s\n\t\t"
                        "fmla %[c13].4s, %[b1].4s, %[a3].4s\n\t\t"
                        :[c00]"+w"(vC00),[c01]"+w"(vC01),[c02]"+w"(vC02),[c03]"+w"(vC03) 
		   	,[c10]"+w"(vC10),[c11]"+w"(vC11),[c12]"+w"(vC12),[c13]"+w"(vC13) 
                        :[b0]"w"(vB0),[b1]"w"(vB1)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
		       );
		    asm(
                        "fmla %[c20].4s, %[b2].4s, %[a0].4s\n\t\t"
                        "fmla %[c21].4s, %[b2].4s, %[a1].4s\n\t\t"
                        "fmla %[c22].4s, %[b2].4s, %[a2].4s\n\t\t"
                        "fmla %[c23].4s, %[b2].4s, %[a3].4s\n\t\t"
                        "fmla %[c30].4s, %[b3].4s, %[a0].4s\n\t\t"
                        "fmla %[c31].4s, %[b3].4s, %[a1].4s\n\t\t"
                        "fmla %[c32].4s, %[b3].4s, %[a2].4s\n\t\t"
                        "fmla %[c33].4s, %[b3].4s, %[a3].4s\n\t\t"
                        :[c20]"+w"(vC20),[c21]"+w"(vC21),[c22]"+w"(vC22),[c23]"+w"(vC23)
			,[c30]"+w"(vC30),[c31]"+w"(vC31),[c32]"+w"(vC32),[c33]"+w"(vC33)
                        :[b2]"w"(vB2),[b3]"w"(vB3)
			,[a0]"w"(vA0),[a1]"w"(vA1),[a2]"w"(vA2),[a3]"w"(vA3)
                    );
		}
                vst1q_f32(C+(i0+0)*N+j0+0,  vC00);
                vst1q_f32(C+(i0+0)*N+j0+4,  vC01);
                vst1q_f32(C+(i0+0)*N+j0+8,  vC02);
                vst1q_f32(C+(i0+0)*N+j0+12, vC03);
                vst1q_f32(C+(i0+1)*N+j0+0,  vC10);
                vst1q_f32(C+(i0+1)*N+j0+4,  vC11);
                vst1q_f32(C+(i0+1)*N+j0+8,  vC12);
                vst1q_f32(C+(i0+1)*N+j0+12, vC13);
                vst1q_f32(C+(i0+2)*N+j0+0,  vC20);
                vst1q_f32(C+(i0+2)*N+j0+4,  vC21);
                vst1q_f32(C+(i0+2)*N+j0+8,  vC22);
                vst1q_f32(C+(i0+2)*N+j0+12, vC23);
                vst1q_f32(C+(i0+3)*N+j0+0,  vC30);
                vst1q_f32(C+(i0+3)*N+j0+4,  vC31);
                vst1q_f32(C+(i0+3)*N+j0+8,  vC32);
                vst1q_f32(C+(i0+3)*N+j0+12, vC33);
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
