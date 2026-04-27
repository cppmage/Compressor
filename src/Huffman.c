#include "Huffman.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Node.h"
#include "Node_Heap.h"

#define TRIGGER_MASK ((1ULL)<<63)
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int compare(const void *a, const void *b) {
    const struct Node *nodeA = (const struct Node *)a;
    const struct Node *nodeB = (const struct Node *)b;

    if (nodeA->freq > nodeB->freq) return -1;
    if (nodeA->freq < nodeB->freq) return 1;
    return 0;
}
uint8_t countBits(uint64_t n) {
    if (n == 0) return 1; 
    uint8_t len = 0;
    while (n > 0) {
        n >>= 1; 
        len++;
    }
    return len;
}
void bfs(struct Node* cur, uint64_t code, struct Data* map, uint64_t* bits, uint8_t depth){
    if (cur == NULL) return;
    if(cur->left==NULL && cur->right==NULL){
        map[cur->c].code=code;
        map[cur->c].len = depth;
        (*bits)+=map[cur->c].len*cur->freq;
        return;
    }
    bfs(cur->left, code<<1, map, bits, depth+1);
    bfs(cur->right, (code<<1)+1, map, bits, depth+1);
}



uint64_t huffman_encode(const uint8_t* src, uint64_t size, uint8_t* dst){
    struct Node arr[MAX_SYMBOLS];
    struct Data map[MAX_SYMBOLS];
    

    for(int i = 0; i<MAX_SYMBOLS; i++){
        arr[i].c = (unsigned char)i;
        arr[i].freq=0;
        arr[i].left =NULL;
        arr[i].right=NULL;
    }

    for(uint64_t i = 0; i<size; i++){
        arr[src[i]].freq++;
    }

    
    qsort(arr, MAX_SYMBOLS, sizeof(struct Node), compare);

    struct Heap heap = init_heap();

    for(int i = 0; i<MAX_SYMBOLS; i++){
        if(arr[i].freq==0)break;
        push(&heap, &arr[i]);
    }

    //building tree
    struct Node pool[MAX_NODES];
    uint64_t pool_id = 0;
    while(get_size(&heap)!=1){
        struct Node* node = &pool[pool_id++];
        struct Node*left = top(&heap);
        pop(&heap);
        struct Node*right = top(&heap);
        pop(&heap);

        node->freq=left->freq+right->freq;
        node->left = left;
        node->right = right;
        push(&heap, node);
    }
    //building codes
    uint64_t bits = 0;
    struct Node* root = top(&heap);
    bfs(root, 0, map, &bits, 0);

    //encoding
    int active_fields = 0;
    for(int i = 0; i<MAX_SYMBOLS; i++){
        active_fields+=(map[i].len!=0);
    }
    dst+=sprintf((char*)dst, "%d %llu ", active_fields, (unsigned long long)bits);
    for(int i = 0; i<MAX_SYMBOLS; i++){
        if(map[i].len!=0){
            dst+=sprintf((char*)dst, "%d %llu %hhu ", i, (unsigned long long)map[i].code, map[i].len);
        }
    }
    uint64_t position_src = 0;
    uint64_t writen_bites = 0;
    uint64_t current = 0, awaited = 0;
    uint8_t accomulated_len = 0, awaited_len = 0;
    (*dst++)='|';
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
    return writen_bites;
}

#define MAX_CODE_LEN 12
uint64_t huffman_decode(const uint8_t* src, uint64_t size, uint8_t* dst){
    //read header
    struct Data map[MAX_SYMBOLS];
    int16_t table[MAX_CODE_LEN][1<<12];
    memset(table, -1, sizeof(table));

    int active_fields = 0;
    uint64_t bits = 0;
    int advance;
    sscanf((char*)src, "%d %llu%n", &active_fields, (unsigned long long*)&bits, &advance);
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
    
    return 0;
}