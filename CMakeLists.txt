cmake_minimum_required(VERSION 4.2)
project(ScorpysCBCBallisticCalculator C)

set(CMAKE_C_STANDARD 17)

add_executable(ScorpysCBCBallisticCalculator main.c
        calculations.c
        calculations.h
        structs.h)