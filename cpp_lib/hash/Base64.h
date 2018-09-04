#ifndef __HASH_BASE_64_H__
#define __HASH_BASE_64_H__
#include <string>

namespace cppframework{
namespace hash{
/**
 * @addtogroup cppframework_hash
 * @{
 */
/**
 * @brief Encoder and Decoder Base64 format
 */
class Base64{
public:
    Base64();
    virtual ~Base64();
    /**
     * @brief Encode data to Base64 format string
     * @param data Data
     * @param in_len Data length in bytes
     * @return std::string Base64 encoded string
     */
    static std::string encode(const void* data, unsigned int in_len);
    /**
     * @brief Decode Base64 string to data
     * @param encoded_string Base64 input string
     * @param data Output data
     * @param dataMaxLength Maximum of bytes of output data
     * @return int Number of bytes in output data
     */
    static int decode(const std::string& encoded_string, void* data, int dataMaxLength);
private:
    static inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }
    static const std::string base64_chars;
};
/**
 * @}
 */ 
}}

#endif

