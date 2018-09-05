#include "SerialSocket.h"

#if (defined(_MSC_VER) || defined(__GNUC__))
#include <Windows.h>
#include "../../../string/StringUtils.h"
#include "../../../safe/Safe.h"

using namespace communication;
using namespace communication::endpoint::serialport;

namespace communication {
	namespace endpoint {
		namespace serialport {
			class SerialPort_Internal {
			public:
				SerialPort_Internal() {
					opened = 0;
					handle = INVALID_HANDLE_VALUE;
					event_overlapped = INVALID_HANDLE_VALUE;
					event_write = INVALID_HANDLE_VALUE;
					event_read = INVALID_HANDLE_VALUE;

					baudrate = 115200;
					byte_size = 8;
					parity = 0;
					stopbit = ONESTOPBIT;
				}
				~SerialPort_Internal() {

				}
				int configurate() {
					int ret = -1;
					COMMTIMEOUTS timeout;
					DCB	dcb;
					BOOL ready = TRUE;
					COMMCONFIG cfg = { 0 };
					DWORD cfg_size = sizeof(cfg);

					if (!opened) return -1;

					ready = SetupComm(handle,
						512,
						512);
					if (ready) {
						ready = GetCommTimeouts(handle, &timeout);
						if (ready) {
							timeout.ReadIntervalTimeout = MAXDWORD;
							timeout.ReadTotalTimeoutConstant = 0;
							timeout.ReadTotalTimeoutMultiplier = 0;
							ready = SetCommTimeouts(handle, &timeout);
						}
					}
					if (ready) {
						ready = FALSE;
						cfg.dwSize = cfg_size;
                        if (GetDefaultCommConfig((LPCSTR)name.c_str(), &cfg, &cfg_size)) {
							if (SetCommConfig(handle, &cfg, cfg_size)) {
								ready = TRUE;
							}
						}
					}
					if (ready) {
						ready = SetCommMask(handle, event_mask);
					}
					if (ready) {
						ready = GetCommState(handle, &dcb);
						if (ready) {
							dcb.BaudRate = baudrate;
							dcb.ByteSize = byte_size;
							dcb.Parity = parity;
							dcb.StopBits = stopbit;
							dcb.fBinary = TRUE;		// skip EOF check
							dcb.fParity = FALSE;	// disable parity check
							dcb.fOutX = FALSE;		// disable output X-ON/X-OFF
							dcb.fInX = FALSE;		// disable input X-ON/X-OFF
							dcb.fOutxCtsFlow = FALSE;	// disable CTS
							dcb.fOutxDsrFlow = FALSE;	// disable DSR
							dcb.fDtrControl = DTR_CONTROL_DISABLE;	// disable dtr
							dcb.fRtsControl = RTS_CONTROL_DISABLE;	// disable rts
							ready = SetCommState(handle, &dcb);
						}
					}
					if (ready) ret = 0;
					return ret;
				}// end configurate
				int open_device(std::string devName) {
					int ret = -1;
					DWORD dwaccess = 0;
					size_t convert_chars;
					if (!opened) {
						name = devName;
                        std::string szName = cppframework::str::Util::format("\\\\.\\{}", devName);
						dwaccess = GENERIC_READ | GENERIC_WRITE;
                        // Must undef unicode fucking stupid api !!!
                        handle = CreateFile((LPCSTR)szName.c_str(),
							dwaccess,
							0,
							0,
							OPEN_EXISTING,
							FILE_FLAG_OVERLAPPED,
							0);
                        //std::cout << szName << " " << handle << std::endl;
						if (handle != INVALID_HANDLE_VALUE) {
                            event_mask = EV_ERR | EV_RXCHAR;
							opened = 1;
							if (configurate() == 0) {
								event_overlapped = CreateEvent(0, TRUE, FALSE, 0);
								event_write = CreateEvent(0, TRUE, FALSE, 0);
								event_read = CreateEvent(0, TRUE, FALSE, 0);
								if (!event_overlapped || !event_write) {
									if (event_overlapped) CloseHandle(event_overlapped);
									if (event_write) CloseHandle(event_write);
									if (event_read) CloseHandle(event_read);
									event_overlapped = INVALID_HANDLE_VALUE;
									event_write = INVALID_HANDLE_VALUE;
									event_read = INVALID_HANDLE_VALUE;

									CloseHandle(handle);
									opened = 0;
								}
								else {
									ret = 0;
								}
							}
							else {
								std::cout << "config failed" << std::endl;
								opened = 0;
								CloseHandle(handle);
							}
						}
						else {
							DWORD dwError = GetLastError();
							switch (dwError) {
							case ERROR_PATH_NOT_FOUND:
								std::cout << "path '"<< szName <<"' not found" << std::endl;
								break;
							case ERROR_ACCESS_DENIED:
								std::cout << "access denied" << std::endl;
								break;
							default:
								std::cout << "open failed error "<< dwError << std::endl;
								break;
							}
						}
					}
					return ret;
				}// open device
				int close_device() {
					if (opened) {                        
						CloseHandle(event_overlapped);
						CloseHandle(event_write);
						CloseHandle(event_read);
						CloseHandle(handle);
                        handle = INVALID_HANDLE_VALUE;
                        event_overlapped = INVALID_HANDLE_VALUE;
                        event_write = INVALID_HANDLE_VALUE;
                        event_read = INVALID_HANDLE_VALUE;
						opened = 0;
					}
					return 0;
				}
				int read_device(void* data, int dataLength, int timeout) {
					int ret = 0;
					DWORD read_len = 0;
					DWORD event_r;
					DWORD err;
					OVERLAPPED os_reader = { 0 };
					BOOL wait_read = FALSE;
					//std::cout << "read" << std::endl;
					if (!opened) {
						std::cout << "handle is null" << std::endl;
						return ret;
					}

					memset(&os_reader, 0, sizeof(os_reader));
					os_reader.hEvent = event_overlapped;

					if (!WaitCommEvent(handle, &event_r, &os_reader)) {
						err = GetLastError();
						if (err == ERROR_IO_PENDING) {
							switch (WaitForSingleObject(event_overlapped, timeout)) {
							case WAIT_OBJECT_0: {
								event_r &= event_mask;
								if (event_r & EV_RXCHAR) {
									// rx event
									os_reader.hEvent = event_read;
									if (ReadFile(handle, data, dataLength, &read_len, &os_reader)) {
										ret = read_len;
                                        //std::cout << "read1 "<< read_len << std::endl;
                                    }
                                    else {
										err = GetLastError();
										if (err == ERROR_IO_PENDING) {
											err = WaitForSingleObject(os_reader.hEvent, timeout);
											switch (err) {
											case WAIT_OBJECT_0: {
												if (GetOverlappedResult(handle, &os_reader, &read_len, FALSE)) {
													ret = read_len;
                                                    //std::cout << "read2 " << read_len << std::endl;
												}
												else {
                                                    //std::cout << "error5 " << std::endl;
												}
												break;
											}
											case WAIT_TIMEOUT: {
												CancelIo(handle);
                                                //std::cout << "timeout 1" << std::endl;
												break;
											}
											default: 
                                                //std::cout << "error4 " << std::endl;
												break;
											}
										}
										else if (err == ERROR_INVALID_HANDLE) {
                                            //std::cout << name <<" invalid handle "<<handle << std::endl;
										}
										else {
                                            //std::cout << "GetLastError()=" << err << std::endl;
										}
									}
                                }
                                else {
                                    //std::cout << "event_read & EV_RXCHAR = " << event_read << std::endl;
								}
								break;
							}
							case WAIT_TIMEOUT: {
								CancelIo(handle);
                                //std::cout << name<<" timeout 2" << std::endl;
								break;
							}
							default: {
								err = GetLastError();
                                //std::cout << "error1 " << err << std::endl;
							}
							}
                        } else {
                            std::cout << "error2 " << err << std::endl;
                            ret = -1;
						}
                    }
                    else {
						err = GetLastError();
                        //std::cout << "error3 " << err << std::endl;
					}
					return ret;
				}// read
				int write_device(const void* data, int dataLength) {
					int ret = 0;
					DWORD written_len = 0;
					DWORD res;
					OVERLAPPED os_write = { 0 };

					if (!opened) return ret;

					os_write.hEvent = event_write;
					if (WriteFile(handle, data, dataLength, &written_len, &os_write)) {
						ret = written_len;
					}
					else {
						if (GetLastError() == ERROR_IO_PENDING) {
							res = WaitForSingleObject(os_write.hEvent, 100);
							switch (res) {
							case WAIT_OBJECT_0: {
								if (GetOverlappedResult(handle, &os_write, &written_len, FALSE)) {
									ret = written_len;
								}
								break;
							}
							case WAIT_TIMEOUT: {
								CancelIo(handle);
								break;
							}
							default: break;
							}
                        } else {
                            std::cout << "error " << GetLastError() << std::endl;
                        }
					}
					return ret;
				}// write
				int set_signal(bool rts, bool dtr) {
					int err = -1;
					if (!opened) return err;

					DWORD lineStats = 0;
					if (rts) lineStats = SETRTS;
					else lineStats = CLRRTS;
					if (EscapeCommFunction(handle, lineStats)) err = 0;

					if (dtr) lineStats = SETDTR;
					else lineStats = CLRDTR;
					if (EscapeCommFunction(handle, lineStats)) err = 0;
					return err;
				}
				int get_signal(bool* cts, bool* dsr) {
					int err = -1;
					if (!opened) return err;

					DWORD lineStats;
					if (GetCommModemStatus(handle, &lineStats)) {
						*cts = (lineStats & MS_CTS_ON) ? true : false;
						*dsr = (lineStats & MS_DSR_ON) ? true : false;
						err = 0;
					}
					return err;
				}


