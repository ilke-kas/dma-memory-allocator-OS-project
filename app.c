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
    void* ptrx4 = dma_alloc(17);
    printf("\n\n\n1035 zamanı\n");
    void* ptrx = dma_alloc(520);
    printf("\n\n\n35 zamanı\n");
    void* ptrx2 = dma_alloc(35);
    void* ptrx3 = dma_alloc(2);
    dma_print_bitmap();
    //void* ptrx4 = dma_alloc(288);
     //dma_print_bitmap();
    //void* ptrx4 = dma_alloc(65);
    dma_free(ptrx4);
    dma_print_bitmap();
    dma_free(ptrx);
    dma_print_bitmap();

    //dma_print_page(0);

}