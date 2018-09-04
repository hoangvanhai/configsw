#include "SerialSocket.h"

using namespace communication::endpoint::serialport;

int SerialSocket::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    (void)attrSize;
    switch(attrId){
        case Attribute_ControlSignal:{
            err = setControlSignal((struct AttributeControlSignal*)attr);
            break;
        }
        case Attribute_PortName:{
            err = setPortName((struct AttributePortName*)attr);
            break;
        }
        case Attribute_Configuration:{
            err = setConfiguration((struct AttributeConfiguration*)attr);

            break;
        }
        case Attribute_CloseDevice:
            err = closeDevice();
        default:
            break;
    }
    
    //std::cout << "err = " << err << std::endl;
    return err;
}
int SerialSocket::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    (void)attrSize;
    switch(attrId){
        case Attribute_ControlSignal:{
            err = getControlSignal((struct AttributeControlSignal*)attr);
            break;
        }
        case Attribute_PortName:{
            err = getPortName((struct AttributePortName*)attr);
            break;
        }
        case Attribute_Configuration:{
            err = getConfiguration((struct AttributeConfiguration*)attr);
            break;
        }
        default:
            break;
    }
    
    return err;
}
int SerialSocket::getPortName(struct AttributePortName *port){
    port->portname = m_portName;
    return 0;
}
int SerialSocket::getConfiguration(struct AttributeConfiguration *cfg){
    
    *cfg = m_cfg;
    
    return 0;
}
// end of file
