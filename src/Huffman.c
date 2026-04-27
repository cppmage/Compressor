#include "Huffman.h"
#include "Internal/Huffman_Internal_Encoding.h"
#include "Internal/Huffman_Internal_Decoding.h"

#define TRIGGER_MASK ((1ULL)<<63)
#define MIN(a, b) ((a) < (b) ? (a) : (b))


uint64_t huffman_encode(const uint8_t* src, uint64_t size, uint8_t* dst){
    uint8_t* start = dst;
    struct Node arr[MAX_SYMBOLS];
    struct Data map[MAX_SYMBOLS];
    
    //count symbols
    init_frequencies(src, size, arr);
    qsort(arr, MAX_SYMBOLS, sizeof(struct Node), compare_huffman);

    //building tree
    struct Node pool[MAX_NODES];
    struct Node* root = build_huffman_tree(arr, pool);

    //building codes
    uint64_t bits = generate_codes(root, map);

    //write header
    dst = write_header(dst, map, bits);

    //encoding
    uint64_t position_src = 0;
    uint64_t writen_bites = 0;
    uint64_t current = 0, awaited = 0;
    uint8_t accomulated_len = 0, awaited_len = 0;
    while(1){
        if(accomulated_len==64){
            memcpy(dst, &current, sizeof(current));
            writen_bites+=accomulated_len;
            dst+=sizeof(current);
            current = 0;
            accomulated_len = 0;
        }
        else if(accomulated_len<64 && awaited_len!=0){

            uint8_t provide = 64 - accomulated_len;
            provide = provide<awaited_len ? provide : awaited_len;
            
            uint8_t shifts = awaited_len-provide;

            uint64_t request = awaited>>shifts;
            awaited &= (1ULL << shifts) - 1; 

            awaited_len-=provide;

            current= current<<provide | request;
            accomulated_len+=provide;

        }
        else if(awaited_len==0){
            if(position_src<size){
                const uint8_t c = src[position_src++];
                awaited=map[c].code;
                awaited_len=map[c].len;
            }
            else {
                break;
            }
        }

    }

    if(accomulated_len>0){
            current<<=(64-accomulated_len);
            memcpy(dst, &current, sizeof(current));
            writen_bites+=accomulated_len;
            dst+=sizeof(current);
            current = 0;
            accomulated_len = 0;
    }
    assert(writen_bites==bits);
    //*bits_will_be=writen_bites;
    return dst-start;
}



uint64_t huffman_decode(const uint8_t* src, uint64_t size, uint8_t* dst){
    //read header
    struct Data map[MAX_SYMBOLS];
    int16_t table[MAX_CODE_LEN][1<<12];
    memset(table, -1, sizeof(table));
    uint64_t bits = 0;

    src = read_header(src, map, table, &bits);
    //encoding
    uint64_t dst_position = 0;
    uint64_t word = 0, word_len = 0;
    uint64_t awaited = 0, awaited_len = 0;
    uint64_t bits_read = 0;
    while(bits_read<bits){
        if(awaited_len==0){
            memcpy(&awaited, src, sizeof(awaited));
            src+=sizeof(awaited);
            awaited_len = MIN(64, bits-bits_read);
            awaited>>=64-awaited_len;
            bits_read+=awaited_len;
        }
        while(awaited_len!=0){
            word=(word<<1) | (0x1&(awaited>>(awaited_len-1)));
            word_len++;
            awaited_len--;
            int c = table[word_len][word];
            if(c!=-1){
                dst[dst_position++]=(uint8_t)c;
                word = 0;
                word_len = 0;
            }
        }
    }
    
    return dst_position;
}