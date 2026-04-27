
#include <stdint.h>
#include <stdio.h>
#include <stddef.h> 
#include <string.h>
#include "LZ77.h"
#include "Node_Heap.h"
#include "Huffman.h"

int main(){

    const char* text = "abracadabra hello world hehehehe aaaaaaaaa lalalalalalalalala 1212121314919083718378138";
   
    char LZ77_ENCODED[1024];
    char HUFFMAN_ENCODED[1024];
    char HUFFMAN_DECODED[1024];
    char LZ77_DECODED[1024];
    memset(LZ77_ENCODED, '\0', sizeof(LZ77_ENCODED));
    memset(HUFFMAN_ENCODED, '\0', sizeof(HUFFMAN_ENCODED));
    memset(HUFFMAN_DECODED, '\0', sizeof(HUFFMAN_DECODED));
    memset(LZ77_DECODED, '\0', sizeof(LZ77_DECODED));

    LZ77_ENCODE(text, strlen(text), LZ77_ENCODED);
    huffman_encode((uint8_t*)LZ77_ENCODED, strlen(LZ77_ENCODED), (uint8_t*)HUFFMAN_ENCODED);
    huffman_decode((uint8_t*)HUFFMAN_ENCODED, strlen(HUFFMAN_ENCODED), (uint8_t*)HUFFMAN_DECODED);
    LZ77_DECODE(HUFFMAN_DECODED, strlen(HUFFMAN_DECODED), LZ77_DECODED);
    
    return 0;
}