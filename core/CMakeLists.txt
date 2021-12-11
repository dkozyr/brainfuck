cmake_minimum_required(VERSION 3.10)
project(core CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS " -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h)

find_package(Boost REQUIRED iostreams)
include_directories(${BOOST_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::iostreams operand_writer)
