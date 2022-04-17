#include "dma.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

/* Your Library Global Variables */
void *segmentptr; // points to the start of memory segment to manage
int segmentsize;  // size of segment to manage
void* bitmap;    
int bitmapsize;
int fragsize;
pthread_mutex_t lp, lp2, lp3, lp4, lp5;
//helper functions
void* lookbitmap(int size);
void dma_print_temp(unsigned int temp);
void toHex(int size);

int dma_init(int m){
    void *p;
    fragsize = 0;
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
            ptr = ptr + 1;
        }
        //reserved area 
        for(i = 0 ; i < 64; i++){
            ptr = ptr +1;
        }
        //allocation area
        segmentptr = (void*) ptr;
        segmentsize = (int) size - bitmapsize - 256; 
    }
    //initialize all mutexes
    pthread_mutex_init(&lp,NULL);
    pthread_mutex_init(&lp2,NULL);
    pthread_mutex_init(&lp3,NULL);
    pthread_mutex_init(&lp4,NULL);
    pthread_mutex_init(&lp5,NULL);
    return 0;
}
void dma_free (void *p){
    ptrdiff_t difptr = p - segmentptr;
    unsigned int*ptr2;
    ptr2 = (unsigned int*)bitmap;
    long shift_val = (long)difptr/8;
    int total_bit = (bitmapsize) * 8;
    total_bit -= shift_val;
    int iteration = total_bit / 32;
    int cur_bit = total_bit;
    int iter_mod = total_bit%32;
    if(iter_mod != 0){
        iteration++;
    }
    int k;
    for(k = 0 ; k < iteration; k++){
        if(shift_val > 32){
            shift_val -=32;
            cur_bit-= 32;
            ptr2 = ptr2 +1;
            continue;
         }
        unsigned int change_val;
        if(cur_bit < 32){
            change_val = pow(2,cur_bit-1) + pow(2,cur_bit-2);
            change_val = ~change_val;
        }else{
            change_val = (pow(2,31) + pow(2,30));
            change_val = ~change_val;
        }
        pthread_mutex_lock(&lp);
        unsigned int temp = *ptr2 << shift_val;
        unsigned int temp2 = temp >> shift_val;
        unsigned int temp3 = *ptr2 ^ temp2;
        unsigned int temp4 = temp << 2;
        unsigned int temp5 = temp4;
        int freesize = 2;
        while((temp4 & change_val) == temp4 ){
            temp5 = ~change_val | temp5;
            change_val = change_val >> 2;
            freesize +=2;
            if(freesize % 32 == 0){
                if((freesize / 32) == 1){
                    unsigned int change_val2;
                    temp5 = temp5>> 2;
                    if(cur_bit < 32){
                        change_val2 = pow(2,cur_bit-1) + pow(2,cur_bit-2);
                    }else{
                        change_val2 = (pow(2,31) + pow(2,30));
                    }
                    temp5 = temp5 | change_val2;
                    temp5 = temp5 >> shift_val;
                    temp5 = temp5 | temp3;
                }
                memcpy(ptr2, &temp5, sizeof(unsigned int));
                ptr2++;
                temp4 = *ptr2;
                temp5 = temp4;
                cur_bit -=32; 
                if(cur_bit < 32){
                    change_val = pow(2,cur_bit-1) + pow(2,cur_bit-2);
                    change_val = ~change_val;
                }else{
                    change_val = (pow(2,31) + pow(2,30));
                    change_val = ~change_val;
                }
            }
        }
        k = iteration;
        if(freesize < 32){ 
            unsigned int change_val2;
            temp5 = temp5>> 2;
            if(cur_bit < 32){
                change_val2 = pow(2,cur_bit-1) + pow(2,cur_bit-2);
            }else{
                change_val2 = (pow(2,31) + pow(2,30));
            }
            temp5 = temp5 | change_val2;
            temp5 = temp5 >> shift_val;
            temp5 = temp5 | temp3;
        }
        memcpy(ptr2, &temp5, sizeof(unsigned int));
        pthread_mutex_unlock(&lp);
    }
}

