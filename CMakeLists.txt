cmake_minimum_required(VERSION 3.17)
project(csc3050Pro1)

set(CMAKE_CXX_STANDARD 14)

add_executable(assembler main.cpp simulator.h _assembler.cpp _assembler.h simulator.cpp MIPS_opr.h)