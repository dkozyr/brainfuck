cmake_minimum_required(VERSION 3.10)
project(operand-writer-tests)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} operand_writer)
