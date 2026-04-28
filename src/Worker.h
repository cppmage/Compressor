#pragma once
#include "Huffman.h"
#include "Pipeline_Includes.h"

typedef struct{
    int id;
    ThreadController* self_controller;
    ChunkPool* chunk_pool;
}WorkerArgs;

void thread_worker(void* arg);