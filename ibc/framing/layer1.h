#ifndef LAYER1_H
#define LAYER1_H
#include <SerialSocket.h>
#include <thread>
#include <chrono>
#ifdef __linux__
#include <sys/prctl.h>
#endif

namespace ibc {
class layer1
{
    int baudRate;
    std::string portName;
    std::shared_ptr<communication::ISocket> sock;

    void thread_function();
    std::thread thread_;
    volatile bool run_;
    communication::Status      status_;

    void set_status(communication::Status status);
    std::function<void(communication::Status status)> notify_;
    std::function<void(const void *data, int len)> data_event_;

public:
    layer1();
    ~layer1();

    int start(const std::string &name, int brate,
              int dbits = communication::endpoint::serialport::WordBits_8,
              int parity = communication::endpoint::serialport::Parity_None,
              int stopbit = communication::endpoint::serialport::StopBits_1);

    int stop();

    size_t sent_raw_data(const void *data, std::size_t len);

    void set_notify_event(const std::function<void(communication::Status status)> &notify) {
        notify_ = notify;
    }

    void set_data_event(const std::function<void(const void *data, int len)> &callback) {
        data_event_ = callback;
    }

};
}
#endif // LAYER1_H
