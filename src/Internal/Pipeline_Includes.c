#include "../Pipeline_Includes.h"
#include <string.h>
#include <stdatomic.h>
#define _GNU_SOURCE
#include <unistd.h> 
#include <sys/syscall.h>    
#include <linux/futex.h>    
#include <errno.h> 

void wait_status_of_chunk(ChunkData* chunk){
    while(1){
        ChunkStatus current = atomic_load_explicit(&chunk->status, memory_order_acquire);
        if(current==CHUNK_EXIT || current==CHUNK_READY_TO_WRITE)return;

        long long res = syscall(SYS_futex, &chunk->status, FUTEX_WAIT, current, NULL, NULL, 0);

        if (res == -1 && errno == EAGAIN) {
            continue; 
        }

    }
}

void wake_up_chunk(ChunkData* chunk){
    syscall(SYS_futex, &chunk->status, FUTEX_WAKE, 1, NULL, NULL, 0);
}

void sleep_while_no_work(ThreadController* controller){
    ThreadStatus expected = THREAD_BUSY; 
    if (!atomic_compare_exchange_strong(&controller->status, &expected, THREAD_FREE)) {
        if (expected == THREAD_EXIT) return; 
    }
    while(1){
        long res = syscall(SYS_futex, &controller->status, FUTEX_WAIT, THREAD_FREE, NULL, NULL, 0);
        if (res == -1 && errno == EAGAIN) {
            break; 
        }
        
        if (atomic_load_explicit(&controller->status, memory_order_acquire) != THREAD_FREE) {
            break;
        }
    }
}
void wake_up_thread(ThreadController* controller){
    atomic_store_explicit(&controller->status, THREAD_BUSY, memory_order_release);
    syscall(SYS_futex, &controller->status, FUTEX_WAKE, 1, NULL, NULL, 0);
}

void init_chunk_pool(ChunkPool* pool){
    memset(pool->pool, 0, sizeof(pool->pool));
    for(int i = 0; i<NUMBER_OF_CHUNKS; i++){
        pool[i].data->dst=NULL;
        pool[i].data->src=NULL;
        pool[i].data->size=0;
        pool[i].data->status=CHUNK_FREE;
        pool[i].data->workType=ENCODE;
    }
}

void init_thread_pool(ThreadPool* pool){
    for(int i = 0; i<NUMBER_OF_THREADS; i++){
        pool->pool[i].assigned_chunk_id=0;
        pool->pool[i].status=THREAD_FREE;
        pool->pool[i].thread=0;
    }
}

