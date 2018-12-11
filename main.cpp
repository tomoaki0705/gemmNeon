#include <iostream>
#include <cstdlib>
#include <arm_neon.h>
#include <chrono>
#include <cstring>
#include <vector>
#include <algorithm>

int main(int argc, char** argv)
{
    int method = 0;
    if (2 <= argc)
        method = std::min(std::max(atoi(argv[1]), 0), 1);
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
    return 0;
}
