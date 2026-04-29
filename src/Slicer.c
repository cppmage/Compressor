#include "Slicer.h"
#include "Pipeline_Includes.h"
#include <assert.h>
#include <stdatomic.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    int fd;
    uint8_t* ptr;
    uint8_t* base_ptr;
    uint64_t file_size;
}FileHandler;



void thread_slicer(void* arg){

    unsigned int cur_chunk_id = 0;
    SlicerArgs* args = (SlicerArgs*)arg;
    SlicerController* controller = args->controller;
    ChunkPool* chunks_carrier=args->chunk_pool;
    ChunkData* data = chunks_carrier->data;
    ThreadPool* thread_pool=args->thread_pool;
    SLICER_LINKER_SHARED* shared=args->shared;

    FileHandler fHandlers_READ[NUMBER_OF_FILES];
    FileHandler fHandlers_WRITE[NUMBER_OF_FILES];

    int read_files = 0, write_files = 0;

    while(1){
        
        slicer_sleep(controller);
        if(atomic_load_explicit(&controller->status, memory_order_acquire)==SLICER_EXIT)return;

        int to_process = controller->to_process;
        WorkType type = controller->type;
        char (*files)[FILENAME_MAX_LEN] = controller->filinemaes;
        

        if(type==ENCODE){
            read_files = to_process;
            write_files=1;
            uint64_t max_file_size = 0;
            int dst_fd = open("output.cmage", O_RDWR | O_CREAT | O_TRUNC, 0666);

            for(int i = 0; i<to_process; i++){
                struct stat st;
                stat(files[i], &st);
                fHandlers_READ[i].file_size = st.st_size;
                max_file_size += st.st_size + HEADER_MAX_SIZE;
            }
            ftruncate(dst_fd, max_file_size);
            uint8_t* dst_ptr = mmap(NULL, max_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, 0);

            fHandlers_WRITE[0].file_size=max_file_size;
            fHandlers_WRITE[0].base_ptr=fHandlers_WRITE[0].ptr=dst_ptr;
            fHandlers_WRITE[0].fd=dst_fd;

            for(int i = 0; i<to_process; i++){
                
                memcpy(fHandlers_WRITE[0].ptr, files[i], FILENAME_MAX_LEN);
                fHandlers_WRITE[0].ptr+=FILENAME_MAX_LEN;
                memcpy(fHandlers_WRITE[0].ptr, &fHandlers_READ[i].file_size, sizeof(uint64_t));
                fHandlers_WRITE[0].ptr+=sizeof(uint64_t);

                int fd_srci = open(files[i], O_RDONLY);
                
                uint8_t* src_ptr = mmap(NULL, fHandlers_READ[i].file_size, PROT_READ, MAP_PRIVATE, fd_srci, 0);

                fHandlers_READ[i].fd = fd_srci;
                fHandlers_READ[i].base_ptr=fHandlers_READ[i].ptr = src_ptr;
                
                uint64_t total_chunks = (fHandlers_READ[i].file_size + SRC_CHUNK_SIZE - 1) / SRC_CHUNK_SIZE;
                uint64_t file_size=fHandlers_READ[i].file_size;

                for(uint64_t j = 0; j<total_chunks; j++){
                    
                    uint64_t current_chunk_size = SRC_CHUNK_SIZE;
                    if(file_size<current_chunk_size)current_chunk_size=file_size;
                    file_size-=current_chunk_size;

                    if(cur_chunk_id>=NUMBER_OF_CHUNKS)cur_chunk_id=0;
                    ChunkData* current_data = data+cur_chunk_id;
                    wait_release_of_chunk(current_data, CHUNK_FREE);
                    if(atomic_load_explicit(&current_data->status, memory_order_acquire)==CHUNK_EXIT)goto exit_label;

                    atomic_store_explicit(&current_data->status, CHUNK_IN_PROCESS, memory_order_release);
                    current_data->src=fHandlers_READ[i].ptr;
                    current_data->size=current_chunk_size;
                    current_data->dst=&fHandlers_WRITE[0].ptr;
                    fHandlers_READ[i].ptr+=current_chunk_size;
                    
                    //punch free thread
                    int thread_id = 0;
                    while(1){
                        if(thread_id>=NUMBER_OF_THREADS){
                            thread_id=0;
                            __asm__ volatile("pause");
                        }
                        if(atomic_load_explicit(&thread_pool->pool[thread_id].status, memory_order_acquire)==THREAD_FREE){
                            thread_pool->pool[thread_id].assigned_chunk_id=cur_chunk_id;
                            atomic_store_explicit(&thread_pool->pool[thread_id].status, THREAD_BUSY, memory_order_release);
                            wake_up_thread(&thread_pool->pool[thread_id]);
                            break;
                        }
                        thread_id++;
                    }

                    cur_chunk_id++;
                    atomic_fetch_add_explicit(&shared->chunks_from_slicer, 1, memory_order_acq_rel);
                }
                assert(file_size==0);
            }
        }
        else if(type==DECODE){

        }

        while(atomic_load_explicit(&shared->chunks_from_slicer, memory_order_acquire)!=atomic_load_explicit(&shared->chunks_from_linker, memory_order_acquire)){
            __asm__ volatile("pause");
        }

        for(int i = 0; i<read_files; i++){
            msync(fHandlers_READ[i].base_ptr, fHandlers_READ[i].file_size, MS_SYNC);
            munmap(fHandlers_READ[i].base_ptr, fHandlers_READ[i].file_size);
            close(fHandlers_READ[i].fd);
        }
        read_files = 0;
        for(int i = 0; i<write_files; i++){
            msync(fHandlers_WRITE[i].base_ptr, fHandlers_WRITE[i].file_size, MS_SYNC);
            munmap(fHandlers_WRITE[i].base_ptr, fHandlers_WRITE[i].file_size);
            close(fHandlers_WRITE[i].fd);
        }
        write_files = 0;
        atomic_store_explicit(&controller->status, SLICER_FREE, memory_order_release);
    }

    exit_label: 
    for(int i = 0; i<read_files; i++){
        msync(fHandlers_READ[i].base_ptr, fHandlers_READ[i].file_size, MS_SYNC);
        munmap(fHandlers_READ[i].base_ptr, fHandlers_READ[i].file_size);
        close(fHandlers_READ[i].fd);
    }
    
    for(int i = 0; i<write_files; i++){
        msync(fHandlers_WRITE[i].base_ptr, fHandlers_WRITE[i].file_size, MS_SYNC);
        munmap(fHandlers_WRITE[i].base_ptr, fHandlers_WRITE[i].file_size);
        close(fHandlers_WRITE[i].fd);
    }
    

}