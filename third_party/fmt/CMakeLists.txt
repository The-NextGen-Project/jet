cmake_minimum_required(VERSION 3.0)

include_directories(../fmt/include)

add_library(fmt
        src/format.cc
        src/os.cc
)