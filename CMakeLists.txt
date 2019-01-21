cmake_minimum_required(VERSION 2.8)
project(sasm C)

if(WIN32)
    add_definitions(-DWIN=1)
elseif(UNIX)
    add_definitions(-DUNIX=1)
    set(CMAKE_C_FLAGS "-Wdiscarded-qualifiers")
endif()

set(CMAKE_C_STANDARD 99)

add_executable(sasm src/main.c src/asm.h src/asm.c src/util.h src/util.c)