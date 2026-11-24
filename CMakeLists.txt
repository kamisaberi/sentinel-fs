cmake_minimum_required(VERSION 3.20)
project(sentinel-fs VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenSSL REQUIRED)

include_directories(include)

add_library(sentinelfs SHARED
    src/PathResolver.cpp
    src/CasStore.cpp
    src/FsLock.cpp
)

target_link_libraries(sentinelfs PUBLIC OpenSSL::Crypto)

target_include_directories(sentinelfs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(sfs tools/sfs.cpp)
target_link_libraries(sfs PRIVATE sentinelfs)

install(TARGETS sentinelfs sfs
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)