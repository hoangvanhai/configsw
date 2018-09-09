#include "layer2.h"


namespace ibc {

layer2::layer2()
{    
    frame_ = std::make_shared<framing>();    
    serial_ = std::make_shared<layer1>();
    notify_ = nullptr;
    start_ = false;

}

layer2::~layer2()
{
    stop();
}

/**
 * @brief layer2::start
 * @param name
 * @param brate
 * @return
 */
int layer2::start(const std::string &name, int brate, int dbits, int parity, int stopbit)
{
    int ret = 0;

    if(communication::Status_Connected == status_) {
        stop();
    }

    start_ = true;

    frame_->start();

    serial_->set_notify_event([this](communication::Status status) {
        set_status(status);
    });

    ret = serial_->start(name, brate, dbits, parity, stopbit);
    if(ret != 0) {
        std::cout << "start layer 1 failed\n";
        return ret;
    }

    start_ = false;

    frame_->set_callback_to_send([&](uint8_t *data, int data_len) {
        serial_->sent_raw_data(data, data_len);
    });

    serial_->set_data_event([this](const void *data, int len) {
        frame_->process_receive((uint8_t*)data, len);
    });


    std::cout << "layer 2 started ok\n"; fflush(stdout);

    return 0;
}


/**
 * @brief layer2::stop
 * @return
 */
int layer2::stop()
{
    frame_->stop();
    serial_->stop();    
    return 0;
}

/**
 * @brief layer2::send
 * @param src
 * @param dst
 * @param ctrl
 * @param data
 * @param len
 * @return
 */
int layer2::send(uint8_t src, uint8_t dst, uint8_t ctrl, uint8_t *data, int len)
{
    if(status_ == communication::Status_Connected)
        return frame_->send(src, dst, ctrl, data, len);
    else
        return 0;
}


/**
 * @brief layer2::send_raw_data
 * @param data
 * @param len
 * @return
 */
std::size_t layer2::send_raw_data(const void *data, int len)
{
    if(status_ == communication::Status_Connected)
        return serial_->sent_raw_data(data, len);
    else
        return 0;
}

int layer2::check_bit(const uint8_t *org, uint8_t *data, uint32_t size)
{
    uint32_t    idx1, idx2;
    uint8_t     u8Val;
    uint8_t     u8BitMask;
    uint32_t    err_bit = 0;

    for (idx1 = 0; idx1 < size; idx1++) {
        if (data[idx1] != org[idx1]) {
            u8BitMask = 0x01;
            u8Val = data[idx1] ^ org[idx1];
            for (idx2 = 0; idx2 < 8; idx2++) {
                if ((u8Val & u8BitMask)) {
                    err_bit++;
                }
                u8BitMask = uint8_t(u8BitMask << 1);
            }
        }
    }

    return err_bit;
}

void layer2::set_status(communication::Status status)
{
    status_ = status;
    if(notify_)
        notify_(status);
}

}
