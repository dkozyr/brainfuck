cmake_minimum_required(VERSION 3.10)
project(brainfuck CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED iostreams)
include_directories(${BOOST_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} Compiler.cpp main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::iostreams operand_writer)

add_subdirectory(operand_writer)
