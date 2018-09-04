/**
 * @addtogroup communication_endpoint
 * @{
 */ 
#ifndef __SSL_CLIENT_H__
#define __SSL_CLIENT_H__
#include "../tcp/TCPClient.h"

namespace communication{
    namespace endpoint{
        namespace ssl{
            /**
             * @brief Implement SSL socket as Client
             *  Attributes
                @code
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                |             ID            |           Attribute          |   Length   |            Desc                 | Action  |
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                @endcode
            */ 
            class Client : public communication::endpoint::tcp::Client{
            public:
                
                enum Attributes_SSL_CLIENT{
                    Attribute_SSL_INFO = Attribute_CLIENT_TCP_SOCKET_ID_END + 1,
                };
                enum Key_Type{
                    KEY_TYPE_UNKNOWN = 0,
                    KEY_TYPE_RSA,
                    KEY_TYPE_DSA,
                };
                enum CONST_VAL{
                    KEY_PUBLIC_LENGTH    = 2048,
                    SUBJECT_NAME_LENGTH  = 128,
                    CIPHER_NAME_LENGTH   = 128,
                };
                struct AttributeSSLInfo{
                    enum Key_Type keyType;
                    int           keyBitLength;
                    int8_t        keyPublic[KEY_PUBLIC_LENGTH];
                    int8_t        subjectName[SUBJECT_NAME_LENGTH];
                    int8_t        cipherName[CIPHER_NAME_LENGTH];
                    int           cipherBitLength;
                };
                
                explicit Client(bool initSSL = true);
                virtual ~Client();
                
                /* Override ISocket Methods */
                int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj) override;
                int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj) override;
                virtual int getAttribute(int attrId, void* attr, int attrSize) override;
                /* Override IClient Methods */
                /**
                 * @note This API is blocking mode, waitTimeoutInMilisecond is ignored
                 */
                int connectToServer(int waitTimeoutInMilisecond) override;
                int disconnectToServer() override;
                
                int setSSLContext(void* ctx);
            private:
                std::shared_ptr<class ClientInternal> m_internal;
                int getInformation(struct AttributeSSLInfo* info);
            };
        }
    }
}
#endif
/** @}
 */ 
