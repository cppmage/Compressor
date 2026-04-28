#include "Huffman_Internal_Decoding.h"
#include "Huffman_Internal_Encoding.h"
#include "Huffman_Internal_Includes.h"
#include <string.h>

const uint8_t* read_header(const uint8_t* src, struct Node* arr) {
    
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        arr[i].c = (unsigned char)i;
        arr[i].freq = 0;
        arr[i].left = arr[i].right = NULL;
    }

    int active_fields = 0;
    memcpy(&active_fields, src, sizeof(int));
    src+=sizeof(int);
    struct header_package pkg;
    
    for (int i = 0; i < active_fields; i++) {
        memcpy(&pkg, src, sizeof(struct header_package));
        src+=sizeof(struct header_package);
        arr[pkg.c].freq=pkg.freq;
    }

    
    
    return src;
}