void *dma_alloc (int size){
    int newsize = size / 8;
    int newmod = size % 8;
    if(newmod != 0){
        newsize++;
    }
    if(newsize %2 == 1){
        newsize++;
    }
    pthread_mutex_lock(&lp);
    void* ptr = lookbitmap(size);
    if(size > segmentsize){
        return NULL;
    }
    else if(ptr == NULL){
        return NULL;
    }
    //change bitmap
     ptrdiff_t difptr = ptr - segmentptr;
    long int shift_val = (long int)difptr /8;
    long int shift_iter = (shift_val + newsize )/ 32;
    long int shift_mod = (shift_val + newsize )% 32;
    if(shift_mod != 0){
        shift_iter++;
    }
    int cond = shift_val/32;
    //change bitmap
    int total_bit = bitmapsize * 8;
    int iteration = total_bit / 32;
    int cur_bit = total_bit;
    int iter_mod = total_bit%32;
    int started = 0;
    unsigned int* ptr2 = (unsigned int*)bitmap;
    if(iter_mod != 0){
        iteration++;
    }
    int j;
    for(j = 0 ; j < shift_iter; j++){
        unsigned int change_val;
        if(cur_bit < 32){
            change_val = pow(2,cur_bit-1); 
            if(newsize > 2){
                int k;
                for(k = 2; k < newsize; k++){
                    change_val = change_val + pow(2, cur_bit-1-k);
                }
            }
            if(cond > j || started ==1){
                change_val = change_val + pow(2,cur_bit-2);
            }
        }else{
            change_val = pow(2,31);
            if(newsize > 2 && newsize <=32){
                int k;
                for(k = 2; k < newsize; k++){
                    change_val = change_val + pow(2, 31-k);
                }
            }
            else if(newsize >32){
                 int k;
                for(k = 2; k < 32; k++){
                    change_val = change_val + pow(2, 31-k);
                }
            }
             if(cond>j|| started == 1){
                change_val = change_val + pow(2,30);
            }
        }
        //find the place where to change bitmap
        if(cond <= j){
            started = 1;
            unsigned local_shift_val_temp = newsize-(shift_val % 32 + newsize - 32);
            unsigned local_shift_val = 32- local_shift_val_temp;
            if(local_shift_val_temp < 0){
                local_shift_val = shift_val;
            }  
            unsigned int temp = *ptr2 << local_shift_val;
            unsigned int temp2 = temp >> local_shift_val;
            unsigned int temp8 = temp2;
            if(shift_val != 0){
                change_val = change_val >> local_shift_val;
            }
            unsigned int temp3 = *ptr2 ^ temp2;
            unsigned int temp4 = temp8 ^ change_val;
            unsigned int temp6 = temp3 | temp4;
            newsize = newsize - local_shift_val_temp;
            shift_val = 32;
            memcpy(ptr2, &temp6, sizeof(unsigned int));
        }
         ptr2 = ptr2 +1;
         cur_bit = cur_bit -32;
    }
    fragsize += ((newsize*8)-size);
    pthread_mutex_unlock(&lp);
    return ptr;
}
void* lookbitmap(int size){
    int i;
    int empty= 0;
    int total_bit = bitmapsize * 8;
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
            shift_val = pow(2,cur_bit-1) + pow(2,cur_bit-2);
        }else{
            shift_val = pow(2,31) + pow(2,30); 
        }

        for( i = 0 ; i < 16; i++){
            unsigned int result = (*ptr) & shift_val;
            if(result == shift_val){
                empty++;
            }
            else{
                ptrreturn = ptrreturn + (empty + 1 )* 4* sizeof(unsigned int); 
                empty = 0;
            }
     
            if(newsize == empty){
                return ptrreturn;
            }
            shift_val = shift_val >> 2;
        }
        ptr = ptr + 1;
        cur_bit = cur_bit - 32;
        if(cur_bit <= 0){
            break;
        }
    }
    return NULL;
}

void dma_print_bitmap(){
    unsigned int* ptr = bitmap;
    int total_bit = bitmapsize * 8;
    int iteration = total_bit / 32;
    int iter_mod = total_bit%32;
    if(iter_mod != 0){
        iteration++;
    }
    int k;
    pthread_mutex_lock(&lp);
    for(k = 0; k < iteration; k++){
        unsigned int n = *ptr;
        int i;
        for (i = 31; i >= 0; i--) {
            unsigned int j = n >> i;
            if (j & 1)
                printf("1");
            else
                printf("0");
            if(i % 8 == 0){
                printf(" ");
            }
        }
        if(k %2 == 1){
            printf("\n");
        }
        ptr = ptr +1;
    }
    printf("\n");
    pthread_mutex_unlock(&lp);
}

