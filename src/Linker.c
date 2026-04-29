#include "Linker.h"
#include "Pipeline_Includes.h"
#include <string.h>
#include <stdatomic.h>
#include <stdint.h>

void thread_linker(void* arg){
    LinkerArgs* args = (LinkerArgs*)arg;
    unsigned int cur_chunk_id = 0;
    ChunkPool* chunks_carrier=args->chunk_pool;
    ChunkData* data = chunks_carrier->data;
    SLICER_LINKER_SHARED* shared=args->shared;

    while(1){
        if(cur_chunk_id>=NUMBER_OF_CHUNKS)cur_chunk_id=0;
        ChunkData* current_data = data+cur_chunk_id;
        wait_release_of_chunk(current_data, CHUNK_READY_TO_WRITE);
        if(atomic_load_explicit(&current_data->status, memory_order_acquire)==CHUNK_EXIT)return;
        uint8_t* src = chunks_carrier->pool[cur_chunk_id][1];
        uint8_t* dst = *current_data->dst;
        uint64_t size = current_data->size;

        //coppy
        memcpy(dst, src, size);
        *current_data->dst+=size;

        //change status and move
        atomic_store_explicit(&current_data->status, CHUNK_FREE, memory_order_release);
        wake_up_chunk(current_data);
        cur_chunk_id++;
        atomic_fetch_add_explicit(&shared->chunks_from_linker, 1, memory_order_acq_rel);
    }
}