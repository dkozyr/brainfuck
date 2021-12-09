cmake_minimum_required(VERSION 3.10)
project(brainfuck CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS " -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(EXAMPLES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/examples\")

find_package(Boost REQUIRED iostreams)
include_directories(${BOOST_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::iostreams operand_writer)

add_subdirectory(operand_writer)
add_subdirectory(tests)
