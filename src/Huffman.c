#include "Huffman.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Node.h"
#include "Node_Heap.h"

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
void bfs(struct Node* cur, uint64_t code, struct Data* map){
    if (cur == NULL) return;
    if(cur->left==NULL && cur->right==NULL){
        map[cur->c].code=code;
        map[cur->c].len = countBits(code);
        return;
    }
    bfs(cur->left, code<<1, map);
    bfs(cur->right, (code<<1)+1, map);
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
    struct Node* root = top(&heap);
    bfs(root, 0, map);

    //encoding


    return 0;

}