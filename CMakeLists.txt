cmake_minimum_required(VERSION 3.29)
project(SAE_L1 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(common)

add_executable(SAE_L1
    common/file.c
    main.c)
