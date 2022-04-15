#include "dma.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int dma_init(int m){
    printf("pid is:%d\n",getpid());
    void *p;
    //check the size of the requested allocation
    if(m < 14 || m > 22){
        printf("M value should be in between 14 and 22 inclusive\n");
        return 0;
    }
    else{
        double size = pow(2,m);
        p = mmap(NULL, (size_t)size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED){
            perror("Could not mmap\n");
            return 0;
        }
        printf("Can map the memory\n");
        printf ("%lx\n", (long) p); // print start address
        while (1)
        ; // loop so that we can run pmap on the process
        //initialization of the bitmap
        //find bitmap size in bits
        double bitmap_bits = size / 8;
        double bitmap_bytes = bitmap_bits / 8;

        //allocate that area as array from the segment 
        int *bitmap_ptr = p;
        //initialize all bits to 1 since free

        //allocate 256 byte free area


    }


    return 1;
}

