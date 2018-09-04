
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <memory>
#include <cstdint>
#include <list>
#include <vector>
#include <mutex>
#include <atomic>

#include "../safe/Safe.h"

namespace cppframework{
    namespace container{
/**
 * @addtogroup cppframework
 * @{
 * @defgroup cppframework_container Module utils
 * @brief Module tools
 * @{
 */
class RingBuffer{
public:
    RingBuffer();
    virtual ~RingBuffer();
    /**
     * @brief Buffer in ring
     */
    class Buffer{
    public:
        explicit Buffer(int _size);
        Buffer(const Buffer& obj);
        virtual ~Buffer();
        /** 
         * @brief Write data to buffer
         * @param data Data
         * @param dataLength Data length
         * @param offset Offset of write pos in buffer
         * @return int Number of bytes are written
         */ 
        int write(std::shared_ptr<uint8_t> data, int dataLength, int offset = 0);
        /** 
         * @brief Write data to buffer
         * @param data Data
         * @param dataLength Data length
         * @param offset Offset of write pos in buffer
         * @return int Number of bytes are written
         */ 
        int write(void* data, int dataLength, int offset = 0);
        /**
         * @brief Return data pointer
         * @return std::shared_ptr<uint8_t> Data pointer
         */ 
        std::shared_ptr<uint8_t> getData();
        /**
         * @brief Return data pointer
         * @return uint8_t* Data pointer
         */
        uint8_t* getRawData();
        /**
         * @brief Return data size
         * @return int data size
         */ 
        int getDataLength();
    //
        std::shared_ptr<uint8_t> data;
        int  size;              ///< Data max size in bytes
        int  length;            ///< Current valid data length in bytes
        std::atomic<bool> empty;             ///< Mask buffer is empty
        std::atomic<bool> processing;
    };
    /**
     * @brief Initialize bufferCount buffers with bufferSize bytes in each
     * @param bufferSize    Size of buffer
     * @param bufferCount   Number of buffer in queue
     * @return 0 Success
     */
    int  init(unsigned int bufferSize, unsigned int bufferCount);
    /**
     * @brief Free all buffer in queue
     * @return 0 Success
     */
    int  destroy();
    /**
     * @brief Mask all buffer to empty
     */
    void clear();
    /**
     * @brief Return number of buffer is has data
     */
    int count();
    /**
     * @brief Return a buffer is empty in queue
     * @return int 0 if no more buffer is empty
     * @note if success, buffer must be free by call freeEmptyBuffer or push_back
     */
    std::shared_ptr<Buffer> allocEmptyBuffer();
    /**
     * @brief Free a empty buffer is alloced
     * @param buffer Empty buffer
     */
    void freeEmptyBuffer(std::shared_ptr<Buffer> buffer);
    /**
     * @brief Push buf back to queue, buf's data is not copied
     * @param buffer Buffer pointer must return by getEmptyBuffer
     * @return Size of data in buf
     */
    int   push_back(std::shared_ptr<Buffer> buffer);
    /**
     * @brief Return top valid buffer (has data)
     * @return 0 if no more valid buffer
     */
    std::shared_ptr<Buffer> front();
    std::shared_ptr<Buffer> at(int index);
    /**
     * @brief Mask top valid buffer to empty state
     * @return 0 Success
     */
    int   pop_front();
private:
    std::vector< std::shared_ptr<Buffer> > m_buffers;
    std::list< std::shared_ptr<Buffer> >   m_pendings;
    std::mutex m_lock;
};
    }}
#endif

/** @} @} */
