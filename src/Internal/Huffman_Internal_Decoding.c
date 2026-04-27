#include "Huffman_Internal_Decoding.h"

const uint8_t* read_header(const uint8_t* src, struct Data* map, 
                           int16_t table[][1<<MAX_CODE_LEN], uint64_t* bits) {
    int active_fields = 0;
    int advance;
    sscanf((char*)src, "%d %llu%n", &active_fields, (unsigned long long*)bits, &advance);
    src+=advance;

    for (int i = 0; i < active_fields; i++) {
        int symbol;
        uint64_t code;
        uint8_t len;
        
        sscanf((char*)src, "%d %llu %hhu%n", &symbol, (unsigned long long*)&code, &len, &advance);
        src += advance;
        
        map[symbol].code = code;
        map[symbol].len = len;

        table[len][code]=symbol;
    }
    while(1){
        if(*src=='|'){
            src++;
            break;
        }
        src++;
    }
    
    return src;
}