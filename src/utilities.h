#ifndef __UTILITIES_H__
    #define __UTILITIES_H__
#include <Arduino.h>

size_t calcFreeMemorySpaceSize() {
  unsigned int target = 512, delta = 512;
  unsigned char* spacePtr = nullptr;
  while(!(delta <= 0)) {
    spacePtr = (unsigned char*)malloc((size_t)target * sizeof(unsigned char));
    if(spacePtr) {  // free space size > target
      target += delta;
      free(spacePtr);
    }else {         // free space size < target
      delta = delta / 2;
      target -= delta;
    }
  }
  return (size_t)target;
}

size_t calcFreeMemory2() {
  extern int __heap_start, *__brkval;
  uint8_t v;
  return (int)&v - (__brkval==0? (int) &__heap_start : (int) __brkval);  // include the heap
}

#endif