cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_REQUIRED_DEFINITIONS  "-march=armv8.2-a+fp16")
INCLUDE(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES("#include <arm_neon.h>
    int main()
    {
        float16x8_t a, b, c;
        c = vfmaq_f16(a, b, c);
        return 0;
    }" HAVE_V82_FP16)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
if(HAVE_V82_FP16)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_ASIMDHP=1 -march=armv8.2-a+fp16")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_ASIMDHP=0")
endif(HAVE_V82_FP16)
add_executable(gemmNeon main.cpp kernel.cpp)


