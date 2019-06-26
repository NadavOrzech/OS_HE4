
#include <stdlib.h>

#define MAX_SIZE 100000000

void* malloc(size_t size){
    if(size == 0 || size > MAX_SIZE)
        return NULL;

    void* retval = sbrk(size);
    if(retval == (void*)(-1))
        return NULL;

    return retval;
}
