/**
 * @defgroup communication Communication Utilities
 * @addtogroup communication
 * @{
 */ 
#ifndef __CONNECTION_I_SOCKET_H__
#define __CONNECTION_I_SOCKET_H__
#include <string>
#include <memory>
#include <iostream>
#include <list>
#include <cstdlib>
namespace communication{
    /**
     * @brief Error code
     */ 
    enum ErrorCode{
        ErrorCode_None = 0,     ///< No error
        ErrorCode_Unknown,      ///< Unknown error
    };
    /**
     * @brief Event value, real event is OR (|) of this
     */ 
    enum Event{
        Event_Timeout  = 0x00,  ///< Timeout
        Event_Readable = 0x01,  ///< Ready to read
        Event_Writable = 0x02,  ///< Ready to write
        Event_Error    = 0x04   ///< Error 
    };

    enum Status {
        Status_Disconnected,
        Status_Connected,
        Status_Device_Reconnected,
        Status_Device_Disconnected
    };

    enum Constant{
#ifdef __linux__
        SOCKET_INVALID = -1,    ///< Invalid socket ID
#elif defined(_MSC_VER)
		SOCKET_INVALID = (~0),
#endif
    };
    class Object{
    public:
        Object(){}
        virtual ~Object(){}
    };
    class Error{
    public:
        explicit Error(int code);
        virtual ~Error();
        
        int  code();
        void setCode(int code);
        virtual std::string toString();
    private:
        int m_errorCode;
    };
    /**
     * @brief Socket interface for a connection
     */ 
    class ISocket{
    public:
        ISocket();
        virtual ~ISocket();
        /**
         * @brief Get the last error object
         */ 
        virtual std::shared_ptr<Error> getLastError();
        /**
         * @brief Monitor socket and return events
         * @param eventMustToWait Monitor event
         * @param timeoutInMilisecond Timeout in mili-second
         * @return int OR (|) of events
         */ 
        virtual int waitEvent(int eventMustToWait, int timeoutInMilisecond);
        /**
         * @brief Write data to socket
         */
        virtual int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj = nullptr) = 0;
        /**
         * @brief Read data from socket
         */ 
        virtual int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj = nullptr) = 0;
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
         * @brief Set socket to blocking mode
         * @retval 0 Successful
         */ 
        virtual int setBlockingMode();
        /**
         * @brief Set socket to non-blocking mode
         * @retval 0 Successful
         */ 
        virtual int setNonBlockingMode();
        /**
         * @brief Set socket's id, use in extern-initialized case
         */ 
        void        setSocketID(int id);
        /**
         * @brief Get current socket's id
         */ 
        int         getSocketID();
        /**
         * @brief Get the last socket's events, use with waitMultiEvent
         */ 
        int         lastEvent();
        /**
         * @brief Wait for event in multi sockets
         */ 
        static int  waitMultiEvent(int eventMustToWait, int timeoutInMilisecond, std::list<std::shared_ptr<ISocket>> socks);
    protected:
        std::shared_ptr<Error> m_error;
        int                    m_sockID;
        int                    m_lastEvent;
        
        void setErrorCode(enum ErrorCode code);
    };
    /**
     * @brief Server interface
     */ 
    class IServer{
    public:
        IServer(){}
        virtual ~IServer(){}
        /**
         * @brief Start listen connection from clients
         * @retval 0 Successful
         */
        virtual int startListen() = 0;
        /**
         * @brief Wait and accept new connection from client
         * @return std::shared_ptr<ISocket> New client socket
         * @retval nullptr Error or no connection
         */ 
        virtual std::shared_ptr<ISocket> acceptClient() = 0;
    };
    /**
     * @brief Client's interface
     */ 
    class IClient{
    public:
        IClient(){}
        virtual ~IClient(){}      
        /**
         * @brief Connect to server
         * @param waitTimeoutInMilisecond Timeout for connect operation
         * @retval 0 Successful
         */ 
        virtual int connectToServer(int waitTimeoutInMilisecond) = 0;
        /**
         * @brief Disconnect to server
         * @retval 0 Successful
         */ 
        virtual int disconnectToServer() = 0;
    };
}

#endif
/**
 * @defgroup communication_endpoint Endpoint
 * @}
 */ 
