cmake_minimum_required(VERSION 3.2)
# Enable C++11
set(CMAKE_CXX_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE -Ofast)
 
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")
project(ORAM)
find_package(OpenSSL REQUIRED)
find_package(OpenMP REQUIRED)
#find_package(Catch2 REQUIRED)
message(${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} )
file(GLOB ORAM_FILES
 "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp")
add_library(ORAM ${ORAM_FILES})
target_include_directories(ORAM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ORAM ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX)
# include(CTest)
# include(Catch)
# catch_discover_tests(ORAM
#   TEST_PREFIX "Test_"
  
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
#   TEST_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/tests
# )