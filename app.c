#include "dma.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

static void *thread1(void *args){ 
    void* ptrx4 = dma_alloc(17);
    void* ptrx = dma_alloc(520);
    dma_free(ptrx4);
    dma_print_bitmap();
    dma_print_blocks();
    printf("internal fragmentation is in thread1:%d\n",dma_give_intfrag());
    pthread_exit(0);
}
static void *thread2(void *args){ 
    void* ptrx2 = dma_alloc(35);
    void* ptrx3 = dma_alloc(2);
    dma_free(ptrx2);
    dma_print_bitmap();
    dma_print_blocks();
    printf("internal fragmentation is in thread2:%d\n",dma_give_intfrag());
    pthread_exit(0);
}
static void *thread3(void *args){ 
    void* ptrx5 = dma_alloc(1020);
    unsigned int* ptr5 = (unsigned int*)ptrx5;
    *ptr5 = 45367283;
    ptr5++;
    *ptr5 = 36728931;
    void* ptrx6 = dma_alloc(65);
    unsigned int* ptr6 = (unsigned int*)ptrx6;
    *ptr6 = 12674856;
    dma_free(ptrx5);
    dma_print_bitmap();
    dma_print_blocks();
    dma_print_page(0);
    printf("internal fragmentation is in thread3:%d\n",dma_give_intfrag());
    pthread_exit(0);
}


int main(){
    dma_init(15);
    //create threads
    pthread_t t4[3];
    int ret;
    //create threads
    ret = pthread_create(&t4[0], NULL, thread1, NULL);
    if (ret != 0) 
        printf("thread create failed for first threads\n");
    ret = pthread_create(&t4[1], NULL, thread2, NULL);
    if (ret != 0) 
        printf("thread create failed for first threads\n");
    ret = pthread_create(&t4[2], NULL, thread3, NULL);
    if (ret != 0) 
        printf("thread create failed for first threads\n");
     
    //join all threads
  
    ret = pthread_join(t4[0], NULL);
    if (ret != 0) 
        printf("thread join failed for first threads\n");
    ret = pthread_join(t4[1], NULL);
    if (ret != 0) 
        printf("thread join failed for first threads\n");
    ret = pthread_join(t4[2], NULL);
    if (ret != 0) 
        printf("thread join failed for first threads\n");
        
    

    dma_print_bitmap();
    printf("internal fragmentation is in main:%d\n",dma_give_intfrag());
    //dma_free(ptrx4);
    //dma_free(ptrx);
    //dma_free(ptrx2);
    //dma_print_bitmap();

    //dma_print_page(0);

}