				HANDLE	handle;
				HANDLE	event_overlapped;
				HANDLE	event_write;
				HANDLE	event_read;
				int		opened;
				DWORD	event_mask;
				DWORD	baudrate;
				BYTE	byte_size;
				BYTE	parity;
				BYTE	stopbit;
				std::string name;
			};
		}
	}
}
SerialSocket::SerialSocket() : ISocket() {
	m_handle = new SerialPort_Internal();
    m_cfg.baudrate = 0;
    m_cfg.parity = Parity_None;
    m_cfg.stopBits = StopBits_1;
    m_cfg.useHardwareFlowControl = false;
    m_cfg.wordBits = WordBits_8;
}
SerialSocket::~SerialSocket() {
	delete static_cast<SerialPort_Internal*>(m_handle);
	m_handle = 0;
}
int SerialSocket::waitEvent(int eventMustToWait, int timeoutInMilisecond) {
	if(m_handle)
		return eventMustToWait;
	return 0;
}
int SerialSocket::writeData(const void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int writeLength = 0;

	if (m_handle) {
		writeLength = (static_cast<SerialPort_Internal*>(m_handle))->write_device(data, dataSizeInByte);
	}

    return writeLength;
}
int SerialSocket::readData(void* data, int dataSizeInByte, std::shared_ptr<Object> obj){
    int readLength = 0;

	if (m_handle) {
		readLength = (static_cast<SerialPort_Internal*>(m_handle))->read_device(data, dataSizeInByte, 100);
	}

    return readLength;
}
int SerialSocket::setPortName(struct AttributePortName *port){
    int err = -1;
	m_portName = port->portname;
	if (m_handle) {
		err = (static_cast<SerialPort_Internal*>(m_handle))->open_device(m_portName);
	}
    return err;
}

