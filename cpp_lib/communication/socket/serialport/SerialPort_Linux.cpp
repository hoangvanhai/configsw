#include "SerialSocket.h"

#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#include <asm/termios.h>
#include <cstring>
#include "../../../dll/dll_function.h"

__BEGIN_DECLS
extern int ioctl (int __fd, unsigned long int __request, ...) __THROW;
extern int tcflush(int fd, int queue_selector);
__END_DECLS

using namespace communication::endpoint::serialport;

SerialSocket::SerialSocket() : ISocket() {
	m_handle = 0;
    m_cfg.baudrate = 115200;
    m_cfg.parity = Parity_None;
    m_cfg.stopBits = StopBits_1;
    m_cfg.useHardwareFlowControl = false;
    m_cfg.wordBits = WordBits_8;
}
SerialSocket::~SerialSocket() {
}
int SerialSocket::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int writeLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return writeLength;
    
    writeLength = write(m_sockID, data, dataSizeInByte);
    
    return writeLength;
}
int SerialSocket::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int readLength = 0;
    
    if(m_sockID == SOCKET_INVALID) return readLength;
    read_dll _read = dll_function<read_dll>("read");
    readLength = _read(m_sockID, data, dataSizeInByte);
    
    return readLength;
}
int SerialSocket::setPortName(struct AttributePortName *port){
    int err = -1;
    open_dll _open = dll_function<open_dll>("open");
    m_sockID = _open(port->portname.c_str(),
    			O_RDWR | O_NDELAY | O_NOCTTY,
				S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH);
    if(m_sockID != SOCKET_INVALID){
        tcflush(m_sockID, TCIOFLUSH);
        m_portName = port->portname;
        err = 0;
    }
    
    return err;
}

int SerialSocket::closeDevice() {

    return 0;
}


int SerialSocket::setConfiguration(struct AttributeConfiguration *cfg){
    int err = -1;
    
    struct termios2 opt;
    if(m_sockID == SOCKET_INVALID) {
        return err;
    }

	ioctl(m_sockID, TCGETS2, &opt);
	memset(&opt, 0, sizeof(opt));
    
    int readTimeout = 1000 / 10;
    opt.c_cc[VMIN]  = 1;
    opt.c_cc[VTIME] = (readTimeout) > 255 ? 255 : readTimeout;
    opt.c_ispeed = cfg->baudrate;
    opt.c_ospeed = cfg->baudrate;
    opt.c_cflag &= ~CBAUD;
    opt.c_cflag |= BOTHER;

    opt.c_cflag &= ~CSIZE;
    switch(cfg->wordBits){
        case WordBits_5:{
            opt.c_cflag |= CS5;
            break;
        }
        case WordBits_6:{
            opt.c_cflag |= CS6;
            break;
        }
        case WordBits_7:{
            opt.c_cflag |= CS7;
            break;
        }
        default:
            opt.c_cflag |= CS8;
            break;
    }
    switch(cfg->stopBits){
        case StopBits_1:{
            opt.c_cflag &= ~CSTOPB;
            break;
        }
        default:
            opt.c_cflag |= CSTOPB;
    }
    switch(cfg->parity){
        case Parity_Even:{
            opt.c_cflag |= PARENB;
            opt.c_cflag &= ~PARODD;
            break;
        }
        case Parity_Odd:{
            opt.c_cflag |= PARENB;
            opt.c_cflag |= PARODD;
            break;
        }
        default:
            opt.c_cflag &= ~PARENB;
            break;
    }    
    opt.c_iflag &= ~INPCK;
    if(cfg->useHardwareFlowControl){
		#if defined CNEW_RTSCTS
		opt.c_cflag |=  CNEW_RTSCTS;
		#endif
        opt.c_cflag |=  CRTSCTS;                        // enable hardware flow control CTS/ RTS
    }else{
		#if defined CNEW_RTSCTS
		opt.c_cflag &=  ~CNEW_RTSCTS;
		#endif
        opt.c_cflag &= ~CRTSCTS;                        // disable hardware flow control CTS/ RTS
    }

    opt.c_cflag |= (CLOCAL | CREAD);                    // ignore modem controls, enable reading
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);     // raw input
    opt.c_oflag &= ~(OPOST|ONLCR|OCRNL);                // raw output
    opt.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK| INLCR| IGNCR| ICRNL); // disable sofware flow 


    ioctl(m_sockID, TCSETS2, &opt);
    
    m_cfg = *cfg;
    err = 0;
    
    return err;
}
int SerialSocket::setControlSignal(struct AttributeControlSignal* sig){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
    
    int status = 0;
    
    err = ioctl(m_sockID, TIOCMGET, &status);
    if(err) return err;
    
    if(sig->rts) status |= TIOCM_RTS;
    else status &= ~TIOCM_RTS;
    
    if(sig->dtr) status |= TIOCM_DTR;
    else status &= ~TIOCM_DTR;
    
    err = ioctl(m_sockID, TIOCMSET, &status);

    return err;
}
int SerialSocket::getControlSignal(struct AttributeControlSignal* sig){
    int err = -1;
    if(m_sockID == SOCKET_INVALID) return err;
    
    int status = 0;
    err = ioctl(m_sockID, TIOCMGET, &status);
    if(!err){    
        if(status & TIOCM_CTS) sig->cts = true;
        else sig->cts = false;
        
        if(status & TIOCM_DSR) sig->dsr = true;
        else sig->dsr = false;
    }

    return err;
}
#endif // __linux__
// end of file
