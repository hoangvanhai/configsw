#ifndef __GETLINE_H__
#define __GETLINE_H__

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <cstdint>
#include <iostream>

#include "../string/StringUtils.h"

namespace cppframework{
namespace cli{
    /**
    * @addtogroup cppframework_cli
    * @{
    */
    /**
     * @brief Console in/out API
     */ 
    class Console{
    public:
        Console();
        ~Console();
        /**
         * @brief Set maximum of bytes are read by getline
         * @retval 0 Succefful
         */
        static int setInputMaxLength(int val);
        /**
         * @brief Set delimiter chars are used by getline
         * @retval 0 Successful
         */
        static int addInputDelimiter(std::string val);
        /**
         * @brief Get a line from stdin
         * @param passwdMode Password mode (not show input charaters), default=true
         * @return std::string Line
         */ 
        static std::string getline(bool passwdMode = false);
        /**
         * @brief Print with format string and multi arguments
           @see cppframework::str::Util::format
         */
        template <typename... Args>
        static void         print(std::string str, Args... args){
            std::cout<<str::Util::format(str, args...);
            fflush(stdout);
        }
        /**
         * @brief Flush all data in in/out stream
         */
        static void        flush();
        static std::shared_ptr<Console> getInstance();
        static void destroyInstance();
    private:
        static std::shared_ptr<Console> m_inst;
        static std::once_flag m_instFlag;
        int   m_bufSize;
        int   m_bufLen;
        std::shared_ptr<uint8_t> m_bufPtr;
        std::vector<char> m_inputDelimiter;
    };
    /**
    * @}
    */ 
}
}

#endif

