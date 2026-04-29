#include "Pipeline_Includes.h"

typedef struct{
    SlicerController* controller;
    ChunkPool* chunk_pool;
    SLICER_LINKER_SHARED* shared;
    ThreadPool* thread_pool;
}SlicerArgs;

void thread_slicer(void* arg);