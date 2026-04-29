
#include <stdint.h>
#include <stdio.h>
#include <stddef.h> 
#include <string.h>
#include "LZ77.h"
#include "Node_Heap.h"
#include "Huffman.h"
#include "Pipeline_Includes.h"
#include <stdlib.h>
#include "Worker.h"
#include "Slicer.h"
#include "Linker.h"

int main(){

    
    // 1. Инициализация памяти
    ChunkPool* chunk_pool = (ChunkPool*)aligned_alloc(64, sizeof(ChunkPool));
    if (!chunk_pool) return perror("malloc pool"), 1;
    init_chunk_pool(chunk_pool);

    SLICER_LINKER_SHARED shared;
    atomic_init(&shared.chunks_from_slicer, 0);
    atomic_init(&shared.chunks_from_linker, 0);

    ThreadPool t_pool;
    init_thread_pool(&t_pool);

    SlicerController s_controller;
    atomic_init(&s_controller.status, SLICER_FREE);
    s_controller.type = ENCODE;

    // 2. Ввод данных через scanf
    printf("=== RISC-V Archiver Core ===\n");
    printf("Введите количество файлов: ");
    if (scanf("%d", &s_controller.to_process) != 1 || s_controller.to_process <= 0) {
        printf("Ошибка ввода количества.\n");
        return 1;
    }

    if (s_controller.to_process > NUMBER_OF_FILES) {
        printf("Ошибка: максимум %d файлов.\n", NUMBER_OF_FILES);
        return 1;
    }

    for (int i = 0; i < s_controller.to_process; i++) {
        printf("Введите путь к файлу [%d]: ", i + 1);
        scanf("%s", s_controller.filinemaes[i]); 
    }

    // 3. Подготовка аргументов
    WorkerArgs w_args[NUMBER_OF_THREADS];
    SlicerArgs s_args = {
        .controller = &s_controller,
        .chunk_pool = chunk_pool,
        .shared = &shared,
        .thread_pool = &t_pool
    };
    LinkerArgs l_args = {
        .chunk_pool = chunk_pool, 
        .shared = &shared
    };

    // 4. Запуск потоков
    pthread_t slicer_thread, linker_thread;

    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        w_args[i].id = i;
        w_args[i].chunk_pool = chunk_pool;
        w_args[i].self_controller = &t_pool.pool[i];
        pthread_create(&t_pool.pool[i].thread, NULL, (void* (*)(void*))thread_worker, &w_args[i]);
    }

    pthread_create(&linker_thread, NULL, (void* (*)(void*))thread_linker, &l_args);
    pthread_create(&slicer_thread, NULL, (void* (*)(void*))thread_slicer, &s_args);

    // 5. Старт
    printf("\n[Main] Все потоки готовы. Начинаю сжатие...\n");
    atomic_store_explicit(&s_controller.status, SLICER_WORK, memory_order_release);
    awake_slicer(&s_controller);

    // 6. Ожидание и финализация
    pthread_join(slicer_thread, NULL);
    pthread_join(linker_thread, NULL);

    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        atomic_store_explicit(&t_pool.pool[i].status, THREAD_EXIT, memory_order_release);
        wake_up_thread(&t_pool.pool[i]);
        pthread_join(t_pool.pool[i].thread, NULL);
    }

    printf("[Main] Работа завершена успешно.\n");
    free(chunk_pool);


    /*
    const char* text = "My name is Alexander. I am 20 years old.";
   
    char LZ77_ENCODED[1024];
    char HUFFMAN_ENCODED[1024];
    char HUFFMAN_DECODED[1024];
    char LZ77_DECODED[1024];
    memset(LZ77_ENCODED, '1', sizeof(LZ77_ENCODED));
    memset(HUFFMAN_ENCODED, '1', sizeof(HUFFMAN_ENCODED));
    memset(HUFFMAN_DECODED, '1', sizeof(HUFFMAN_DECODED));
    memset(LZ77_DECODED, '1', sizeof(LZ77_DECODED));

    uint64_t out1 = LZ77_ENCODE(text, strlen(text), LZ77_ENCODED);
    uint64_t out2 = huffman_encode((uint8_t*)LZ77_ENCODED, out1, (uint8_t*)HUFFMAN_ENCODED);
    uint64_t out3 = huffman_decode((uint8_t*)HUFFMAN_ENCODED, out2, (uint8_t*)HUFFMAN_DECODED);
    uint64_t out4 = LZ77_DECODE(HUFFMAN_DECODED, out3, LZ77_DECODED);
    LZ77_ENCODED[out1]='\0';
    HUFFMAN_ENCODED[out2]='\0';
    HUFFMAN_DECODED[out3]='\0';
    LZ77_DECODED[out4]='\0';

    printf("Og text: %s\n", text);
    printf("Encoded: %s\n", HUFFMAN_ENCODED);
    printf("Decoded: %s\n", LZ77_DECODED);

    FILE* f = fopen("text.txt", "r");

    if(f==NULL){
        return 1;
    }
    char buffer[255];
    while(fgets(buffer, 255, f)){
        printf("%s\n", buffer);
    }
    fclose(f);
    */
    return 0;
}