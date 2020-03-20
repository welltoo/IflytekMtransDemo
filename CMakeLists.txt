cmake_minimum_required(VERSION 3.10)
project(xfyunfy)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -Wsign-compare")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -Wsign-compare")

set(POCO_PATH "/home/xuxintao/iflytek/poco-1.10.0-all")
set(POCO_LIB_PATH "/usr/local/lib")

include_directories(
        ${POCO_PATH}/Net/include
        ${POCO_PATH}/NetSSL_OpenSSL/include
        ${POCO_PATH}/Foundation/include
        ${POCO_PATH}/Crypto/include
        ${POCO_PATH}/JSON/include
)
link_directories(${POCO_LIB_PATH})

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH ${POCO_LIB_PATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(xfyunfy xfyunfy.cpp)
target_link_libraries(xfyunfy libPocoNet.so libPocoFoundation.so libPocoCrypto.so libPocoJSON.so)
