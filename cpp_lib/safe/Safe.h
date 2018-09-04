#ifndef __cppframework_SAFE_H__
#define __cppframework_SAFE_H__

#include <cstddef>


namespace cppframework{
    namespace safe{
        /**
        * @addtogroup cppframework
        * @{
        * @defgroup cppframework_safe Safe function
        * @{
        */
        /**
         * @brief The function copies min(n,dest_size) bytes from memory area src to memory area dest. The memory areas must not overlap
         * @param dest Destination buffer
         * @param src  Source buffer
         * @param n Number of bytes need to copy from src to dest
         * @param dest_size Size of destination buffer
         * @return void* The function returns a pointer to dest
         * @retval 0 Failed
         */
        void* s_memcpy(void* dest, const void* src, size_t n, size_t dest_size);
        /**
        * @}
        * @}
        */
    }
}


#endif
