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

    dma_init(15);
    void* ptrx = dma_alloc(17);
    unsigned int* ptr = (unsigned int*) ptrx;
    *ptr  = 34648739;
    dma_print_bitmap();
    void* ptrx2 = dma_alloc(35);
    dma_print_bitmap();
    void* ptrx3 = dma_alloc(2);
    dma_print_bitmap();
    void* ptrx4 = dma_alloc(65);
    dma_print_bitmap();
    
    dma_print_page(0);

}