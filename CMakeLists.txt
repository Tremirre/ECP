cmake_minimum_required(VERSION 3.5.2)
project(ECP VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(SOURCES
    src/main.cpp
    src/argparser.cpp
    src/node.cpp
    src/random.cpp
    src/solvers.cpp
)

include_directories(
    include
)
include(CPack)

add_executable(ECP ${SOURCES})
