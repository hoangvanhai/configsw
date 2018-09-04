/**
 * @addtogroup communication_protocol
 * @{
 *
 */ 
#ifndef __CONNECTION_STREAM_FRAMING_H__
#define __CONNECTION_STREAM_FRAMING_H__

#include "ILayer.h"
namespace communication{
    namespace protocol{
        /**
         * @brief Chuyen doi dong byte (vd: TCP) thanh cac frame va nguoc lai
         @code
             |len| data 1 |len| data 2 |len| data 3 |len| data 4 | as reliable stream
         <=> | Frame 1 | Frame 2 | Frame 3 | Frame 4 | as array of frames
         @endcode
         */ 
        class StreamFraming : public ILayer{
        public:
            enum ErrorCode{
                Error_Unknown = 0,
                Error_LengthInvalid
            };
            enum Attribute_ID{
                Attribute_Frame = 0,
            };
            struct AttributeFrame{
                int maxSizeInByte;
            };
            StreamFraming();
            virtual ~StreamFraming();
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
                RX_STATE_HEADER = 0,
                RX_STATE_DATA
            };
            enum RX_STATE m_rxState;
            uint8_t      *m_rxBuffer;
            int           m_rxBufferLength;
            uint8_t      *m_txBuffer;
            
            typedef struct{
                uint32_t m_length;
				int length();
				void setLength(int val);
            }Header;
            Header        m_rxHeader;
            int           m_rxHdrIndex;
            int           m_rxBufLen;
            int           m_rxBufIndex;
        };
    }
}

#endif // SERIALFRAMING_H
/** @}
 */ 
