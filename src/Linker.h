#pragma once
#include "Pipeline_Includes.h"

typedef struct{
    ChunkPool* chunk_pool;
}LinkerArgs;

void thread_linker(void* arg);