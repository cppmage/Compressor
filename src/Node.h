#pragma  once
#include <stdint.h>
#include <stdbool.h>

#define MAX_SYMBOLS 256
#define MAX_NODES (2 * MAX_SYMBOLS - 1)

struct Node{
    unsigned char c;
    int64_t freq;
    struct Node* left;
    struct Node* right;
};
struct Data{
    uint64_t code;
    uint8_t len;
};  
