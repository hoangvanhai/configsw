/**
 * @addtogroup communication_endpoint
 * @{
 */
#ifndef SERIAL_SOCKET_H
#define SERIAL_SOCKET_H

#include "../ISocket.h"
#include <string>

namespace communication{
    namespace endpoint{
        namespace serialport{
            /**
            * @brief Attribute ID
            */ 
            enum Attribute_ID{
                Attribute_PortName = 0,     ///< Set port name (and open)
                Attribute_Configuration,    ///< Set port configuration
                Attribute_ControlSignal,    ///< Get/Set control signals
                Attribute_CloseDevice,      ///< Close device
            };
            /**
            * @brief Attribute_PortName parameter
            */ 
            struct AttributePortName{
                std::string portname;       ///< Serial port name
            };
            enum Parity{
                Parity_None = 0,
                Parity_Odd,
                Parity_Even
            };
            enum StopBits{
                StopBits_1 = 0,
                StopBits_1_5,
                StopBits_2
            };
            enum WordBits{
                WordBits_5 = 0,
                WordBits_6,
                WordBits_7,
                WordBits_8,
            };
            /**
            * @brief Attribute_Configuration parameter
            */ 
            struct AttributeConfiguration{
                int           baudrate;                 ///< Baudrate, eg: 921600
                enum Parity   parity;                   ///< Parity mode
                enum StopBits stopBits;                 ///< Stop bits
                enum WordBits wordBits;                 ///< Word bits
                bool          useHardwareFlowControl;   ///< true - use cts-rts as flow control signals, false - disable
            };
            /**
            * @brief Attribute_ControlSignal parameter
            */ 
            struct AttributeControlSignal{
                bool cts;    // Clear To Send, INPUT
                bool rts;    // Request To Send, OUTPUT
                bool dsr;    // Data Set Ready, INPUT
                bool dtr;    // Data Terminal Ready, OUTPUT
            };
            /**
            * @brief Serial port socket
            * @note Must be has ROOT privilege to access I/O functions
            */ 
            class SerialSocket : public ISocket{
            public:
                SerialSocket();
                virtual ~SerialSocket();
                
                /* Override ISocket */
                virtual int writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                virtual int readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj);
                virtual int setAttribute(int attrId, void* attr, int attrSize);
                virtual int getAttribute(int attrId, void* attr, int attrSize);

#if (defined(_MSC_VER) || defined(__GNUC__))
				virtual int waitEvent(int eventMustToWait, int timeoutInMilisecond);
#endif
                
            private:
				void* m_handle;
                std::string m_portName;
                struct AttributeConfiguration m_cfg;
                
                int setPortName(struct AttributePortName *port);
                int getPortName(struct AttributePortName *port);
                int setConfiguration(struct AttributeConfiguration *cfg);
                int getConfiguration(struct AttributeConfiguration *cfg);    
                int setControlSignal(struct AttributeControlSignal* sig);
                int getControlSignal(struct AttributeControlSignal* sig);
                int closeDevice();
            };
        }
    }
}
#endif // UDPSOCKET_H
/**
 * @}
 */ 
