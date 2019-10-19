---
layout: post
title: A (Bad) Libc-free Malloc Implementation
date: Jun 12, 2019
summary: "please, sir, I want some more memory"
permalink: malloc-no-libc
---

### introduction

I must apologize: I still haven't made any progress toward the project I originally intended to blog about (go figure), and this blog post is due tomorrow (also go figure). So, I'll actually draw this post from a different project that I've been making progress on: that [C library](https://github.com/air-wreck/ezlibc) that I mentioned in an [earlier post](https://blog.ericzheng.org/_posts/2018-09-18-Approximation/). The point of this project is to make a tiny little C library that provides a subset of the functionality of the C Standard Library (`libc`). This allows us to sort of "peer into" the often mysterious inner workings of computers and answer questions like: (a) what happens to my code when I compile it? or (b) what does my code actually do when it runs? So far, I've met with reasonable success (and since that earlier post, I have indeed implemented `arc` and `argv`, although I haven't bothered to test the Mac OS implementations). This post is about my (very naive) implementation of the classic `malloc` function in C.

In order to compile/use the code in this post, some technical prerequisites are necessary:
* A C compiler (of course): I'll be using GCC, but any C compiler should work
* An Intel x86 computer: x86_64 is also acceptable (and is what I'm using); if you don't know what this is, you probably have x86_64
* A computer running Linux

If you don't meet these requirements, you can still follow along, but you won't be able to run the code. Also, a familiarity with C programming is helpful, but I'll try to be very explicit about stuff as I go along.

---
### the `malloc` API

If you've done C programming before, you're doubtless familiar with the `malloc` and `free` functions (as well as its associated family members, like `calloc` and `realloc`). If not, this family of functions allows one to request/use dynamic memory from the operating system. In higher-level languages like Python, this is abstracted away: one can just instantiate an object without having to worry about how many bytes it takes up or when it needs more memory. With that in mind, let's dissect the call signature of the two most important functions in this group, `malloc` and `free`:

``` c
void *malloc(size_t size);
void free(void *ptr);
```

This tells us that `malloc` is a function that takes a `size_t` (the `sizeof` operator's return type, which here we can just pretend to be an `int`), which specifies how many bytes we wish to allocate from the operating system. It returns a pointer of type `(void *)`, which is really just a pointer to a block of memory that doesn't have a specific type (cf. `(int *)`, which clearly points to an integer). This pointer points to the block of memory that the operating system has allocated to us. Famously, one should always check if the return value of `malloc` is `NULL` before using it.

The `free` function, on the other hand, accepts a pointer of type `(void *)`; this pointer must have already been returned by a prior call to `malloc` or another of its friends. It will then deallocate the associated memory for us. This is used when we are finished with some memory that we have `malloc`'d. Famously, one should be careful not to call `free` on the same pointer twice (undefined behavior), and one should especially not use a pointer after it has been `free`'d (a major security hazard).

A usage example of these two functions might look like:

``` c
/* file: malloc_example.c */

#include <stdlib.h>
#include <stdio.h>

int main() {
  const int length = 10;  // arbitrary array length
  int *array;             // an uninitialized,  dynamically-sized integer array

  // we'll malloc some memory for the array
  // let's be naughty and not check if this is NULL before using
  array = malloc(length * sizeof(int));

  // let's fill up the array with some numbers
  for (int i = 0; i < length; i++) {
    array[i] = i + 13;
  }

  // to prove that this works, we'll print out the array
  for (int i = 0; i < length; i++) {
    printf("array[%d] is: %d\n", i, array[i]);
  }

  // return the allocated memory to the OS
  free(array);

  return 0;
}
```

And compiling and running the code works as expected:

``` sh
$ gcc malloc_example.c -o tmp
$ ./tmp
array[0] is: 13
array[1] is: 14
array[2] is: 15
array[3] is: 16
array[4] is: 17
array[5] is: 18
array[6] is: 19
array[7] is: 20
array[8] is: 21
array[9] is: 22
```

---
### wrapping the `mmap`/`munmap` syscalls

When we program without the benefit of the C Standard Library, we lose a lot of convenience. Rather than calling library functions, we'll have to interface directly with the operating system. The interface that many operating systems (including Linux, which you should totally use) use to interface with userspace programs is the system call interface (or syscalls, for short). This is exposed on the assembly/machine code level. System calls handle requests from userspace processes to do things like writing to a particular file, exiting a process, and (what's helpful to us) getting dynamic memory. If we wish to request such actions ourselves without the help of the standard library functions, we'll need to write some small assembly wrappers for these programs. Fortunately, at least on Linux, it's fairly trivial to interface between assembly and C programs.

For this exercise, I've chosen to implement the `malloc`/`free` API using the `mmap`/`munmap` syscalls. This is **not** what a typical `malloc` implementation does; a typical implementation, at least on Linux, will use `brk`/`sbrk` to grow and shrink the process's heap to allocate more memory. The `mmap`/`munmap` syscalls will still allocate memory in the process's virtual address space, but in a different place. I've chosen this route because it's significantly easier to implement, although it's not what you'd normally see.

I don't explain this part particularly well, so feel free to skip it (especially if you've never seen assembly programming before). Essentially, I'll make two small assembly functions (`sys_mmap` and `sys_munmap`) to wrap the `mmap` and `munmap` syscalls and expose them to the C code. Since the `mmap` argument structure is actually somewhat complicated, I also have further C wrapper functions.

The two assembly functions do some pretty generic stack frame manipulations to get their arguments. The actual syscalls are handled by the `movl $90, %eax` (move the number 90, the code for `mmap`, into the `eax` register) and `int $128` (issue a system call).

``` asm
; file: mmap.s

.global sys_mmap 
sys_mmap:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %ebx
  movl $90, %eax
  int $128
  popl %ebp
  ret

.global sys_munmap
sys_munmap:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %ecx
  movl 12(%ebp), %ebx
  movl $91, %eax
  int $128
  popl %ebp
  ret
```

The C wrappers are also fairly self-explanatory. The `mmap` syscall actually takes a pointer to its own special struct, probably because it has too many arguments to shove into registers.

``` c
/* file: mmap.c */

// this is the way the x86 Linux interface
// specifies the arguments to mmap
struct mmap_arg_struct {
  unsigned long addr;
  unsigned long len;
  unsigned long prot;
  unsigned long flags;
  unsigned long fd;
  unsigned long offset;
};

// these functions refer to our assembly subroutines
extern void *sys_mmap(struct mmap_arg_struct *args);
extern int sys_munmap(void *addr, int len);

// this is the ultimate mmap wrapper
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset) {
  struct mmap_arg_struct args;
  args.addr = (unsigned long) addr;
  args.len = len;
  args.prot = prot;
  args.flags = flags;
  args.fd = fd;
  args.offset = offset;

  return sys_mmap(&args);
}

// this is the ultimate munmap wrapper
int munmap(void *addr, int len) {
  return sys_munmap(addr, len);
}
```

---
### implementing `malloc` and `free`

Now that we have the C wrapper functions for `mmap` and `munmap`, we can proceed with writing the `malloc` function. Basically, it will boil down to calling `mmap` with certain settings each time we wish to allocate new memory. Actually, if not for the requirement that we be able to `free` memory later, `malloc` would be quite easy to write. As it is, we need to have some sort of meta-structure that keeps track of all the memory that we've allocated so far. This way, when the user asks us to deallocate some memory, we know where it is and how big it is. I've arbitrarily decided to use two parallel arrays to store the addresses and lengths of the allocated chunks.

However, there's a slight complication: the table needs to be able to grow dynamically to accommodate as many allocations as the user requests (at least until the system runs out of memory). So, funnily enough, we'll actually need to use `malloc` in our definition of `malloc`. The `grow_table` function will handle such increases. I'll dispense with most of narrative and instead just present the source code, with plenty of comments.

``` c
/* file: malloc.c */
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
```

We additionally have the corresponding C header file, `malloc.h`.

``` c
/* file: malloc.h */
#ifndef MALLOC_H
  #define MALLOC_H
  void *malloc(int size);
  void free(void *ptr);
  void *memcpy(void *dst, const void *src, int len);
#endif
```

---
### using our implementation

Well, does it work? I haven't done much thorough testing (especially with the whole table growing aspect), but we can pretty easily modify the original `malloc_example.c` to work with our `malloc`. Here's what the new file looks like:

``` c
/* file: main.c */
#include <stdio.h>
#include "malloc.h"

int main() {
  const int length = 10;  // arbitrary array length
  int *array;             // an uninitialized,  dynamically-sized integer array

  // we'll malloc some memory for the array
  array = malloc(length * sizeof(int));

  // let's fill up the array with some numbers
  for (int i = 0; i < length; i++) {
    array[i] = i + 13;
  }

  // to prove that this works, we'll print out the array
  for (int i = 0; i < length; i++) {
    printf("array[%d] is: %d\n", i, array[i]);
  }

  // return the allocated memory to the OS
  free(array);

  return 0;
}
```

We can compile everything with:

``` sh
$ gcc -m32 mmap.s mmap.c malloc.c main.c -o tmp
```

The `-m32` flag explicitly tells GCC to compile for the 32-bit architecture. If we run the file with `./tmp`, we get exactly the same output as before, with the "official" GNU libc implementation! This implies that if our function is wrong, it's at least rather subtly wrong.

---
### conclusion/further reading

Well, I hope you enjoyed that at least somewhat. If I explained myself decently, you got to see how one might write a very well-known library function like `malloc` from scratch, without the benefit of libc. I like doing this because it peels back the mystery of computers a bit.

I apologize for the alarming disorganization and lack of pretty graphics beyond the source code; these are symptoms of starting a blog post the night before it's due. As I said before, the `malloc` implementation in this post is not how most people would implement it, but my brain is very small, so you're stuck with my version. If you'd like to see an example of a legit one, [have fun](http://g.oswego.edu/dl/html/malloc.html). If you want to see a "beginner's" implementation that's still more legit than the one presented in this post, you can see a linked-list implementation [here](https://danluu.com/malloc-tutorial/). However, I will note that these implementations rely on libc, because any sane C programmer would write it that way.

If you'd like, all the source code referenced in this post is available here:
* [`malloc_example.c`](../../scripts/malloc_example.c)
* [`mmap.s`](../../scripts/mmap.s)
* [`mmap.c`](../../scripts/mmap.c)
* [`malloc.c`](../../scripts/malloc.c)
* [`malloc.h`](../../scripts/malloc.h)
* [`main.c`](../../scripts/main.c)

