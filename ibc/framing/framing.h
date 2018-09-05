#ifndef IBC_H
#define IBC_H

#include <stdint.h>
#include <functional>
#include <memory>
#include <cstring>
#include <fifo.h>
#include <crc8.h>
#include <list>
#include <mutex>
#include <background.h>
#include <condition_variable>


namespace ibc {

#define IS_ACK_REQ(ctrl)            ((ctrl) & 0x80 ? true : false)
#define IS_CRCD_REQ(ctrl)           ((ctrl) & 0x40 ? true : false)
#define GET_FRM_TYPE(ctrl)          ((ctrl) & 0x1F)
#define CLEAR_ACK_REQ_BIT(ctrl)     ((ctrl) & 0x7F)
#define GET_CTRL(ctrl)              ((ctrl) & 0x3F)

#define SET_CRCD_REQ_BIT(ctrl)      ((ctrl) |= 0x40)
#define CLR_CRCD_REQ_BIT(ctrl)      ((ctrl) &= 0xBF)

#define MASK_DATA                   0x20
#define CMD_NONE                    0x00
#define	CMD_ACK                     0x01
#define CMD_NACK                    0x02
#define CMD_SLV_SEND_EN             0x03	//only for IBC
#define CMD_NOTDONE                 0x04
#define CMD_ERROR                   0x05

#define FRM_DATA                    0x20
#define FRM_CMD                     0x00
#define ACK_REQ                     0x80

#define IS_DATA_FRM(fk)				(((fk) & MASK_DATA) ? true : false)
#define IS_CMD_FRM(fk)				(((fk) & MASK_DATA) ? false : true)

#define ADDR_BROADCAST				0xFF

#define MAX_NUM_SEND				(3)

#define TRANS_TIMEOUT_TICK			(10)	//in mili-ten second


#define CST_PREAMBLE                0x21
#define FRM_HDR_SIZE                8
#define FRM_MAX_DLEN                240
#define FRM_MAX_SIZE                (FRM_MAX_DLEN + FRM_HDR_SIZE)

#define IDX_FRM_PREAM				0
#define IDX_FRM_SRCADR				1
#define IDX_FRM_DSTADR				2
#define IDX_FRM_SEQNUM				3
#define IDX_FRM_CTRL				4
#define IDX_FRM_DLEN				5
#define IDX_FRM_CRCD				6
#define IDX_FRM_CRCH				7
#define IDX_FRM_DATA0				8


struct frame_info {
    uint16_t timeout;
    uint8_t num_sent;
    uint8_t src;
    uint8_t dst;
    uint8_t ctrl;
    uint8_t dlen;
    uint8_t seq;
    uint8_t *data;
    uint8_t *frame;
};

enum frame_type {
    FRM_TYPE_CMD = 0,      //(CMD_ACK or CMD_NACK)
    FRM_TYPE_DATA = 1      //(DATA_FRM)
};

enum action_code {
    ACT_ERROR = -1,
    ACT_NONE = 0,
    ACT_RCV_ACK = 1,    //receive ACK frame from master
    ACT_RCV_NACK = 2,   //receive ACK frame from master
    ACT_REQ_ACK = 3,    //recieve data frame requireing ACK
    ACT_REQ_NACK = 4    //receive a wrong frame
};

struct cond_variable{
    std::mutex mtx;
    std::condition_variable cv;
};

class framing : public background
{
public:
    framing();
    ~framing();

    int start();

    int stop();

    int send_(uint8_t src, uint8_t dst, uint8_t ctrl,
             uint8_t *data, int len, frame_type type);

    int send(uint8_t src, uint8_t dst, uint8_t ctrl,
             uint8_t *data, int len);

    int send_cmd(uint8_t src, uint8_t dst, uint8_t ctrl);

    int to_stream();

    void thread_function();

    void send_trigger(std::shared_ptr<uint8_t> &mem);

    void store_frame(std::shared_ptr<uint8_t> &mem);

    int to_frame(const uint8_t *stream, int stream_len,
                     const std::function<void (action_code act, uint8_t src, uint8_t dst)> &action);

    int get_frame(const uint8_t *stream, int stream_len,
                  const std::function<void(uint8_t*frame, int frame_len)> &callback);

    int process_receive(const uint8_t *stream, int len);

    void set_callback_to_send(const std::function<void(uint8_t *data, int data_len)> &callback) {
        callback_to_stream_ = callback;
    }

    void set_callback_received(const std::function<void(uint8_t src, uint8_t dst, uint8_t seq, uint8_t ctrl,
                                   const void *data_, int data_len_)> &callback) {
        callback_to_frame_ = callback;
    }

    void set_callback_data_frame(const std::function<void(uint8_t *data_, int data_len_)> &callback) {
        callback_data_frame_ = callback;
    }

    void set_callback_cmd_frame(const std::function<void(uint8_t *data_, int data_len_)> &callback) {
        callback_cmd_frame_ = callback;
    }

    void set_callback_sent(const std::function<void(uint8_t src, uint8_t dst, uint8_t ctrl,
                                   uint8_t *data_, int data_len_)> &callback) {
        callback_sent_ = callback;
    }

    void set_callback_recv_raw(const std::function<void(uint8_t *data, int data_len)> &callback) {
        callback_recv_raw_ = callback;
    }

    std::shared_ptr<uint8_t> pop_sent_frame_seq(uint8_t seq);
    std::shared_ptr<uint8_t> pop_head(std::list<std::shared_ptr<uint8_t> > &list);

    void set_nack(bool set) {set_nack_ = set;}

    void set_ack(bool set) {ack_ = set;}
    void set_protocol(bool set) {protocol_ = set;}
    bool get_protocol() const {return protocol_;}

    size_t get_buf_size() const {return frames_buf_.size();}
    size_t get_sent_size() const {return frames_sent_.size();}

private:
    uint8_t seq_local_;
    uint8_t seq_remote_;
    uint8_t seq_err_;    
    uint8_t seq_cmd_;    
    volatile bool is_err_;
    volatile bool check_crc_;
    volatile bool check_seq_;
    FIFO    fifo_;

    int recv_count;
    uint8_t recv_dlen;
    uint8_t recv_frm[2480];

    std::thread thread_;
    volatile bool run_;
    cond_variable cond_var_;

    std::mutex          lock_frames_sent_;
    std::list<std::shared_ptr<uint8_t>> frames_sent_;

    std::mutex          lock_frames_buf_;
    std::list<std::shared_ptr<uint8_t>> frames_buf_;

    std::function<void(uint8_t src, uint8_t dst, uint8_t seq, uint8_t ctrl,
                             uint8_t *data_, int data_len_)> callback_to_frame_;

    std::function<void(uint8_t *data_, int data_len_)> callback_data_frame_;

    std::function<void(uint8_t *cmd_, int cmd_len_)> callback_cmd_frame_;

    std::function<void(uint8_t src, uint8_t dst, uint8_t ctrl,
                             uint8_t *data_, int data_len_)> callback_sent_;

    std::function<void(uint8_t *data, int data_len)> callback_to_stream_;

    std::function<void(uint8_t *data, int data_len)> callback_recv_raw_;

    bool set_nack_;
    volatile bool ack_;
    volatile bool protocol_;
};
}

#endif // IBC_H
