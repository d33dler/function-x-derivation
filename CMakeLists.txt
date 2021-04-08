cmake_minimum_required(VERSION 3.15)
project(Code)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(scanner.c
        recognizeExp.c
        infixExp.c
        evalExp.c
        mainInfix.c
        scanner.c)
