cmake_minimum_required(VERSION 3.14)

project(RiscV_Archiver C)

#set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS_RELEASE "-O3 -march=rv64gcv -mabi=lp64d")


add_executable(archiver src/main.c src/LZ77.c src/Huffman.c src/Node_Heap.c src/Internal/Huffman_Internal_Encoding.c src/Internal/Huffman_Internal_Decoding.c
    src/Internal/Pipeline_Includes.c 
    src/Worker.c src/Linker.c)

target_include_directories(archiver PRIVATE 
    src
    src/Internal
)