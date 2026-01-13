cmake_minimum_required(VERSION 3.12)

set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_C_COMPILER clang++-12)
# set(CMAKE_CXX_COMPILER clang++-12)

project(spiral VERSION 0.1 LANGUAGES CXX)

# CXX options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -O3 -std=c++17 -MD -march=native")
find_package(OpenMP REQUIRED)
option(NOAVX512 "Disable AVX-512" OFF)
option(NOAVX2 "Disable AVX2" OFF)
if (NOAVX512)
    message("Disabling AVX-512")
    add_compile_options(-mno-avx512f)
endif()
if (NOAVX2)
    message("Disabling AVX2")
    add_compile_options(-mno-avx2)
endif()
if (NOCRT)
    message("Disabling CRT")
    add_compile_options(-DNOCRT=1)
endif()

# main target
set(SRC_FILES src/spiral.cpp src/core.cpp src/constants.cpp src/poly.cpp src/util.cpp src/client.cpp src/testing.cpp )
# set(SRC_FILES src/spiral.cpp src/core.cpp src/constants.cpp src/poly.cpp src/util.cpp src/client.cpp src/testing.cpp src/main.cpp)
set(HEADER_FILES include/spiral.h include/core.h include/constants.h include/poly.h include/spiral_util.h include/spiral_client.h include/testing.h)
# add_executable(spiral ${SRC_FILES} ${HEADER_FILES})
add_library(spiral ${SRC_FILES} ${HEADER_FILES})
target_include_directories(spiral PUBLIC include)
# target_include_directories(spiral PUBLIC /home/$ENV{USER}/vcpkg/installed/)


# Must specify:
# -DCMAKE_TOOLCHAIN_FILE=/home/ubuntu/vcpkg/scripts/buildsystems/vcpkg.cmake

find_package(HEXL CONFIG REQUIRED PATHS "/home/$ENV{USER}/local" NO_DEFAULT_PATH)
# 打印HEXL的位置信息
message(STATUS "HEXL found at: ${HEXL_DIR}")
target_link_libraries(spiral PUBLIC HEXL::hexl OpenMP::OpenMP_CXX)
#target_link_libraries(spiral PUBLIC HEXL::hexl)
#2^11*2048byte
# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^11, 2^6 *2^5 *2048Byte"
# )
#2^12*2048byte
# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^12, 2^6 *2^6 *2048Byte"
# )
# #2^13*2048byte
# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^13, 2^6 *2^7 *2048Byte"
# )
# #2^14*2048byte
# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^14, 2^6 *2^8 *2048Byte"
# )
# add_compile_definitions(
#     TEXP=8
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=9
#     QPBITS=21
#     PVALUE=256
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^15, 2^8 *2^7 *8192Byte"
# )
# add_compile_definitions(
#     TEXP=2
#     TEXPRIGHT=56
#     TCONV=16
#     TGSW=4
#     QPBITS=15
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^9, 2^5 *2^4 *2048 Byte"
# )

# add_compile_definitions(
#     TEXP=2
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^9, 2^4 *2^4 *2048 Byte"
# )
# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^13, 2^7 *2^6 *2048 Byte"
# )

# add_compile_definitions(
#     TEXP=8
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=8
#     QPBITS=21
#     PVALUE=256
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^13, 2^8 *2^5 *8192 Byte"
# )

# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=5
#     QPBITS=16
#     PVALUE=16
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^13, 2^7 *2^6 *4096 Byte"#2^6*2^6*4096 Byte
# )

add_compile_definitions(
    TEXP=$(TEXP)
    TEXPRIGHT=$(TEXPRIGHT)
    TCONV=$(TCONV)
    TGSW=$(TGSW)
    QPBITS=$(QPBITS)
    PVALUE=$(PVALUE)
    QNUMFIRST=$(QNUMFIRST)
    QNUMREST=$(QNUMREST)
    OUTN=$(OUTN)
)
# add_compile_definitions(
#     TEXP=4
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^11, 2^6 *2^5 *2048Byte"
# )