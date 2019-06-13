#include <stdlib.h>
#include <stdio.h>

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

