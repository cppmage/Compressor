#pragma once
#include "Huffman_Internal_Includes.h"

const uint8_t* read_header(const uint8_t* src, struct Data* map, 
                           int16_t table[][1<<MAX_CODE_LEN], uint64_t* bits);