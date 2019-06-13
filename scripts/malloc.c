#include "malloc.h"

// this is our NULL pointer
#define NULL 0

// outsource the mmap wrappers to another file
extern void *mmap(void *addr, int len, int prot, int flags, int fd, int offset);
extern int munmap(void *addr, int len);

// for malloc, these are the appropriate settings to pass to mmap
// the numerical values are taken from /usr/include/bits/mman-linux.h
#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define PROT (PROT_READ | PROT_WRITE)
#define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS)

// these arrays store the allocated pointers and sizes for reference
// they need to be dynamic, so they start off uninitialized
#define INIT_SIZE 10
int table_size = 0;
void **malloc_ptr;
int *malloc_len;

// expand the global table to size len
void grow_table(int len) {

  // create an appropriately sized table
  void **new_ptr = mmap(NULL, len * sizeof(void *), PROT, FLAGS, -1, 0);
  int *new_len = mmap(NULL, len * sizeof(int), PROT, FLAGS, -1, 0);

  // clear the newly allocated memory
  for (int i = 0; i < table_size; i++) {
    new_ptr[i] = NULL;
    new_len[i] = 0;
  }

  // copy over the old data into the new arrays
  memcpy(new_ptr, malloc_ptr, table_size * sizeof(void *));
  memcpy(new_len, malloc_len, table_size * sizeof(int));

  // unless they were previously uninitialized, we should free the old arrays
  if (table_size > 0) {
    munmap(malloc_ptr, table_size * sizeof(void *));
    munmap(malloc_len, table_size * sizeof(int));
  }

  malloc_ptr = new_ptr;
  malloc_len = new_len;
  table_size = len;
}

// this is an ugly malloc implementation with an mmap for everything
void *malloc(int size) {

  // if size is 0, the Standard says to return NULL
  if (size == 0)
    return NULL;

  // if this is the first run, initialize the global arrays
  if (table_size == 0)
    grow_table(INIT_SIZE);

  // get the first available (empty) index
  // TODO: error handling?
  int index = -1;
  for (int i = table_size; i > 0; i--) {
    if (malloc_ptr[i - 1] == NULL) {
      index = i - 1;
    }
  }
  
  // if the mapping arrays are running low on space, expand them
  // we'll allocate new arrays with double the space
  if (table_size - index < 3)
    grow_table(table_size * 2);
  
  void *ptr = mmap(NULL, size, PROT, FLAGS, -1, 0);
  malloc_ptr[index] = ptr;
  malloc_len[index] = size;
  return ptr;
}

void free(void *ptr) {

  // if the pointer is NULL, the Standard says to do nothing
  if (ptr == NULL)
    return;

  // find the index of *ptr in the global mapping array
  int index = 0;
  for (int i = 0; i < table_size; i++) {
    if (malloc_ptr[i] == ptr) {
      index = i;
      break;
    }
  }

  // deallocate the memory
  munmap(ptr, malloc_len[index]);

  // shift all the elements in the mapping table down
  for (int i = index + 1; i < table_size; i++) {
    malloc_ptr[i - 1] = malloc_ptr[i];
    malloc_len[i - 1] = malloc_len[i];
  }
  malloc_ptr[table_size - 1] = NULL;
  malloc_len[table_size - 1] = 0;
}

// utility function: basic memcpy implementation
void *memcpy(void *dst, const void *src, int len) {
  char *dst_str = (char *)dst;
  char *src_str = (char *)src;

  for (int i = 0; i < len; i++)
    dst_str[i] = src_str[i];
  return dst;
}

