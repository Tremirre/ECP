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


file(GLOB TEST_FILES tests/test*.cpp)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${SOURCES})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

