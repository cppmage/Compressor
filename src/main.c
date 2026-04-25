
#include <stdio.h>
#include <stddef.h> 
#include <string.h>
#include "LZ77.h"


int main(){



    const char* text = "abracadabra hello world hehehehe aaaaaaaaa lalalalalalalalala";
    
    char mass[1024];
    char decoded[1024];
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