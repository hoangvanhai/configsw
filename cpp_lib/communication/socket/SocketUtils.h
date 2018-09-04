/**
 * @addtogroup communication
 * @{
 */ 
#ifndef __CONNECTION_SOCKETUTILS_H
#define __CONNECTION_SOCKETUTILS_H

#include "ISocket.h"
#include <mutex>
#include <condition_variable>

namespace communication{
    namespace utils{
        /**
         * @brief Network Utilities
         */ 
        class Network{
        public:
            Network();
            virtual ~Network();
            /**
             * @note if bufferSize = 0, ignore set it to socket
             */ 
            static int setBufferSize(ISocket* sock, int recvBufferSizeInByte, int sendBufferSizeInByte);
            static int getBufferSize(ISocket* sock, int *recvBufferSizeInByte, int *sendBufferSizeInByte);
        };
        /**
         * @brief CRC Utilities
         */ 
        class CRC{
        public:
            CRC();
            ~CRC();
            static uint16_t crc16(uint16_t init, const void* data, int dataLength);
            static std::shared_ptr<CRC> getInstance();
            static void destroyInstance();
        private:
            struct CRC16{
                int16_t polynomial;
                int16_t tbl[0x10000];
            };
            struct CRC16 m_crc16;
            static std::shared_ptr<CRC> m_inst;
            static std::once_flag m_initFlag;
        };
    }
}

#endif // SOCKETUTILS_H
/**
 * @}
 */ 
