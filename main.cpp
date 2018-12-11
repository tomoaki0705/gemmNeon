#include <iostream>
#include <cstdlib>
#include <arm_neon.h>
#include <chrono>

int main(int argc, char** argv)
{
    int method = 0;
    if (2 <= argc)
        method = std::min(std::max(atoi(argv[1]), 0), 0);
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
    }
    
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout << "time :" << elapsed << "[ms]" << std::endl;
    delete [] A;
    delete [] B;
    delete [] C;
    return 0;
}
