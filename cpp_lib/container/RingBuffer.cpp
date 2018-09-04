#include "RingBuffer.h"

using namespace cppframework::container;


RingBuffer::RingBuffer(){}
RingBuffer::~RingBuffer(){}
/**
    * @brief Initialize bufferCount buffers with bufferSize bytes in each
    * @param bufferSize    Size of buffer
    * @param bufferCount   Number of buffer in queue
    * @return 0 Success
    */
int  RingBuffer::init(unsigned int bufferSize, unsigned int bufferCount){
    while(bufferCount > 0){
        std::shared_ptr<Buffer> buf = std::make_shared<Buffer>(bufferSize);
        m_buffers.push_back(buf);
        bufferCount --;
    }
    return 0;
}
/**
    * @brief Free all buffer in queue
    * @return 0 Success
    */
int  RingBuffer::destroy(){
    m_buffers.clear();  
    return 0;
}
/**
    * @brief Mask all buffer to empty
    */
void RingBuffer::clear(){
    m_lock.lock();
    for(auto buf : m_buffers){
        buf->empty = true;
        buf->processing = false;
        buf->length = 0;
    }
    m_pendings.clear();
    m_lock.unlock();
}
/**
    * @brief Return number of buffer is has data
    */
int RingBuffer::count(){
    m_lock.lock();
    int ret = m_pendings.size();
    m_lock.unlock();
    return ret;
}
/**
    * @brief Return a buffer is empty in queue
    * @return int 0 if no more buffer is empty
    * @note if success, buffer must be free by call freeEmptyBuffer or push_back
    */
std::shared_ptr<RingBuffer::Buffer> RingBuffer::allocEmptyBuffer(){
    std::shared_ptr<Buffer> ret = nullptr;
    m_lock.lock();
    for(auto buf : m_buffers){
        if(buf->empty && !buf->processing){
            buf->processing = true;
            ret = buf;
            break;
        }
    }
    m_lock.unlock();
    return ret;
}
/**
    * @brief Free a empty buffer is alloced
    * @param buffer Empty buffer
    */
void RingBuffer::freeEmptyBuffer(std::shared_ptr<Buffer> buffer){
    buffer->processing = false;
}
/**
    * @brief Push buf back to queue, buf's data is not copied
    * @param buffer Buffer pointer must return by getEmptyBuffer
    * @return Size of data in buf
    */
int   RingBuffer::push_back(std::shared_ptr<Buffer> buffer){
    m_lock.lock();
    if(buffer){
        buffer->empty = false;
        buffer->processing = false;
        m_pendings.push_back(buffer);
    }
    m_lock.unlock();
    return buffer->length;
}
/**
    * @brief Return top valid buffer (has data)
    * @return 0 if no more valid buffer
    */
std::shared_ptr<RingBuffer::Buffer> RingBuffer::front(){
    std::shared_ptr<Buffer> ret = nullptr;
    m_lock.lock();
    if(m_pendings.size() > 0){
        ret = m_pendings.front();
    }
    m_lock.unlock();
    return ret;
}
std::shared_ptr<RingBuffer::Buffer> RingBuffer::at(int index){
    std::shared_ptr<Buffer> ret = nullptr;
    m_lock.lock();
    if(index >= 0 && index < (int)m_pendings.size()){
        std::list< std::shared_ptr<Buffer> >::iterator iter = m_pendings.begin();
        while(index > 0){
            ++iter;
            index--;
        }
        ret = *iter;
    }
    m_lock.unlock();
    return ret;
}
/**
    * @brief Mask top valid buffer to empty state
    * @return 0 Success
    */
int   RingBuffer::pop_front(){
    m_lock.lock();
    if(m_pendings.size() > 0){
        std::shared_ptr<Buffer> buf = m_pendings.front();
        m_pendings.pop_front();
        buf->empty = true;
    }
    m_lock.unlock();
    return 0;
}

RingBuffer::Buffer::Buffer(int _size):
data(std::shared_ptr<uint8_t>(new uint8_t[_size], [](uint8_t* p){ delete[] p;})),
empty(true),
processing(false)
{
    size = _size;
    length = 0;
}
RingBuffer::Buffer::Buffer(const Buffer& obj):
data(std::shared_ptr<uint8_t>(new uint8_t[obj.size], [](uint8_t* p){ delete[] p;})),
empty(true),
processing(false)
{
    size = obj.size;
    length = 0;
}
RingBuffer::Buffer::~Buffer(){}
/** 
    * @brief Write data to buffer
    * @param data Data
    * @param dataLength Data length
    * @param offset Offset of write pos in buffer
    * @return int Number of bytes are written
    */ 
int RingBuffer::Buffer::write(std::shared_ptr<uint8_t> _data, int dataLength, int offset){
    if(dataLength <= 0) return 0;
    if(dataLength+offset > size) dataLength = size-offset;
    cppframework::safe::s_memcpy(&((uint8_t*)data.get())[offset], _data.get(), dataLength, size);
    length = dataLength;
    return dataLength;
}
/** 
    * @brief Write data to buffer
    * @param data Data
    * @param dataLength Data length
    * @param offset Offset of write pos in buffer
    * @return int Number of bytes are written
    */ 
int RingBuffer::Buffer::write(void* _data, int dataLength, int offset){
    if(dataLength <= 0) return 0;
    if(dataLength+offset > size) dataLength = size-offset;
    cppframework::safe::s_memcpy(&((uint8_t*)data.get())[offset], _data, dataLength, size);
    length = dataLength;
    return dataLength;
}
/**
    * @brief Return data pointer
    * @return std::shared_ptr<uint8_t> Data pointer
    */ 
std::shared_ptr<uint8_t> RingBuffer::Buffer::getData(){return data;}
/**
    * @brief Return data pointer
    * @return uint8_t* Data pointer
    */
uint8_t* RingBuffer::Buffer::getRawData(){return data.get();}
/**
    * @brief Return data size
    * @return int data size
    */ 
int RingBuffer::Buffer::getDataLength(){return length;}

// end of file
