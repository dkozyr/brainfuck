cmake_minimum_required(VERSION 3.10)
project(operand_writer ASM_NASM)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")

set_source_files_properties(operand_writer.asm PROPERTIES LANGUAGE ASM_NASM)

add_library(${PROJECT_NAME} operand_writer.asm)
