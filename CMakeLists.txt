cmake_minimum_required(VERSION 3.10)

project (main VERSION 1.0)

add_subdirectory(spiral output/spiral)
add_subdirectory(PathOram output/PathOram)

find_package(SEAL REQUIRED PATHS "/home/$ENV{USER}/local" NO_DEFAULT_PATH)
find_package(OpenMP REQUIRED)
find_package(OpenSSL REQUIRED)
set(CMAKE_CXX_FLAGS "-g -O3 -std=c++17 -MD -march=native")
link_libraries( -lcrypto -lpthread -lcryptopp)
link_directories(/usr/lib)

include_directories(./cryptopp/install_x86_64/include "${CMAKE_CURRENT_SOURCE_DIR}/spiral" ${OPENSSL_INCLUDE_DIR})
link_directories(./cryptopp/install_x86_64/lib "${CMAKE_CURRENT_SOURCE_DIR}/spiral")
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")


add_executable(main ${SOURCES})
# main.cpp utils.cpp KVclient.cpp KVserver.cpp DBclient.cpp DBserver.cpp FakePIR.cpp PIRANA_client.cc PIRANA_server.cc PIRANA_pir_parms.cc PIRANA_utils.cc
target_include_directories(main PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/sux/sux/function
    
)

target_link_libraries(main PUBLIC  SEAL::seal OpenMP::OpenMP_CXX cryptopp spiral ORAM ${OPENSSL_LIBRARIES})
target_compile_features(main PRIVATE cxx_std_20)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
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
#     TEXP=2
#     TEXPRIGHT=56
#     TCONV=4
#     TGSW=4
#     QPBITS=14
#     PVALUE=4
#     QNUMFIRST=1
#     QNUMREST=0
#     OUTN=2
#     DESC="the scale is 2^5, 2^3 *2^2 *2048 Byte"
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
#     DESC="the scale is 2^7, 2^4*2^3 *2048 Byte"
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
#     DESC="the scale is 2^13, 2^7 *2^6 *4096 Byte" #2^7*2^4*4096 Byte #2^7*2^5*4096 Byte #2^7*2^3*4096 Byte
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


# enable_testing()
# find_package(Catch2 3 REQUIRED)
# add_test(NAME my_test COMMAND test_pir)
# add_executable(test_pir "./Test/test.cpp" "./src/utils.cpp" "./src/Spiral_Instance.cpp" "./src/KVclient.cpp" "./src/KVserver.cpp" "./src/DBserver.cpp" "./src/Spiral_Instance.cpp" "./src/FakePIR.cpp" "./src/PIRANA_client.cc" "./src/PIRANA_server.cc" "./src/PIRANA_pir_parms.cc" "./src/PIRANA_utils.cc" "./src/oram_wrapper.cpp")
# target_include_directories(test_pir PUBLIC 
#     ${CMAKE_CURRENT_SOURCE_DIR}/include 
#     ${CMAKE_CURRENT_SOURCE_DIR}/sux/sux/function
#     ./cryptopp/install_x86_64/include
#     ${CMAKE_CURRENT_SOURCE_DIR}/spiral
# )
# target_link_libraries(test_pir SEAL::seal OpenMP::OpenMP_CXX cryptopp spiral Catch2::Catch2WithMain ORAM)
# target_compile_features(test_pir PRIVATE cxx_std_20)

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")