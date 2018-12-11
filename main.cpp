#include <iostream>
#include <cstdlib>
#include <arm_neon.h>
#include <chrono>

int main(int argc, char** argv)
{
    int method = 0;
    if (2 <= argc)
        method = std::min(std::max(atoi(argv[1]), 0), 1);
    int N = 512;
    if (3 <= argc)
        N = std::max(atoi(argv[2]), 8);

    std::cout << "type: " << method << std::endl;
    std::cout << "size: " << N << "x" << N << std::endl; 
    float *A = new float[N*N];
    float *B = new float[N*N];
    float *C = new float[N*N];
    auto start = std::chrono::system_clock::now();
    switch(method)
    {
    case 0:
    default:
        for (int i = 0;i < N;i++)
        for (int j = 0;j < N;j++)
        for (int k = 0;k < N;k++)
            C[i*N+j] += A[i*N+k] + B[k*N+j];
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
    }
    
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "time :" << elapsed << "[ms]" << std::endl;
    delete [] A;
    delete [] B;
    delete [] C;
    return 0;
}
