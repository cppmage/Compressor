#include "Node_Heap.h"
#include "Node.h"
#include <stdint.h>
#include <string.h>

struct Heap init_heap(){
    struct Heap heap;
    heap.right_border = 1;
    return heap;
}

uint64_t get_size(const struct Heap* heap){
    return heap->right_border-1;
}

void swap_nodes(struct Node** ptr, uint64_t id1, uint64_t id2){
    struct Node* buf = ptr[id1];
    ptr[id1] = ptr[id2];
    ptr[id2] = buf;
}

void sieve_from_top(struct Heap* heap){
    uint64_t cur_i = 1, right_border = heap->right_border;
    struct Node** ptr = heap->mass;

    while(cur_i*2<right_border){
        uint64_t left_i = cur_i*2;
        uint64_t right_i = cur_i*2+1;
        
        int64_t cur_value =ptr[cur_i]->freq;
        int64_t left_value =ptr[left_i]->freq;
        int64_t right_value = right_i < right_border ? ptr[right_i]->freq : INT64_MAX;

        if(left_value>right_value){
            left_i = right_i;
            left_value = right_value;
        }

        if(left_value<cur_value){
            swap_nodes(ptr, left_i, cur_i);
            cur_i = left_i;
        }
        else{
            break;
        }

    }
}

void sieve_from_bottom(struct Heap* heap){
    uint64_t cur_i = heap->right_border, right_border = heap->right_border;
    struct Node** ptr = heap->mass;

    while(cur_i!=1){
        uint64_t parrent_i = cur_i/2;
        
        int64_t cur_value =ptr[cur_i]->freq;
        int64_t parrent_value =ptr[parrent_i]->freq;


        if(parrent_value>cur_value){
            swap_nodes(ptr, parrent_i, cur_i);
            cur_i = parrent_i;
        }
        else{
            break;
        }

    }
}

struct Node* top(const struct Heap* heap){
    return heap->mass[1];
}

void pop(struct Heap* heap){
    swap_nodes(heap->mass, 1, get_size(heap));
    heap->right_border--;
    sieve_from_top(heap);
}

void push(struct Heap* heap, struct Node* node){
    heap->mass[heap->right_border]=node;
    sieve_from_bottom(heap);
    heap->right_border++;
}

