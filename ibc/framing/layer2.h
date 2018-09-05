#ifndef LAYER2_H
#define LAYER2_H
#include <framing.h>
#include <memory>
#include <SerialSocket.h>
#include <layer1.h>

namespace ibc {
class layer2
{

public:
    layer2();
    ~layer2();

    int start(const std::string &name, int brate,
              int dbits = communication::endpoint::serialport::WordBits_8,
              int parity = communication::endpoint::serialport::Parity_None,
              int stopbit = communication::endpoint::serialport::StopBits_1);

    int stop();
    int send(uint8_t src, uint8_t dst, uint8_t ctrl,
                 uint8_t *data, int len);

    void set_callback_data_recv(
            const std::function<void(uint8_t src, uint8_t dst,
                                     uint8_t seq, uint8_t ctrl,
                                     const void *data_,
                                     int data_len_)> &callback) {
        frame_->set_callback_received(callback);
    }

    void set_callback_send_done(
            const std::function<void(uint8_t src, uint8_t dst,
                                     uint8_t ctrl,const void *data_,
                                     int data_len_)> &callback) {
        frame_->set_callback_sent(callback);
    }


    void set_callback_frame_recv(
            const std::function<void(uint8_t *data_,
                                     int data_len_)> &callback) {
        frame_->set_callback_data_frame(callback);
    }

    void set_callback_cmd_recv(
            const std::function<void(const void *data_,
                                     int data_len_)> &callback) {
        frame_->set_callback_cmd_frame(callback);
    }

    void set_callback_recv_raw(
            const std::function<void(const void *data_,
                                     int data_len_)> &callback) {
        frame_->set_callback_recv_raw(callback);
    }

    size_t send_raw_data(const void *data, int len);
    static int check_bit(const uint8_t* org, uint8_t* data, uint32_t size);

    void set_notify_event(const std::function<void(communication::Status status)> &notify) {
        notify_ = notify;
    }

    void set_ack(bool set) {frame_->set_ack(set);}
    void set_protocol(bool set) {frame_->set_protocol(set);}
    bool get_protocol() const {return frame_->get_protocol();}
    void set_status(communication::Status status);
    communication::Status get_status() const {return status_;}

    bool is_start() const {return start_;}

private:
    size_t get_buf_size() const { return frame_->get_buf_size();}
    size_t get_sent_size() const { return frame_->get_sent_size();}

    void set_nack(bool set) { frame_->set_nack(set);}

private:
    std::shared_ptr<framing> frame_;
    std::shared_ptr<layer1>  serial_;

    std::function<void(communication::Status status)> notify_;
    communication::Status      status_;
    bool                start_;

};

}

#endif // LAYER2_H
