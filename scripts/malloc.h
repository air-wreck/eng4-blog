#ifndef MALLOC_H
  #define MALLOC_H
  void *malloc(int size);
  void free(void *ptr);
  void *memcpy(void *dst, const void *src, int len);
#endif

