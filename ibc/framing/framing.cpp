#include "framing.h"
#include <iostream>

namespace ibc {
framing::framing()
{    
    run_ = false;
    ack_ = true;
    protocol_ = true;
    fifo_.Init(2480);
    is_err_ = false;    
    check_crc_ = true;
    check_seq_ = true;
    seq_local_ = 0x00;
    seq_remote_ = 0xff;
    recv_count = 0;

    callback_sent_ = nullptr;
    callback_to_frame_ = nullptr;
    callback_to_stream_ = nullptr;
    callback_cmd_frame_ = nullptr;
    callback_data_frame_ = nullptr;
    callback_recv_raw_ = nullptr;

}

framing::~framing()
{
    stop();
}


int framing::start()
{
    if(1) {
        bg_start();
        bg_set_delay(10);
        bg_set_work([this](const void *param) {
            (void)param;
            auto list_size = frames_sent_.size();
            for(size_t idx = 0; idx < list_size; idx++) {
                lock_frames_sent_.lock();
                std::shared_ptr<uint8_t> mem = pop_head(frames_sent_);
                lock_frames_sent_.unlock();
                if(mem != nullptr) {
                    frame_info* pframe = (frame_info*)mem.get();
                    if (pframe->num_sent > 0) {
                        if (--pframe->timeout == 0)
                        {
                            pframe->num_sent--;
                            pframe->timeout = TRANS_TIMEOUT_TICK;
                            send_trigger(mem);
                            //std::cout << "timeout resend frm " << (int)pframe->seq << std::endl;
                        } else {
                            store_frame(mem);
                        }
                    } else { // After u8NumSend passed
                        std::cout << "NOT GET ACK SEQ " << (int)pframe->seq << std::endl;
                        if(mem) mem.reset();
                    }
                }
            }
        }, 0);

        stop();

        run_ = true;
        thread_ = std::thread([&]() {
            #ifdef __linux__
            prctl(PR_SET_NAME, "framing", 0, 0, 0);
            #endif
            thread_function();
        });
    }
    return 0;
}

int framing::stop()
{
    run_ = false;
    if(thread_.joinable())
        thread_.join();

    fifo_.Reset();
    frames_buf_.clear();
    frames_sent_.clear();
    return 0;
}

int framing::send_(uint8_t src, uint8_t dst, uint8_t ctrl,
                   uint8_t *data, int len, frame_type type)
{
    int ret = 0;
    if(len > FRM_MAX_DLEN) len = FRM_MAX_DLEN;

    lock_frames_buf_.lock();
    auto buff_size = frames_buf_.size();
    lock_frames_buf_.unlock();
    if(buff_size > 1000) {
        return ret;
    }

    auto buf_len = sizeof(frame_info) + FRM_HDR_SIZE + len;
    std::shared_ptr<uint8_t> mem(new uint8_t[buf_len], [](uint8_t *p) {delete[] p;});
    frame_info *pframe = (frame_info*)mem.get();
    pframe->frame = (uint8_t*)pframe + sizeof(frame_info);
    pframe->data = &pframe->frame[IDX_FRM_DATA0];

    pframe->src = src;
    pframe->dst = dst;
    pframe->ctrl = ctrl;
    pframe->dlen = (uint8_t)len;

    if(type == FRM_TYPE_CMD) {
        if(ctrl == CMD_NACK && is_err_) {
            pframe->seq = seq_err_;
        } else {
            pframe->seq = seq_remote_;
        }
    } else {
        pframe->seq = seq_local_++;
    }

    if(len > 0 && data != NULL) {
        for(int i = 0; i < len; i++) {
            pframe->data[i] = data[i];
        }
    }

    pframe->num_sent = MAX_NUM_SEND;
    pframe->timeout = TRANS_TIMEOUT_TICK;

    send_trigger(mem);
    return ret;
}

int framing::send(uint8_t src, uint8_t dst,
                  uint8_t ctrl, uint8_t *data, int len)
{
    return send_(src, dst, ctrl, data, len, FRM_TYPE_DATA);
}

int framing::send_cmd(uint8_t src, uint8_t dst, uint8_t ctrl)
{
    return send_(src, dst, ctrl, NULL, 0, FRM_TYPE_CMD);
}



int framing::to_stream()
{
    int ret = 0;
    if(frames_buf_.size() > 0)
    {
        lock_frames_buf_.lock();
        std::shared_ptr<uint8_t> mem = pop_head(frames_buf_);
        lock_frames_buf_.unlock();

        frame_info *pframe = (frame_info*)mem.get();

        pframe->frame[IDX_FRM_PREAM]    = CST_PREAMBLE;
        pframe->frame[IDX_FRM_SRCADR]   = pframe->src;
        pframe->frame[IDX_FRM_DSTADR]   = pframe->dst;
        pframe->frame[IDX_FRM_SEQNUM]   = pframe->seq;
        pframe->frame[IDX_FRM_CTRL]     = pframe->ctrl;
        pframe->frame[IDX_FRM_DLEN]     = pframe->dlen;
        pframe->frame[IDX_FRM_CRCD]     = crc8(pframe->data, pframe->dlen);
        pframe->frame[IDX_FRM_CRCH]     = crc8(&pframe->frame[IDX_FRM_SRCADR], FRM_HDR_SIZE - 2);


        // for test gen wrong frame
        if(set_nack_ && pframe->dlen > 0) {
            pframe->data[0] = uint8_t(~pframe->data[0]);
            set_nack_ = false;
        }

        if(callback_to_stream_) {
            callback_to_stream_(pframe->frame, FRM_HDR_SIZE + pframe->dlen);

            if(callback_sent_)
                callback_sent_(pframe->src, pframe->dst, pframe->ctrl, pframe->data, pframe->dlen);
        }

        if(ack_ && IS_ACK_REQ(pframe->ctrl)) {
        //if(IS_ACK_REQ(pframe->ctrl)) {
            store_frame(mem);
        }

        ret = FRM_HDR_SIZE + pframe->dlen;
    }

    return ret;
}

void framing::thread_function()
{
    std::unique_lock<std::mutex> lck(cond_var_.mtx);
    while(run_) {
        cond_var_.cv.wait_for(lck, std::chrono::milliseconds(10));
        if(to_stream() > 0) {
            std::cout << ">"; fflush(stdout);
        }
    }
    std::cout << "stop send thread\n";
}

void framing::send_trigger(std::shared_ptr<uint8_t> &mem)
{
    lock_frames_buf_.lock();
    frames_buf_.push_back(mem);
    lock_frames_buf_.unlock();
    cond_var_.cv.notify_all();
}

void framing::store_frame(std::shared_ptr<uint8_t> &mem)
{
    lock_frames_sent_.lock();
    frames_sent_.push_back(mem);
    lock_frames_sent_.unlock();
}


int framing::to_frame(const uint8_t *stream, int stream_len,
                          const std::function<void (action_code act, uint8_t src,
                                                    uint8_t dst)> &action)
{
    return get_frame(stream, stream_len, [this, action](uint8_t *frame, int frame_len) {               
        action_code act_code = ACT_NONE;

        bool is_error = false;
        bool frame_ok = false;

        if(frame[IDX_FRM_DLEN] > FRM_MAX_DLEN &&
                (frame[IDX_FRM_DSTADR] != ADDR_BROADCAST)) {
            act_code = ACT_REQ_NACK;
            seq_err_ =  frame[IDX_FRM_SEQNUM];
            is_error = true;
        }

        if(check_crc_ && frame[IDX_FRM_DLEN] != 0) {
            uint8_t crcd = crc8(&frame[IDX_FRM_DATA0], frame[IDX_FRM_DLEN]);
            if(crcd != frame[IDX_FRM_CRCD]) {
                std::cout << "wrong crcd\n";
                if(IS_ACK_REQ(frame[IDX_FRM_CTRL]) &&
                        (frame[IDX_FRM_DSTADR] != ADDR_BROADCAST)) {
                    //std::cout << "gen nack\n";
                    act_code = ACT_REQ_NACK;
                    is_error = true;
                }
                seq_err_ =  frame[IDX_FRM_SEQNUM];
            }
        }

        if(!is_error) {
            if (IS_DATA_FRM(frame[IDX_FRM_CTRL])) { /* DATA FRAME */
                if (!check_seq_ || frame[IDX_FRM_SEQNUM] != seq_remote_) {
                    frame_ok = true;
                    seq_remote_ = frame[IDX_FRM_SEQNUM];
                    //std::cout << "update seq remote " << (int)seq_remote_ << std::endl;
                    if (IS_ACK_REQ(frame[IDX_FRM_CTRL]) &&
                            (frame[IDX_FRM_DSTADR] != ADDR_BROADCAST)) {
                        act_code = ACT_REQ_ACK;
                    }
                } else {
                    std::cout << "same SEQ " << (int)frame[IDX_FRM_SEQNUM] << std::endl;
                }
            } else {                               /* COMMAND FRAME*/
                switch (GET_FRM_TYPE(frame[IDX_FRM_CTRL]))
                {
                case CMD_NONE:
                    break;
                case CMD_ACK:
                    act_code = ACT_RCV_ACK;
                    seq_cmd_ = frame[IDX_FRM_SEQNUM];
                    break;
                case CMD_NACK:
                    act_code = ACT_RCV_NACK;
                    seq_cmd_ = frame[IDX_FRM_SEQNUM];
                    break;
                default:
                    std::cout << "failed\n";
                    break;
                }

                if(callback_cmd_frame_)
                    callback_cmd_frame_(frame, frame_len);
            }
        }

        if(ack_ && action)
        //if(action)
            action(act_code, frame[IDX_FRM_SRCADR], frame[IDX_FRM_DSTADR]);

        if(frame_ok) {
            // pass entire mem block to higher layer
            if(callback_data_frame_)
                callback_data_frame_(frame, frame_len);

            // pass some information to higher layer
            if(callback_to_frame_) {
                callback_to_frame_(frame[IDX_FRM_SRCADR], frame[IDX_FRM_DSTADR], frame[IDX_FRM_SEQNUM],
                           frame[IDX_FRM_CTRL], &frame[IDX_FRM_DATA0], frame[IDX_FRM_DLEN]);
            }

        }

    });
}

int framing::get_frame(const uint8_t *stream, int stream_len,
                       const std::function<void (uint8_t *frame, int frame_len)> &callback)
{
    int frame_count = 0;
    if(stream_len <= 0) return ACT_ERROR;
    if(fifo_.Push((uint8_t*)stream, stream_len) != (uint32_t)stream_len)
        std::cout << "push fifo error \n";

    while(fifo_.GetCount() > 0) {
        bool is_frame = false;
        while(fifo_.GetCount() > 0) {
            if(recv_count == 0) { // receive header
                if(fifo_.GetCount() < FRM_HDR_SIZE) {
                    is_frame = false;
                    return frame_count;
                }

                uint8_t byte;
                if(!fifo_.Pop(byte))
                    std::cout << "pop failed\n";
                if(byte != CST_PREAMBLE) {
                    //std::cout << "preamble " << (int)fifo_.GetCount() << std::endl;
                    std::cout << "#";
                    continue;
                }
                recv_frm[0] = CST_PREAMBLE;
                recv_count = fifo_.Recv(&recv_frm[1], FRM_HDR_SIZE - 1);

                if(recv_count != FRM_HDR_SIZE - 1) {
                    fifo_.Reset();
                    std::cout << "recv_count error " << (int)recv_count << std::endl;
                    return frame_count;
                }

                // check crch if
                uint8_t crch = crc8(&recv_frm[1], FRM_HDR_SIZE - 2);
                if(crch != recv_frm[IDX_FRM_CRCH]) {
                    recv_count = 0;
                    continue;
                }

                // get dlen
                recv_dlen = recv_frm[IDX_FRM_DLEN];
                recv_count = FRM_HDR_SIZE;

                if(recv_dlen != 0) {
                    continue;
                } else {
                    recv_count = 0;
                    is_frame = true;
                    break;
                }

            } else { // receive data
                if((recv_count - FRM_HDR_SIZE) < recv_dlen) {
                    uint8_t byte;
                    if(!fifo_.Pop(byte))
                        continue;
                    recv_frm[recv_count++] = byte;
                    if((recv_count - FRM_HDR_SIZE) < recv_dlen)
                        continue;
                    is_frame = true;
                    recv_count = 0;
                    break;
                } else {
                    // this code should not reachable in runtime
                    std::cout << "loop " << recv_count; fflush(stdout);
                    recv_count = 0;
                    break;
                }
            }

        }

        if(is_frame) {
            frame_count++;
            if(callback)
                callback(recv_frm, recv_dlen + FRM_HDR_SIZE);
        }        
    }

    return frame_count;
}

int framing::process_receive(const uint8_t *stream, int len) {

    if(protocol_) {
    return to_frame(stream, len, [this](action_code act, uint8_t src, uint8_t dst){
        switch(act) {

        case ACT_NONE:

            break;
        case ACT_RCV_ACK: {            
            //std::cout << "recv ack of seq: " <r< (int)seq_cmd_ << std::endl;
            lock_frames_sent_.lock();
            std::shared_ptr<uint8_t> mem = pop_sent_frame_seq(seq_cmd_);
            lock_frames_sent_.unlock();
            if(mem) {                
                mem.reset();

            }
            //std::cout << "recv ack sent list size: " << (int)frames_sent_.size() << std::endl;
        }
            break;

        case ACT_RCV_NACK: {            
            lock_frames_sent_.lock();
            std::shared_ptr<uint8_t> mem = pop_sent_frame_seq(seq_cmd_);
            lock_frames_sent_.unlock();
            if(mem) {
                frame_info* pframe = reinterpret_cast<frame_info*>(mem.get());
                pframe->timeout = TRANS_TIMEOUT_TICK;
                pframe->num_sent--;
                send_trigger(mem);
                std::cout << "recv nack resend frm " << (int)pframe->seq << std::endl;
            }
        }
            break;

        case ACT_REQ_ACK:
            //std::cout << "req ack -> send ack\n";
            send_cmd(dst, src, CMD_ACK);
            break;

        case ACT_REQ_NACK:
            is_err_ = true;
            std::cout << "req ack -> send nack\n";
            send_cmd(dst, src, CMD_NACK);
            is_err_ = false;
            break;
        default:
            std::cout << "not handle\n";
            break;
        }
    });
    } else {
        if(callback_recv_raw_)
            callback_recv_raw_((uint8_t*)stream, len);
        return 0;
    }
}


std::shared_ptr<uint8_t> framing::pop_sent_frame_seq(uint8_t seq) {
    std::list<std::shared_ptr<uint8_t>>::iterator iter = frames_sent_.begin();        
    while(iter != frames_sent_.end()) {
        std::shared_ptr<uint8_t> frame = *iter;
        frame_info *pframe = reinterpret_cast<frame_info*>(frame.get());
        if(pframe->seq == seq) {
            frames_sent_.erase(iter);
            return frame;
        }
        iter++;
    }
    return nullptr;
}


std::shared_ptr<uint8_t> framing::pop_head(std::list<std::shared_ptr<uint8_t>> &list) {
    if(list.size() > 0) {
        std::list<std::shared_ptr<uint8_t>>::iterator iter = list.begin();
        std::shared_ptr<uint8_t> data = *iter;
        list.erase(iter);
        return data;
    }
    return nullptr;
}


}
