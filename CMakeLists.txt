cmake_minimum_required(VERSION 3.0.0)
project(BPlusTree VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. DIRSRC)
add_executable(BPlusTree ${DIRSRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
