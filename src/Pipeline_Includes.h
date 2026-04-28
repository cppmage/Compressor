#pragma once
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>

#define CHUNK_SIZE (1ULL<<18)
#define NUMBER_OF_CHUNKS 4
#define NUMBER_OF_THREADS 2

typedef enum {
    CHUNK_FREE,
    CHUNK_IN_PROCESS,
    CHUNK_READY_TO_WRITE,
    CHUNK_EXIT
}ChunkStatus;

typedef enum {
    THREAD_FREE,
    THREAD_BUSY,
    THREAD_EXIT
}ThreadStatus;

typedef enum {
    ENCODE,
    DECODE
}WorkType;

typedef struct{
    _Atomic ChunkStatus status;

    WorkType workType;

    uint8_t* src;
    uint64_t size;
    uint8_t* dst;
}ChunkData;

typedef struct {
    _Atomic ThreadStatus status;
    uint64_t assigned_chunk_id;

    pthread_t thread;
}ThreadController;

typedef struct{
    uint8_t pool[NUMBER_OF_CHUNKS][2][CHUNK_SIZE];
    ChunkData data[NUMBER_OF_CHUNKS];
}ChunkPool;

typedef struct{
    ThreadController pool[NUMBER_OF_THREADS];
}ThreadPool;

void wait_release_of_chunk(ChunkData* chunk, ChunkStatus awaited);
void wake_up_chunk(ChunkData* chunk);

void sleep_while_no_work(ThreadController* controller);
void wake_up_thread(ThreadController* controller);

void init_chunk_pool(ChunkPool* pool);
void init_thread_pool(ThreadPool* pool);