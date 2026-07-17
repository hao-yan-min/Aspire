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


