/* 01.04.2020
 * Semih Demir
 * 18120205005 
 * hw03.cpp
 * making malloc prototype
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void *remove_from_freelist(Block *p)//removing block from the freelist
{
  if (free_list == NULL)
  {
    fprintf(stderr, "free_list is empty\n");
    return NULL;
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
  return;
}

void *add_to_freelist(Block *p)//adds block in freelist
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
  return;
}

Block *split(Block *b, size_t size)// splits area and separetes two pieces
{
  int tempsize = b->info.size;
  b->info.size = size;
  Block *b_split = ((void *)b + b->info.size + sizeof(Block));
  b_split->info.size = tempsize - size - sizeof(Block);
  add_to_freelist(b_split);
  return b_split;
}

void *first_fit_operation(size_t size) // first fitting - finds the first usable and enough space in the freelist
{
  if (free_list->next == NULL)
  {
    Block *b = split(free_list, size);
    return (void *)b->data;
  }
  Block *p_Free = free_list;
  while (p_Free->next != NULL)
  {
    if (p_Free->info.size > size)
    {
      remove_from_freelist(p_Free);
      Block *p = split(p_Free, size);
      return (void *)p->data;
    }
    p_Free = p_Free->next;
  }
  if (p_Free == NULL)
  {
    fprintf(stderr, "no available space\n");
    return NULL;
  }
  return NULL;
}

void *best_fit_operation(size_t size) // best fitting - finds the smallest size in the freelist
{
  Block *temp = free_list;
  Block *best = free_list;

  while (temp->next != NULL)
  {
    if (temp->info.size >= size &&
        temp->info.size <= best->info.size)
    {
      best = temp;
    }
    temp = temp->next;
  }
  if (temp == NULL && best == free)
  {
    fprintf(stderr, "no available space\n");
    return NULL;
  }
  else
  {
    remove_from_freelist(best);
    return (void *)best->data;
  }
}

void *worst_fit_operation(size_t size) // worst fitting - finds the largest space in the freelist
{
  Block *temp = free_list;
  Block *worst = free_list;

  while (temp->next != NULL)
  {
    if (temp->info.size > worst->info.size)
    {
      worst = temp;
    }
    temp = temp->next;
  }

  if (worst->info.size < size)
  {
    fprintf(stderr, "no available space\n");
    return NULL;
  }
  else
  {
    split(worst, size);
  }
  remove_from_freelist(worst);
  return (void *)worst->data;
}

void *mymalloc(size_t size) // allocates space in the heap and manages
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
      return NULL;
    }
    heap_start->info.size = 1024;
    heap_start->info.isfree = 1;
    free_list = heap_start;

    if (free_list == NULL)
    {
      perror("no avaible space!");
    }

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
  }
  return;
}

void *myfree(void *p) // the field is released and added to the freelist
{
  Block *header_free = (Block *)(p - sizeof(Block));
  header_free->info.isfree = 1;
  add_to_freelist(header_free);
  return (void *)header_free;

  /* Block *header_free = (Block *)(p - sizeof(Block)); //block header
  header_free->info.isfree = 1;
  Block *next_free = p + header_free->info.size; // next block
  if (!next_free->info.isfree)
  {
    header_free->info.size += next_free->info.size;
  }

  Block *prev_free = (void*)header_free - sizeof(Block);//previous block
  if(!prev_free->info.isfree){
    prev_free->info.size += header_free->info.size + sizeof(Block);
    header_free=prev_free;
  }
  add_to_freelist(header_free);
  return (void*)header_free; */
}

int test(int fitStrategy)//testing parts
{
  strategy = fitStrategy;
  for (int i = 0; i < 16; i++)
  {
    size_t random = rand() % 128;
    int *x = mymalloc(random);
    if (x == NULL)
    {
      return -1;
    }
    *x = random;
    printf("%d\n", *x);
    myfree(x);
  }
  return 1;
}

int main()
{
  /* test(firstFit);
  printf("firstfit\n");
  test(worstFit);
  printf("worstfit\n");
  test(bestFit);
  printf("bestfit\n"); */
}