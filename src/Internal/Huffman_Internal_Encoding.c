#include "Huffman_Internal_Encoding.h"

int compare_huffman(const void *a, const void *b) {
    const struct Node *nodeA = (const struct Node *)a;
    const struct Node *nodeB = (const struct Node *)b;

    if (nodeA->freq > nodeB->freq) return -1;
    if (nodeA->freq < nodeB->freq) return 1;
    return 0;
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

void init_frequencies(const uint8_t* src, uint64_t size, struct Node* arr) {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        arr[i].c = (unsigned char)i;
        arr[i].freq = 0;
        arr[i].left = arr[i].right = NULL;
    }
    for (uint64_t i = 0; i < size; i++) {
        arr[src[i]].freq++;
    }
}

struct Node* build_huffman_tree(struct Node* arr, struct Node* pool) {
    struct Heap heap = init_heap();
    for(int i = 0; i<MAX_SYMBOLS; i++){
        if(arr[i].freq==0)break;
        push(&heap, &arr[i]);
    }
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
    return top(&heap);
}


uint64_t generate_codes(struct Node* root, struct Data* map) {
    uint64_t bits = 0;
    bfs(root, 0, map, &bits, 0);
    return bits;
}

uint8_t* write_header(uint8_t* dst, const struct Data* map, uint64_t bits) {
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
    (*dst++)='|';
    return dst;
}
