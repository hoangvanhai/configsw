#ifndef __cppframework_random_h__
#define __cppframework_random_h__

namespace cppframework{
    namespace rand{
        /**
        * @addtogroup cppframework
        * @{
        * @defgroup cpp_framework_random Random
        * @{
 */
        /**
         * @brief Implement ramdom
         */
        class Random{
        public:
            /**
             * @brief Generate 'len' bytes random to 'data' buffer
             * @param data Data buffer
             * @param len Data buffer length
             * @return int Size (in bytes) of random data is generated
             */
            static int generate(void* data, int len);
            /**
             * @brief Return a random integer in [min; max] range
             * @param min Minimum value range
             * @param max Maximum value range
             * @return int Return value
             */
            static int generate(int min, int max);
        };
        /**
        * @}
        * @}
        */
    }
}

#endif
