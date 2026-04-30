#pragma once
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdalign.h>

#define CHUNK_SIZE (1ULL<<18)
#define HEADER_MAX_SIZE 1556

#define SRC_CHUNK_SIZE ((CHUNK_SIZE)-HEADER_MAX_SIZE)

#define NUMBER_OF_CHUNKS 4
#define NUMBER_OF_THREADS 2
#define FILENAME_MAX_LEN 256
#define NUMBER_OF_FILES 128

typedef struct{
    alignas(64) _Atomic uint64_t chunks_from_slicer;
    alignas(64) _Atomic uint64_t chunks_from_linker;
    alignas(64) uint64_t final_size;
}SLICER_LINKER_SHARED;

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

typedef enum{
    SLICER_FREE,
    SLICER_WORK,
    SLICER_CANCELL,
    SLICER_EXIT
}SlicerStatus;

typedef struct{
    _Atomic ChunkStatus status;

    WorkType workType;

    uint8_t* src;
    uint64_t size;
    uint8_t** dst;
}ChunkData;

typedef struct {
    _Atomic ThreadStatus status;
    uint64_t assigned_chunk_id;

    pthread_t thread;
}ThreadController;

typedef struct {
    uint8_t* ptr;
    uint64_t ptr_size;
    uint64_t bytes;
    uint64_t chunks;
}MetaData;

typedef struct{
    uint8_t pool[NUMBER_OF_CHUNKS][2][CHUNK_SIZE];
    ChunkData data[NUMBER_OF_CHUNKS];

    MetaData meta_data[NUMBER_OF_CHUNKS];
}ChunkPool;

typedef struct{
    ThreadController pool[NUMBER_OF_THREADS];
}ThreadPool;

typedef struct{
    _Atomic SlicerStatus status;

    char filinemaes[NUMBER_OF_FILES][FILENAME_MAX_LEN];
    int to_process;
    WorkType type;
}SlicerController;

void wait_release_of_chunk(ChunkData* chunk, ChunkStatus awaited);
void wake_up_chunk(ChunkData* chunk);

void sleep_while_no_work(ThreadController* controller);
void wake_up_thread(ThreadController* controller);

void init_chunk_pool(ChunkPool* pool);
void init_thread_pool(ThreadPool* pool);

void slicer_sleep(SlicerController* controller);
void awake_slicer(SlicerController* controller);