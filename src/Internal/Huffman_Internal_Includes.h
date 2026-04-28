#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Node.h"
#include "../Node_Heap.h"

#define MAX_CODE_LEN 12

#pragma pack(push, 1)
struct header_package{
    uint32_t freq;
    unsigned char c;
};
#pragma pack(pop)