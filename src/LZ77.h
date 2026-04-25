#pragma once
#include <stdio.h>
#include <stddef.h> 
#include <stdint.h>

uint64_t LZ77_ENCODE(const char* src, uint64_t size, char* dst);

uint64_t LZ77_DECODE(const char* src, uint64_t size, char* dst);