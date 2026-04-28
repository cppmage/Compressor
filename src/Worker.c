#include "Worker.h"
#include "LZ77.h"
#include "Huffman.h"
#include "Pipeline_Includes.h"
#include <stdatomic.h>

void thread_worker(void* arg){
    WorkerArgs* args = (WorkerArgs*)arg;
    ThreadController* controller = args->self_controller;
    ChunkPool* pool = args->chunk_pool;

    while(atomic_load_explicit(&controller->status, memory_order_acquire)!=THREAD_EXIT){
        sleep_while_no_work(controller);
        if(atomic_load_explicit(&controller->status, memory_order_acquire)==THREAD_EXIT)break;

        int chunk_id = controller->assigned_chunk_id;

        ChunkData* data = &pool->data[chunk_id];
        
        WorkType to_do = data->workType;
        uint8_t* chunk1 = pool->pool[chunk_id][0];
        uint8_t* chunk2 = pool->pool[chunk_id][1];

        uint64_t size_LZ77 = 0;
        uint64_t size_HUFFMAN= 0;

        if(to_do==ENCODE){
            size_LZ77 = LZ77_ENCODE((char*)data->src, data->size, (char*)chunk1);
            size_HUFFMAN = huffman_encode(chunk1, size_LZ77, chunk2);
            data->size=size_HUFFMAN;
        }
        else if(to_do==DECODE){
            size_HUFFMAN = huffman_decode(data->src, data->size, chunk1);
            size_LZ77 = LZ77_DECODE((char*)chunk1, size_HUFFMAN, (char*)chunk2);
            data->size=size_LZ77;
        }

        atomic_store_explicit(&data->status, CHUNK_READY_TO_WRITE, memory_order_release);
        wake_up_chunk(data);
    }

}