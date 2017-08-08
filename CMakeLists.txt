cmake_minimum_required(VERSION 3.7)
project(PDMAssembler)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c instruction.h parser.h parser.c vector.c vector.h hashtable.h hashtable.c)
add_executable(PDMAssembler ${SOURCE_FILES})