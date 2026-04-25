#pragma  once
#include "Node.h"

struct Heap{
    struct Node* mass[MAX_NODES+1];
    uint64_t right_border;
};

struct Heap init_heap();

uint64_t get_size(const struct Heap* heap);

void sieve_from_top(struct Heap* heap);

void sieve_from_bottom(struct Heap* heap);

struct Node* top(const struct Heap* heap);

void pop(struct Heap* heap);

void push(struct Heap* heap, struct Node* node);
