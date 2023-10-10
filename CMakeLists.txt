cmake_minimum_required(VERSION 3.5.2)
project(ECP VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(TEST_DIRECTORY tests)

set(SOURCES
    src/argparser.cpp
    src/node.cpp
    src/random.cpp
    src/solvers.cpp
)

include_directories(
    include
)
include(CPack)

add_executable(ECP
    src/main.cpp
    ${SOURCES}
)

add_executable(test_random
    tests/test_random.cpp
    ${SOURCES}
)

add_executable(test_node
    tests/test_node.cpp
    ${SOURCES}
)

add_executable(test_solvers
    tests/test_solvers.cpp
    ${SOURCES}
)

add_test(TestRandom ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_random)
add_test(TestNode ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_node)
add_test(TestSolvers ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_solvers)
