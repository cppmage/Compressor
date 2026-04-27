
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
    memset(LZ77_DECODED, '1', sizeof(LZ77_DECODED));

    uint64_t out1 = LZ77_ENCODE(text, strlen(text), LZ77_ENCODED);
    uint64_t out2 = huffman_encode((uint8_t*)LZ77_ENCODED, out1, (uint8_t*)HUFFMAN_ENCODED);
    uint64_t out3 = huffman_decode((uint8_t*)HUFFMAN_ENCODED, out2, (uint8_t*)HUFFMAN_DECODED);
    uint64_t out4 = LZ77_DECODE(HUFFMAN_DECODED, out3, LZ77_DECODED);
    LZ77_DECODED[out4]='\0';

    printf("Og text: %s\n", text);
    printf("Encoded: %s\n", HUFFMAN_ENCODED);
    printf("Decoded: %s\n", LZ77_DECODED);

    FILE* f = fopen("text.txt", "r");

    if(f==NULL){
        return 1;
    }
    char buffer[255];
    while(fgets(buffer, 255, f)){
        printf("%s\n", buffer);
    }
    fclose(f);
    return 0;
}