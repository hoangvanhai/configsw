#ifndef __HASH_MD5_H__
#define __HASH_MD5_H__

#include <cstdint>
#include <string>

namespace cppframework{
namespace hash{
/**
 * @addtogroup cppframework
 * @{
 * @defgroup cppframework_hash Hash utilities
 * @{
 * This package supports both compile-time and run-time determination of CPU
 * byte order.  If ARCH_IS_BIG_ENDIAN is defined as 0, the code will be
 * compiled to run only on little-endian CPUs; if ARCH_IS_BIG_ENDIAN is
 * defined as non-zero, the code will be compiled to run only on big-endian
 * CPUs; if ARCH_IS_BIG_ENDIAN is not defined, the code will be compiled to
 * run on either big- or little-endian CPUs, but will run slightly less
 * efficiently on either one than if ARCH_IS_BIG_ENDIAN is defined.
 */
class MD5{
public:
    MD5();
    MD5(const MD5& obj);
    virtual ~MD5();
    
    typedef uint8_t  md5_byte_t; /* 8-bit byte */
    typedef uint32_t md5_word_t; /* 32-bit word */
    /* Define the state of the MD5 Algorithm. */
    typedef struct md5_state_s {
        md5_word_t count[2];	/* message length in bits, lsw first */
        md5_word_t abcd[4];		/* digest buffer */
        md5_byte_t buf[64];		/* accumulate block */
    } md5_state_t;
    
    /** @brief Initialize the algorithm
     */
    void init();
    /** @brief Append a string to the message
     */
    void append(const void *data, int nbytes);
    /** @brief Finish the message and return the digest
     */
    void finish(void *digest);
    /**
     * @brief Encode data to md5 string (hex format)
     * @param data Data
     * @param dataLength Data length in bytes
     * @return std::string md5 sum in hex format
     */ 
    static std::string encode(const void* data, int dataLength);
private:
    md5_state_t *pms;
    void process(const md5_byte_t *data /*[64]*/);
    
    static int8_t bin2hex(int8_t val);
};
/**
 * @} @}
 */
}}

#endif /* md5_INCLUDED */
