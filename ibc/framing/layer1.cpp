#include "layer1.h"
#include <cstring>
//using namespace std::literals::chrono_literals;
using namespace communication::endpoint::serialport;

namespace ibc {

layer1::layer1()
{
    sock = std::make_shared<SerialSocket>();
    notify_ = nullptr;
    data_event_ = nullptr;
    set_status(communication::Status_Disconnected);
}

layer1::~layer1()
{
    stop();
}

void layer1::set_status(communication::Status status)
{
    status_ = status;
    if(notify_)
        notify_(status);
}

void layer1::thread_function()
{
    uint8_t rxBuff[4096];
    int err_cnt = 0;
    int quit_code = 0;
    while(run_){
        int event = sock->waitEvent(communication::Event_Readable, 100);

        if (event & communication::Event_Error) {
            std::cout << "Wait error\r\n";
            if(err_cnt++ > 10) {
                quit_code = -1;
                break;
            }
        }

        if (event & communication::Event_Readable) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            memset(rxBuff, 0, 4096);
            int rlen = sock->readData(rxBuff, 4096);            
            if (rlen > 0) {
                err_cnt = 0;
                if(data_event_) {
                    data_event_(rxBuff, rlen);
                }
            } else if(rlen == 0){
                #ifdef __linux__
                if(err_cnt++ > 10) {
                    quit_code = -2;
                    break;
                }
                #endif
            } else { //rlen < 0
                if(err_cnt++ > 10) {
                    quit_code = -2;
                    break;
                }
            }
        }
    }
    if(quit_code == -2) {
        std::cout << "device unplugged !!!\n"; fflush(stdout);
        set_status(communication::Status_Device_Disconnected);
    }
    std::cout << "listen thread stop code: " << quit_code << std::endl;
}



int layer1::start(const std::string &name, int brate, int dbits, int parity, int stopbit)
{
    portName = name;
    baudRate = brate;
    int err = 0;
    AttributePortName portNameArgs;
#if defined (__linux__)
    portNameArgs.portname = "/dev/" + portName;
#else
    portNameArgs.portname = portName;
    err = sock->setAttribute(Attribute_CloseDevice, &portNameArgs);
#endif


    err = sock->setAttribute(Attribute_PortName, &portNameArgs);
    if(err != 0) {
        set_status(communication::Status_Disconnected);
        std::cout << "Port open " << portNameArgs.portname << " FAILED!\n";
        return -1;
    }
    // setup config
    communication::endpoint::serialport::AttributeConfiguration cfgArgs;
    cfgArgs.baudrate    = baudRate;
    cfgArgs.wordBits    = WordBits(dbits);
    cfgArgs.stopBits    = StopBits(stopbit);
    cfgArgs.parity      = Parity(parity);
    cfgArgs.useHardwareFlowControl = false;
    err = sock->setAttribute(Attribute_Configuration, &cfgArgs);
    if(err != 0) {
        set_status(communication::Status_Disconnected);
        std::cout << "Port setup " << portNameArgs.portname << " FAILED!\n";
        return -2;
    }
    std::cout << "Port open " << portNameArgs.portname << " SUCCESS!\n";

    run_ = true;
    thread_ = std::thread([&](){
        #ifdef __linux__
        prctl(PR_SET_NAME, "listener", 0, 0, 0);
        #endif
        thread_function();
    });

    set_status(communication::Status_Connected);

    return 0;
}

int layer1::stop()
{
    int err;
    run_ = false;
    if(thread_.joinable())
        thread_.join();

    set_status(communication::Status_Disconnected);

    AttributePortName portNameArgs;
    portNameArgs.portname = portName;
    err = sock->setAttribute(Attribute_CloseDevice, &portNameArgs);
    (void)err;
    return 0;
}

std::size_t layer1::sent_raw_data(const void *data, std::size_t len)
{
    if(data == NULL || len <= 0) return -1;
    int event = sock->waitEvent(communication::Event_Writable, 100);

    if(event & communication::Event_Error){
        std::cout << "select failed\n";
        return -2;
    }
    if((event & communication::Event_Writable) == 0){
        std::cout << "event not writable\n";
        return -3;
    }

//    uint8_t *sendData = (uint8_t*)data;

//    for(int i = 0; i < len; i++) {
//        printf("%c", sendData[i]);
//    }

//    printf("\r\n");

//    fflush(stdout);

    return sock->writeData(data, len);
}
}
