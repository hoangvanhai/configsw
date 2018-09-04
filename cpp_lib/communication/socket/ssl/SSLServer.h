/**
 * @addtogroup communication_endpoint
 * @{
 */ 
#ifndef __SSL_SERVER_H__
#define __SSL_SERVER_H__
#include "../tcp/TCPServer.h"

namespace communication{
    namespace endpoint{
        namespace ssl{
            /**
             * @brief Implement SSL socket as Server
            *  Attributes
                @code
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                |             ID            |           Attribute          |   Length   |            Desc                 | Action  |
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                | Attribute_SSL_CERT_KEY    | struct AttributeCertAndKey&  |   ignore   | setup certification & key file  | Get/Set |
                +---------------------------+------------------------------+------------+---------------------------------+---------+
                @endcode
            */ 
            class Server : public communication::endpoint::tcp::Server{
            public:
                /**
                 * @brief SSL server attributes
                 */ 
                enum Attribute_SSL_ID{
                    Attribute_SSL_CERT_KEY =  Attribute_TCP_SERVER_ID_END + 1,  ///< certification and key file
                };
                /**
                 * @brief Attributes for Attribute_SSL_CERT_KEY
                 */ 
                struct AttributeCertAndKey{
                    std::string certFile;       ///< Certification file
                    std::string keyFile;        ///< Key file
                };
                Server();
                virtual ~Server();
                
                /* Override ISocket Methods */
                virtual int setAttribute(int attrId, void* attr, int attrSize) override;
                virtual int getAttribute(int attrId, void* attr, int attrSize) override;
                /* Override IServer Methods */
                virtual std::shared_ptr<communication::ISocket> acceptClient() override;
            private:
                std::shared_ptr<class ServerInternal> m_internal;
                virtual std::shared_ptr<communication::ISocket> makeClientInstance() override;
            };
        }
    }
}
#endif
/**
 * @}
 */ 
