#pragma once
#include <stdio.h>
#include <stddef.h> 
#include <stdint.h>

uint64_t huffman_encode(const uint8_t* src, uint64_t size, uint8_t* dst);

uint64_t huffman_decode(const uint8_t* src, uint64_t size, uint8_t* dst);