/**
 * @addtogroup communication
 * @{
 * @defgroup communication_protocol Protocol
 * @{
 *
 */ 
#ifndef SERIALFRAMING_H
#define SERIALFRAMING_H

#include "ILayer.h"

namespace communication{
    namespace protocol{
        /**
         * @brief Framing a serial data use HDLC-like byte framing
         */ 
        class SerialFraming : public ILayer{
        public:
            /**
             * @brief Object for writeToLower call
             */ 
            class WriteToLowerObject : public Object{
            public:
                WriteToLowerObject();
                virtual ~WriteToLowerObject();
                /**
                 * @brief Check has more data will be written to layer
                 */ 
                bool hasMoreData();
                /**
                 * @brief Set more data will be written to layer
                 */ 
                void setMoreData(bool set);
            private:
                bool m_hasMoreData;
            };
            /**
             * @brief Atrributes for layer
             */ 
            enum Attribute_ID{
                Attribute_Frame = 0,    ///< Set frame parameters
            };
            /**
             * @brief Attributes for Attribute_Frame
             */ 
            struct AttributeFrame{
                int maxSizeInByte;      ///< Maximum of bytes in a frame
            };
            SerialFraming();
            virtual ~SerialFraming();
            /* Override ILayer */
            int setAttribute(int attrId, void* attr, int attrSize) override;
            int getAttribute(int attrId, void* attr, int attrSize) override;
            int writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr) override;
            int writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr) override;
            int waitEvent(int eventNeedToWait, int timeoutInMilisecond) override;
            int start() override;
            int stop() override;
            int reset() override;
        private:
            struct AttributeFrame   m_frmAttr;
            enum RX_STATE{
                RX_STATE_WAIT_FRAME = 0,
                RX_STATE_IN_FRAME
            };
            enum RX_STATE m_rxState;
            uint8_t      *m_rxBuffer;
            int           m_rxBufferLength;
            bool          m_isEspace;
        };
    }
}

#endif // SERIALFRAMING_H
/** @} @}
 */ 