void dma_print_temp(unsigned int temp){
        int i;
        for (i = 31; i >= 0; i--) {
            unsigned int j = temp >> i;
            if (j & 1)
                printf("1");
            else
                printf("0");
        }
        printf("\n");
}

int dma_give_intfrag(){
    return fragsize;
}

void dma_print_page(int pno){
    
    int maxpagenum = (bitmapsize + segmentsize + 256) / 4096;
    if(pno >= maxpagenum || pno < 0){
        printf("Invalid pno\n");
    }
    else{
        unsigned int *ptr;
        ptr = bitmap;
        ptr = ptr + 1024 * pno;
        int j;
        pthread_mutex_lock(&lp);
        for(j = 0; j <1024; j++){
            //print
            if(j%8 ==0){
                printf("\n");
            }
            long  quot, remain;
            int i, k = 0;
            char hexadecimal[8] = {'0','0','0','0','0','0','0','0'};

            quot = *ptr;
            while (quot!= 0)
            {
                remain= quot % 16;
                if (remain < 10)
                    hexadecimal[k++] = 48 + remain;
                else
                    hexadecimal[k++] = 55 + remain;
                quot = quot / 16;
            }
            // display integer into character
            for (i = 7; i >= 0; i--)
                printf("%c", hexadecimal[i]);
            ptr = ptr +1;
        }
        pthread_mutex_unlock(&lp);
    }
    printf("\n");
}

void dma_print_blocks(){
    unsigned int* ptr = (unsigned int*)bitmap;
    unsigned int* ptr2 = (unsigned int*)segmentptr;
    int total_bit = bitmapsize * 8;
    int iteration = total_bit / 32;
    int iter_mod = total_bit%32;
    if(iter_mod != 0){
        iteration++;
    }
    int k;
    int free = 0;
    int allocated = 0;
    pthread_mutex_lock(&lp);
    for(k = 0; k < iteration; k++){
        unsigned int change_val_free= pow(2,31) + pow(2,30);
        unsigned int change_val_alloc= pow(2,30);
        unsigned int control_val_alloc =pow(2,30);
        int i;
        unsigned int result;
        unsigned int result2;
        for(i = 0; i <16; i++){
            result = (*ptr) & change_val_free;
            result2 = (*ptr) & change_val_alloc;
            if(result == change_val_free){ 
                if(free == 0 && allocated > 0){
                    printf("A, 0x%lx, 0x", (long)ptr2);
                    toHex(allocated*16);
                    printf(" (%d)\n",(allocated*16));
                    ptr2 = ptr2 + (allocated *4);
                }
                free++;
                allocated = 0;
            }
            else if(result2 == control_val_alloc){ 
                if(free > 0){
                    printf("F, 0x%lx, 0x", (long)ptr2);
                    toHex(free*16);
                    printf(" (%d)\n",(free*16));
                    ptr2 = ptr2 + (free *4);
                }
                else if(allocated > 0){
                    printf("A, 0x%lx, 0x", (long)ptr2);
                    toHex(allocated*16);
                    printf(" (%d)\n",(allocated*16));
                    ptr2 = ptr2 + (allocated *4);
                }
                allocated = 1;
                free = 0;
            }
            else{
                allocated++;
                free = 0;
            }
            change_val_free = change_val_free >> 2;
            change_val_alloc = change_val_alloc >> 2;
            control_val_alloc = control_val_alloc >>2;
        }
        ptr = ptr + 1;
    }
    if(free > 0){
        printf("F, 0x%lx, 0x", (long)ptr2);
        toHex(free*16);
        printf(" (%d)\n",(free*16));
    }
    else if(allocated > 0){
        printf("A, 0x%lx, 0x", (long)ptr2);
        toHex(allocated*16);
        printf(" (%d)\n",(allocated*16));
    }
    pthread_mutex_unlock(&lp);
}

void toHex(int size){
    long  quot, remain;
    int i, k = 0;
    char hexadecimal[100] = {'0'};

    quot = size;
    while (quot!= 0)
    {
        remain= quot % 16;
        if (remain < 10)
            hexadecimal[k++] = 48 + remain;
        else
            hexadecimal[k++] = 55 + remain;
        quot = quot / 16;
    }
    // display integer into character
    for (i = 99; i >= 0; i--)
        printf("%c", hexadecimal[i]);
}