int SerialSocket::closeDevice(){
    int err = -1;
    if (m_handle) {
        err = (static_cast<SerialPort_Internal*>(m_handle))->close_device();
    }
    return err;
}

int SerialSocket::setConfiguration(struct AttributeConfiguration *cfg){
    int err = -1;
	m_cfg = *cfg;
	if (m_handle) {
		SerialPort_Internal* p = static_cast<SerialPort_Internal*>(m_handle);
		p->baudrate = cfg->baudrate;
		switch (cfg->stopBits){
		case StopBits_1_5:
			p->stopbit = ONE5STOPBITS;
			break;
		case StopBits_2:
			p->stopbit = ONE5STOPBITS;
			break;
		default:
			p->stopbit = TWOSTOPBITS;
			break;
		}
		switch (cfg->wordBits)
		{
		case WordBits_5:
			p->byte_size = 5;
			break;
		case WordBits_6:
			p->byte_size = 6;
			break;
		case WordBits_7:
			p->byte_size = 7;
			break;
		default:
			p->byte_size = 8;
			break;
		}
		p->parity = cfg->parity;
	}
	err = 0;
    return err;
}
int SerialSocket::setControlSignal(struct AttributeControlSignal* sig){
    int err = -1;
	if (m_handle) {
		SerialPort_Internal* p = static_cast<SerialPort_Internal*>(m_handle);
		err = p->set_signal(sig->rts, sig->dtr);
	}
    return err;
}
int SerialSocket::getControlSignal(struct AttributeControlSignal* sig){
    int err = -1;
	if (m_handle) {
		SerialPort_Internal* p = static_cast<SerialPort_Internal*>(m_handle);
		err = p->get_signal(&sig->cts, &sig->dsr);
	}
	return err;
}
#endif // _MSC_VER
// end of file
