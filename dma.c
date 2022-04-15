#include "dma.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Your Library Global Variables */
void *segmentptr; // points to the start of memory segment to manage
int   segmentsize;  // size of segment to manage
void* bitmap;
int  bitmapsize;



int dma_init(int m){
    void *p;
    //check the size of the requested allocation
    if(m < 14 || m > 22){
        printf("M value should be in between 14 and 22 inclusive\n");
        return -1;
    }
    else{
        double size = pow(2,m);
        p = mmap(NULL, (size_t)size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (p == MAP_FAILED){
            perror("Could not mmap\n");
            return 0;
        }
        printf("Can map the memory\n");
        printf ("bitmap %lx\n", (long) p); // print start address
        //initialization of the bitmap
        //find bitmap size in bits
        double bitmap_bits = size / 8;
        double bitmap_bytes = bitmap_bits / 8;
        double int_num = bitmap_bytes / 4;
        bitmapsize = (int) bitmap_bytes;
        //allocate that area as array from the segment
        bitmap =  p;
        unsigned int allones = 4294967295;
        unsigned int* ptr = (unsigned int*)bitmap;
        int i;
        for(i = 0 ; i < (int)int_num; i++){
           *ptr = allones;
            //printf("%u\n",*ptr);
            ptr = ptr + 1;
        }
        //reserved area 
        for(i = 0 ; i < 64; i++){
            ptr = ptr +1;
        }
        //allocation area
        segmentptr = (void*) ptr;
        segmentsize = (int) size - bitmapsize - 256; 
        printf(" segmentptr %lx\n", (long) segmentptr);
        //printf("sizes: size:%d, segmentsize:%d, bitmapsize:%d\n",(int)size,segmentsize,bitmapsize);
    }
    return 0;
}

void *dma_alloc (int size){
    int newsize = size / 16;
    int newmod = size % 16;
    if(newmod != 0){
        newsize++;
    }
    void* ptr = lookbitmap(size);
    if(size > segmentsize){
        return NULL;
    }
    else if(ptr == NULL){
        return NULL;
    }
    //change bitmap
    int total_bit = bitmapsize * 8;
    int iteration = total_bit / 32;
    int cur_bit = total_bit;
    int iter_mod = total_bit%32;
    unsigned int* ptr = (unsigned int*)bitmap;
    if(iter_mod != 0){
        iteration++;
    }
    int j;
    for(j = 0 ; j < iteration; j++){
        unsigned int change_val;
        if(cur_bit < 32){
            if(newsize == 2){
                change_val = pow(2,cur_bit-1) ;
            }
            else{
                change_val = pow(); //BURDA KALDINIZ
            }
            
        }else{
            if(newsize == 2){
                change_val = pow(2,31);
            }
            else{
                change_val = pow(); // BURDA KALDINIZ
            }
        }


    }



    return ptr;
}
void* lookbitmap(int size){
    int i;
    int empty= 0;
    int total_bit = bitmapsize * 8;
    //int total_bit = 32; //sil
    //unsigned int * bitmap = malloc(sizeof(unsigned int)); // sil
    //*bitmap = 3486908413; //sil
    int iteration = total_bit / 32;
    int cur_bit = total_bit;
    int iter_mod = total_bit%32;
    void* ptrreturn = segmentptr;
    unsigned int* ptr = (unsigned int*)bitmap;
    if(iter_mod != 0){
        iteration++;
    }
    int j;
    int newsize = size / 16;
    int newmod = size % 16;
    if(newmod != 0){
        newsize++;
    }
    for(j = 0 ; j < iteration; j++){
        unsigned int shift_val;
        if(cur_bit < 32){
            shift_val = pow(2,cur_bit-1) + pow(2,cur_bit-2); // BUNU total_bit ten değiştirdiniz
        }else{
            shift_val = pow(2,31) + pow(2,30); 
        }

        for( i = 0 ; i < cur_bit/2; i++){
            unsigned int result = (*ptr) & shift_val;
            printf("%d-ptr: %u, shift_val:%u, result:%u\n",i,(*ptr),shift_val,result);
            if(result == shift_val){
                printf("i:%d de buldu\n",i);
                empty++;
            }
            else{
                ptrreturn = ptrreturn + (empty + 1 )* 4* sizeof(unsigned int); 
                empty = 0;
            }
     
            if(newsize == empty){
                printf("empty is:%d\n",empty);
                printf("in func ptrreturn %lx\n", (long) ptrreturn);
                return ptrreturn;
            }
            shift_val = shift_val >> 1;
            shift_val = shift_val >> 1;
        }
       
        ptr = ptr + 1;
        cur_bit = cur_bit - 32;
        if(cur_bit <= 0){
            break;
        }
    }
    return NULL;
    
}

