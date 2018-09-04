#include "Safe.h"
#include <cstring>
void* cppframework::safe::s_memcpy(void* dest, const void* src, size_t n, size_t dest_size){
    if(n > dest_size) n = dest_size;

//     typedef void* (*memcpy_dll)(void* dest, const void* src, size_t n);
//     memcpy_dll _memcpy = &memcpy;
//     if(_memcpy){
//         return _memcpy(dest, src, n);
//     }
//     return 0;
    return memmove(dest, src, n);
}
// end of file

