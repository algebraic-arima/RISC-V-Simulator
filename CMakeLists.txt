cmake_minimum_required(VERSION 3.22)
project(code)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(src)

add_executable(code
        include/lsb.h
        src/lsb.cpp
        include/decoder.h
        include/memory.h
        include/param.h
        src/memory.cpp
        src/decoder.cpp
        include/utils.h
        src/regfile.cpp
        include/regfile.h
        include/alu.h
        src/alu.cpp
        include/state.h
        include/rss.h
        include/rob.h
        src/rob.cpp)
