#include "StreamFraming.h"
#include "../../safe/Safe.h"

using namespace communication::protocol;

#if defined(__linux__)
#include <arpa/inet.h>
#elif defined(_MSC_VER)
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

int StreamFraming::Header::length(){ return ntohl(m_length); }
void StreamFraming::Header::setLength(int val){
	m_length = htonl(val);
}

StreamFraming::StreamFraming() : ILayer(),
m_rxState(RX_STATE_HEADER)
{
    m_frmAttr.maxSizeInByte = 65536*2;
    m_rxBuffer = new uint8_t[m_frmAttr.maxSizeInByte];
    m_txBuffer = new uint8_t[m_frmAttr.maxSizeInByte];
    m_rxBufferLength = 0;
    m_rxHdrIndex = 0;
    m_rxBufLen = 0;
    m_rxBufIndex = 0;
    m_rxHeader.setLength(0);
}
StreamFraming::~StreamFraming(){
    delete[] m_rxBuffer;
    delete[] m_txBuffer;
}
int StreamFraming::start(){return 0;}
int StreamFraming::stop(){return 0;}
int StreamFraming::reset(){return 0;}
int StreamFraming::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
        case Attribute_Frame:{
            struct AttributeFrame* frm = (struct AttributeFrame*)attr;
            m_frmAttr = *frm;
            if(m_rxBuffer) delete[] m_rxBuffer;
            if(m_txBuffer) delete[] m_txBuffer;
            m_rxBuffer = new uint8_t[m_frmAttr.maxSizeInByte];
            m_txBuffer = new uint8_t[m_frmAttr.maxSizeInByte];
            err = 0;
            break;
        }
        default:
            break;
    }
    
    return err;
}
int StreamFraming::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
        case Attribute_Frame:{
            struct AttributeFrame* frm = (struct AttributeFrame*)attr;
            *frm = m_frmAttr;
            err = 0;
            break;
        }
        default:
            break;
    }
    
    return err;
}
int StreamFraming::waitEvent(int eventNeedToWait, int timeoutInMilisecond){
    int event = 0;
    event = m_waitLower(eventNeedToWait, timeoutInMilisecond);
    return event;
}
int StreamFraming::writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    int wlen = 0;
    if(dataLength + (int)sizeof(Header) > m_frmAttr.maxSizeInByte) return 0;
    
    // write 4 byte length
    Header *hdr = static_cast<Header*>((void*)m_txBuffer);
    hdr->setLength(dataLength);
    cppframework::safe::s_memcpy(&m_txBuffer[sizeof(Header)], data, dataLength, dataLength);
    wlen = dataLength + sizeof(Header);
    m_wLower(m_txBuffer, wlen, nullptr);
        
    return wlen;
}
int StreamFraming::writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    int wlen = 0;
    
    uint8_t *pu8 = (uint8_t*)data;
    uint8_t* hdr = (uint8_t*)&m_rxHeader;

    while(dataLength > 0){

        if(m_rxState == RX_STATE_HEADER){
            hdr[m_rxHdrIndex]= *pu8;            
            m_rxHdrIndex++;
            dataLength --;
            pu8++;
            if(m_rxHdrIndex >= (int)sizeof(Header)){
                // recv all bytes of len
                m_rxHdrIndex = 0;
                m_rxState    = RX_STATE_DATA;
                m_rxBufLen   = 0;
                m_rxBufIndex = 0;
                
                int dlen = m_rxHeader.length();
                if(dlen > m_frmAttr.maxSizeInByte){
                    std::cout<<"rxlen maybe invalid "<<dlen<<std::endl;
                    if(m_error) m_error(Error_LengthInvalid, nullptr);
                    return 0;
                }
            }
        }

        if(m_rxState == RX_STATE_DATA){
            int dlen = m_rxHeader.length();
            
            int copyLen = dataLength;
            if(copyLen > dlen - m_rxBufLen){
                copyLen = dlen - m_rxBufLen;
            }
            if(copyLen > 0){
                cppframework::safe::s_memcpy(&m_rxBuffer[m_rxBufIndex], pu8, copyLen, copyLen);
            }
            m_rxBufLen   += copyLen;
            //std::cout << "m_rxBufLen: " << m_rxBufLen << std::endl;
            dataLength   -= copyLen;
            pu8          += copyLen;
            m_rxBufIndex += copyLen;
           if(m_rxBufIndex >= m_frmAttr.maxSizeInByte) m_rxBufIndex = m_frmAttr.maxSizeInByte-1;

            if(m_rxBufLen >= dlen){
                m_wUpper(m_rxBuffer, dlen, nullptr);
                m_rxState = RX_STATE_HEADER;
            }
        }
    }
    
    
    return wlen;
}
