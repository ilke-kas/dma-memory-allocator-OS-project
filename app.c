#include "dma.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(){
    dma_init(8);
    void* ptrx = dma_alloc(7);
    printf("free %lx\n", (long) ptrx);
}