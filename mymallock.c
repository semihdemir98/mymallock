#include <stdio.h>
#include <stdlib.h>

typedef enum
{
  bestFit,
  worstFit,
  firstFit
} Strategy;

Strategy strategy = bestFit;

typedef struct
{
  unsigned int size;
  unsigned int isfree;
} Info;

typedef struct
{
  Info info;          /*number of  16 byte blocks*/
  struct block *next; /*next free*/
  char data[0];       /*start of the allocated memory*/
} Block;
/*Block: |4byte(size)|4byte(isfree)|8-byte next|0-byte(data)|*/

static Block *free_list = NULL;  /*start of the free list*/
static Block *heap_start = NULL; /*head of allocated memory from sbrk */

void *mymalloc(size_t size)
{
  static int firstcall = 1;
  size = size - (size % 16) + 16;
  if (firstcall)
  {
    //
    firstcall = 0;
    heap_start = sbrk(sizeof(Block) * 2 * 1024);
    if (heap_start == NULL)
    {
      perror("sbrk(heap_start) didnt work");
      return -1;
    }
    heap_start->info.size = 1024;
    heap_start->info.isfree = 1;
    heap_start->next = NULL;
    //free list
    free_list = (void *)heap_start + sizeof(Block) + heap_start->info.size;
    free_list->info.size = 1024;
    free_list->info.isfree = 1;

    // free alana 2 blok split et , birisini free listeye ekle
    // digerinin size kadar alandaki baslangic adresini return et.

    if (strategy == bestFit)
    {
      return best_fit_operation(size);
    }
    else if (strategy == worstFit)
    {
      return worst_fit_operation(size);
    }
    else if (strategy == firstFit)
    {
      return first_fit_operation(size);
    }
    return NULL;
  }
}

void *remove_from_freelist(Block *p)
{
  if (free_list == NULL)
  {
    fprintf(stderr, "free_list is empty\n");
    return;
  }
  else
  {
    Block *temp = free_list;
    while (temp->next != p)
    {
      temp = temp->next;
    }
    temp->next = p->next;
  }
}
void *add_to_freelist(Block *p)
{
  if (p == NULL)
  {
    perror("error, null pointer");
    return NULL;
  }
  if (free_list == NULL)
  {
    free_list = p;
  }
  else
  {
    p->next = free_list;
    free_list = p;
  }
}
void *first_fit_operation(size_t size)
{
  Block *p_Free = free_list;
  while (p_Free->next != NULL &&
         p_Free->info.size < size ||
         p_Free->info.isfree == 0)
  {
    p_Free = (void *)p_Free + sizeof(Block) + p_Free->info.size;
  }
  if (p_Free == NULL)
  {
    fprintf(stderr, "no available space\n");
    return NULL;
  }
  else if (size < p_Free->info.size)
  {
    split(p_Free, size);
  }
  //p_Free->info.isfree = 1; // p yi remove et
  remove_from_freelist(p_Free);
  p_Free->info.isfree = 0;
  p_Free->info.size = size;
  return (void *)p_Free->data;
}

void *best_fit_operation(size_t size)
{
    Block *temp = free_list;
    Block *best = free_list;

    while(temp->next =! NULL){
      if(temp->info.size >= size &&
          temp->info.size <= best->info.size )
          {
            best = temp;
          }
          temp = temp->next;
    }
    if(temp == NULL && best == free){
      fprintf(stderr, "no available space\n");
      return NULL;
    }
    else{
      return (void*)best->data;
    }
}

void *worst_fit_operation(size_t size){
  Block *temp = free_list;
  Block *worst = free_list;

  while(temp->next != NULL){
      if(temp->info.size > worst->info.size){
        worst = temp;
      }
      temp = temp->next;
  }

  if(worst->info.size < size){
      fprintf(stderr, "no available space\n");
      return NULL;
  }
  else{
    split(worst,size);
  }
}
void *myfree(void *p)
{
  Block *header_free = (Block *)(p - sizeof(Block)); //block header
  header_free->info.isfree = 1;
  Block *next_free = p + header_free->info.size; // next block
  if (!next_free->info.isfree)
  {
    header_free->info.size += next_free->info.size;
  }
}
Block *split(Block *b, size_t size)
{
  int tempsize = b->info.size;
  b->info.size = size;
  Block *b_split = ((void *)b + b->info.size + sizeof(Block));
  b_split = tempsize - size - sizeof(Block);
  add_to_freelist(b_split);
  return b;
}
int test(int fitStrategy);
int main()
{
}