#pragma once
#include "Huffman_Internal_Includes.h"

int compare_huffman(const void *a, const void *b);
void bfs(struct Node* cur, uint64_t code, struct Data* map, uint64_t* bits, uint8_t depth);

void init_frequencies(const uint8_t* src, uint64_t size, struct Node* arr);

struct Node* build_huffman_tree(struct Node* arr, struct Node* pool);

uint64_t generate_codes(struct Node* root, struct Data* map);

uint8_t* write_header(uint8_t* dst, const struct Data* map, uint64_t bits);
