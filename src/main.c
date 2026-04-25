
#include <stdint.h>
#include <stdio.h>
#include <stddef.h> 
#include <string.h>
#include "LZ77.h"
#include "Node_Heap.h"
#include "Huffman.h"

int main(){

    struct Heap heap = init_heap();
    struct Node node1, node2, node3, node4;
    node1.freq=10;
    node2.freq=20;
    node3.freq=30;
    node4.freq=15;

    push(&heap, &node3);
    printf("%d\n", (int)top(&heap)->freq);
    push(&heap, &node2);
    printf("%d\n", (int)top(&heap)->freq);
    push(&heap, &node1);
    printf("%d\n", (int)top(&heap)->freq);
    push(&heap, &node4);
    printf("%d %d\n", (int)top(&heap)->freq, (int)heap.right_border);
    pop(&heap);
    printf("%d %d\n", (int)top(&heap)->freq, (int)heap.right_border);
    pop(&heap);
    printf("%d %d\n", (int)top(&heap)->freq, (int)heap.right_border);

    const char* text = "abracadabra hello world hehehehe aaaaaaaaa lalalalalalalalala";
   
    char mass[1024];
    char decoded[1024];
    huffman_encode((uint8_t*)text, strlen(text), (uint8_t*)mass);
    memset(mass, '\0', sizeof(mass));
    memset(decoded, '\0', sizeof(decoded));
    LZ77_ENCODE(text, strlen(text), mass);
    
    LZ77_DECODE(mass, strlen(mass), decoded);

    printf("Encoded: %s\n", mass);
    printf("Decoded: %s\n", decoded);

    FILE* f = fopen("text.txt", "r");

    if(f==NULL){
        return 1;
    }
    char buffer[255];
    while(fgets(buffer, 255, f)){
        printf("%s\n", buffer);
    }
    fclose(f);



    return 0;
}