/**
 * @addtogroup communication
 * @{
 */ 
#ifndef __CONNECTION_I_LAYER_H__
#define __CONNECTION_I_LAYER_H__

#include "../socket/ISocket.h"
#include <memory>
#include <functional>

namespace communication{
    /**
     * @brief Protocol layer interface
     @code
            Upper layer
     +--+-------------^--+
        |             |
        v             +
              writeToUpperLayer
  writeToLower    Callback
        +             ^
        |             |
     +--v-------------+--+
     |                   |
     |       Layer       |
     |                   <---------+
     +--+-------------^--+         |
        |             |            |
        v             +            v
writeToLowerLayer            waitLowerEvent
     Callback    writeToUpper   Callback
        +             ^            ^
        |             |            |
     +--v-------------+--+         |
          Lower layer              |
                         <---------+
     @endcode
     */ 
    class ILayer{
    public:
        ILayer(){
        }
        virtual ~ILayer(){}
        /**
         * @brief Set socket's attribute
         * @retval 0 Successful
         */ 
        virtual int setAttribute(int attrId, void* attr, int attrSize = 0) = 0;
        /**
         * @brief Get socket's attribute
         * @retval 0 Successful
         */
        virtual int getAttribute(int attrId, void* attr, int attrSize = 0) = 0;
        /**
         * @brief Write data to lower layer
         */ 
        virtual int writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr) = 0;
        /**
         * @brief Write data to upper layer
         */ 
        virtual int writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr) = 0;
        /**
         * @brief Wait event
         */ 
        virtual int waitEvent(int eventNeedToWait, int timeoutInMilisecond) = 0;
        /**
         * @brief Start protocol
         */ 
        virtual int start() = 0;
        /**
         * @brief Stop protocol
         */
        virtual int stop()  = 0;
        /**
         * @brief Reset protocol
         */
        virtual int reset() = 0;
        
        /**
         * @brief Set callback function
         * @param writeToLowerLayerCallback Write data to lower layer
         * @param writeToUpperLayerCallback Write data to upper layer
         * @param waitLowerEvent Wait lower layer event
         */ 
        int setCallback(
            std::function<int(const void* data, int dataLength, std::shared_ptr<communication::Object> obj)> writeToLowerLayerCallback,
            std::function<int(const void* data, int dataLength, std::shared_ptr<communication::Object> obj)> writeToUpperLayerCallback,
            std::function<int(int event, int timeout)> waitLowerEvent,
            std::function<int(int code, std::shared_ptr<communication::Object> obj)> error = nullptr
            ){
            if(writeToLowerLayerCallback != nullptr) m_wLower = writeToLowerLayerCallback;
            if(writeToUpperLayerCallback != nullptr) m_wUpper = writeToUpperLayerCallback;
            if(waitLowerEvent != nullptr) m_waitLower = waitLowerEvent;
            if(error) m_error = error;
            return 0;
        }
    protected:
        std::function<int(const void* data, int dataLength, std::shared_ptr<communication::Object> obj)> m_wLower;
        std::function<int(const void* data, int dataLength, std::shared_ptr<communication::Object> obj)> m_wUpper;
        std::function<int(int event, int timeout)> m_waitLower;
        std::function<int(int code, std::shared_ptr<communication::Object> obj)> m_error;
    };
    /**
     * @brief Packet interface
     */ 
    class IPacket{
    public:
        virtual ~IPacket(){}
        virtual int      importFromRaw(const void* raw, int rawLen) = 0;

        virtual void*    rawData()          = 0;
        virtual int      rawDataLength()    = 0;
        virtual void*    data()             = 0;
        virtual int      dataLength()       = 0;
        virtual bool     checkValid()       = 0;
        virtual int      receiveSequence()  = 0;
        virtual int      sendSequence()     = 0;
        virtual std::string toString()      = 0;
        
        /**
         * @brief Check packet is a RR command
         */ 
        virtual bool     isReceiveReady()   = 0;
        /**
         * @brief Check packet is a RR request command
         */ 
        virtual bool     isReceiveReadyRequest() = 0;
        virtual bool     isReject()         = 0;
        virtual bool     isReset()          = 0;
        virtual bool     isDataMessage()    = 0;
        virtual bool     isControlMessage() = 0;
    };
}

#endif
/**
 * @}
 */ 

