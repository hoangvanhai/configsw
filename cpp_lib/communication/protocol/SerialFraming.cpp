#include "SerialFraming.h"

using namespace communication::protocol;

SerialFraming::WriteToLowerObject::WriteToLowerObject() : Object(){
    m_hasMoreData  = false;
}
SerialFraming::WriteToLowerObject::~WriteToLowerObject(){}
bool SerialFraming::WriteToLowerObject::hasMoreData(){
    return m_hasMoreData;
}
void SerialFraming::WriteToLowerObject::setMoreData(bool set){
    m_hasMoreData = set;
}

SerialFraming::SerialFraming() : ILayer(),
m_rxState(RX_STATE_WAIT_FRAME)
{
    m_frmAttr.maxSizeInByte = 65536;
    m_rxBuffer = new uint8_t[m_frmAttr.maxSizeInByte];
    m_rxBufferLength = 0;
    m_isEspace = false;
}
SerialFraming::~SerialFraming(){
    delete[] m_rxBuffer;
}
int SerialFraming::start(){return 0;}
int SerialFraming::stop(){return 0;}
int SerialFraming::reset(){return 0;}
int SerialFraming::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
        case Attribute_Frame:{
            struct AttributeFrame* frm = (struct AttributeFrame*)attr;
            m_frmAttr = *frm;
            if(m_rxBuffer) delete[] m_rxBuffer;
            m_rxBuffer = new uint8_t[m_frmAttr.maxSizeInByte];
            err = 0;
            break;
        }
        default:
            break;
    }
    
    return err;
}
int SerialFraming::getAttribute(int attrId, void* attr, int attrSize){
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
int SerialFraming::waitEvent(int eventNeedToWait, int timeoutInMilisecond){
    int event = 0;
    event = m_waitLower(eventNeedToWait, timeoutInMilisecond);
    return event;
}
int SerialFraming::writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    int wlen = 0;
    
    const uint8_t* u8p = (const uint8_t*)data;
    uint8_t  u8espace[2];
    int dataIndex   = 0;
    int pendingLen  = 0;
    u8espace[0]     = 0x7D;
    
    uint8_t u8data = 0x7E;
    wlen += m_wLower(&u8data, 1, nullptr);
    // find 0x7E or 0x7D
    while(dataIndex < dataLength){
        u8data = *u8p;
        if(u8data == 0x7E){
            if(pendingLen > 0){
                wlen += m_wLower(u8p - pendingLen, pendingLen, nullptr);
                pendingLen = 0;
            }
            u8espace[1] = 0x5E;
            wlen += m_wLower(u8espace, 2, nullptr);
        }else if(u8data == 0x7D){
            if(pendingLen > 0){
                wlen += m_wLower(u8p - pendingLen, pendingLen, nullptr);
                pendingLen = 0;
            }
            u8espace[1] = 0x5D;
            wlen += m_wLower(u8espace, 2, nullptr);
        }else{
            pendingLen++;
        }
        dataIndex++;
        u8p++;
    }
    if(pendingLen > 0){
        wlen += m_wLower(u8p - pendingLen, pendingLen, nullptr);
    }
    std::shared_ptr<WriteToLowerObject> wobj = std::dynamic_pointer_cast<WriteToLowerObject>(obj);
    if((wobj == nullptr) || !wobj->hasMoreData()){
        u8data = 0x7E;
        wlen += m_wLower(&u8data, 1, nullptr);
    }
    
    return wlen;
}
int SerialFraming::writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    int wlen = 0;
    
    const uint8_t* u8p = (const uint8_t*)data;
    int dataIndex = 0;

    while(dataIndex < dataLength){
        uint8_t u8data = *u8p;
        if(m_rxState == RX_STATE_IN_FRAME){
            if(u8data == 0x7E){
                if(m_rxBufferLength > 0)
                    wlen += m_wUpper(m_rxBuffer, m_rxBufferLength, nullptr);
                m_rxBufferLength = 0;
                m_isEspace = false;
            }else if(u8data == 0x7D){
                m_isEspace = true;
            }else{
                if(m_isEspace){
                    m_isEspace = false;
                    u8data |= (1 << 5);
                }
                if(m_rxBufferLength < m_frmAttr.maxSizeInByte)
                    m_rxBuffer[m_rxBufferLength++] = u8data;
            }
        }else if(m_rxState == RX_STATE_WAIT_FRAME){
            // find begin flag
            if(u8data == 0x7E){
                m_rxState = RX_STATE_IN_FRAME;
                m_rxBufferLength = 0;
                m_isEspace = false;
            }
        }
        u8p++;
        dataIndex++;
    }
    
    return wlen;
}
