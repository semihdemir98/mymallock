#include <stdio.h>
#include <stdlib.h>

typedef enum {bestFit, worstFit, firstFit} Strategy;

Strategy strategy = bestFit;


typedef struct{
    unsigned int size;
    unsigned int isfree;
}Info;

typedef struct{
   Info info;           /*number of  16 byte blocks*/
   struct block *next;  /*next free*/
   char data[0];        /*start of the allocated memory*/
 }Block;
/*Block: |4byte(size)|4byte(isfree)|8-byte next|0-byte(data)|*/

static Block *free_list = NULL;     /*start of the free list*/
static Block *heap_start = NULL;    /*head of allocated memory from sbrk */


void *mymalloc(size_t size){
  static int firstcall = 1;
  size = size - (size % 16) + 16;
  if(firstcall){
    //
    firstcall = 0;
    heap_start = sbrk(sizeof(Block)*2*1024);
    if(heap_start == NULL){
      perror("sbrk(heap_start) didnt work");
      return -1;
    }
    heap_start -> info.size = 1024;
    heap_start -> info.isfree = 1;
    heap_start -> next = NULL;
    // free alana 2 blok split et , birisini free listeye ekle
    // digerinin size kadar alandaki baslangic adresini return et.
    if(strategy == bestFit){}
    else if(strategy == worstFit){}
    else if(strategy == firstFit){ // firstfit
      Block * p_Free = free_list;
      while(p_Free -> next != NULL && 
            p_Free -> info.size < size || p_Free -> info.isfree == 0){

              p_Free = (void *)p_Free + sizeof(Block) + p_Free-> info.size;
      }
      if(p_Free == NULL){
        fprintf(stderr,"no available space\n");
        return NULL;
      }
      else if(size < p_Free -> info.size){
        split(p_Free,size);
      }
      p_Free->info.isfree = 1; // p yi remove et
      
      // Bu kisimi izin varsa ayri fonksiyonda yaz

      /* if(free_list == NULL){
        fprintf(stderr,"free_list is empty\n");
        return;
      }
      else{
        Block * temp = free_list;
        while(temp -> next != p_Free){
            temp = temp -> next;
        } 
        temp -> next = p_Free -> next; 
      } */
    }
    return NULL;
  }
}
void *myfree(void *p);
Block *split(Block *b, size_t size){
  int tempsize = b -> info.size;
  b-> info.size = size;
  Block * b_split = ((void*)b + b -> info.size + sizeof(Block));
  b_split = tempsize - size - sizeof(Block);
  // ?? tag ve atamalar bekleniyor
}
int test(int fitStrategy);
int main(){

}