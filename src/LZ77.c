#include "LZ77.h"
#include <stdio.h>

#define WINDOW_SIZE 1024
#define MAX_LEN 1024
#define MIN_MATCH 1

char* write_token(char* dst, int offset, int len, char symbol){
    if (offset == 0 && len == 0) {
        dst += sprintf(dst, "L%c", symbol);
    } else {
        dst += sprintf(dst, "R%d,%d,%c", offset, len, symbol);
    }
    return dst;
}

uint64_t LZ77_ENCODE(const char* src, uint64_t size, char* dst){
    int dst_i = 0;
    int pos = 0;
    char* start = dst;

    while(pos<size){
        int max_len = 0, max_offset = 0;
        
        for(int offset = 1; offset<=pos && offset<=WINDOW_SIZE; offset++){
            int len = 0;
            while(len<MAX_LEN && pos+len<size && src[pos-offset+len]==src[pos+len]){
                len++;
            }
            if(len>max_len){
                max_len = len;
                max_offset = offset;
            }
        }

        if(max_offset>=MIN_MATCH){
            dst = write_token(dst, max_offset, max_len, src[pos]);
            pos+=max_len;
        }
        else{
            dst = write_token(dst, 0, 0, src[pos]);
            pos++;
        }

    }
    return dst-start;
}

uint64_t LZ77_DECODE(const char* src, uint64_t size, char* dst){

    char* start = dst;
    while(*src){
        const char mode = *src;
        if(mode=='L'){
            src++;
            *dst=(*src++);
            dst++;
        }
        else if(mode=='R'){
            src++;
            int offset, len;
            char symbol;

            sscanf(src, "%d,%d,%c", &offset, &len, &symbol);

            for (int i = 0; i < len; i++) {
                dst[i] = dst[i - offset];
            }
            dst += len;
            
            
            while(*src && *src!='R' && *src!='L')src++;
        }
    }
    
    return dst-start;
}