#include "HDLCPacket.h"
#include "../../socket/SocketUtils.h"
#include <cstring>
#include <sstream>
#include "../../../safe/Safe.h"

using namespace communication::protocol::packet;

HDLC::HDLC() : communication::IPacket(){
    m_data = 0;
    m_dataLength = 0;
}
HDLC::~HDLC(){
    if(m_data) delete[] m_data;
    m_data = 0;
}
void* HDLC::rawData(){ return m_data; }
int   HDLC::rawDataLength(){ return m_dataLength; }
void* HDLC::data(){
    if(!m_data) return 0;
    return &m_data[sizeof(Header)];
}
int   HDLC::dataLength(){
    if(!m_data) return 0;
    return m_dataLength - 2 - sizeof(Header);
}
HDLC::Header* HDLC::header(){
    if(!m_data) return 0;
    return static_cast<Header*>((void*)m_data);
}
int   HDLC::importFromRaw(const void* raw, int rawLen){
    int err = -1;    
    if(rawLen >= (int)sizeof(Header) + (int)sizeof(uint16_t)){
        m_data = new uint8_t[rawLen];
        m_dataLength = rawLen;
        cppframework::safe::s_memcpy(m_data, raw, rawLen, rawLen);
        err = 0;
    }
    return err;
}
bool  HDLC::checkValid(){
    bool valid = false;

    if(!m_data) return valid;
    
    uint16_t crc = 0;
    crc = communication::utils::CRC::crc16(0, m_data, m_dataLength - 2);
    
    uint16_t* pcrc = (uint16_t*)&m_data[m_dataLength - 2];
    if(crc == *pcrc) valid = true;
    
    return valid;
}
bool HDLC::isReceiveReady(){
    int ret = false;
    Header* hdr = header();
    if(hdr->control.id_2.id == CONTROL_ID_S_FRAME){
        if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_RR){
            if(hdr->control.s_frame_bits.pf == CONTROL_PF_FINAL){
                ret = true;
            }
        }
    }
    return ret;
}
bool HDLC::isReceiveReadyRequest(){
    int ret = false;
    Header* hdr = header();
    if(hdr->control.id_2.id == CONTROL_ID_S_FRAME){
        if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_RR){
            if(hdr->control.s_frame_bits.pf == CONTROL_PF_POLL){
                ret = true;
            }
        }
    }
    return ret;
}
bool HDLC::isReject(){
    int ret = false;
    Header* hdr = header();
    if(hdr->control.id_2.id == CONTROL_ID_S_FRAME){
        if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_REJ){
            ret = true;
        }
    }
    return ret;
}
bool HDLC::isReset(){
    int ret = false;
    Header* hdr = header();
    if(hdr->control.id_2.id == CONTROL_ID_U_FRAME){
        if((hdr->control.u_frame_bits.function1 == CONTROL_U_RSET_F1) && (hdr->control.u_frame_bits.function2 == CONTROL_U_RSET_F2)){
            ret = true;
        }
    }
    return ret;
}
bool HDLC::isDataMessage(){
    int ret = false;
    Header* hdr = header();
    if(hdr->control.id_1.id == CONTROL_ID_I_FRAME){
        ret = true;
    }
    return ret;
}
bool HDLC::isControlMessage(){
    int ret = false;
    Header* hdr = header();
    if((hdr->control.id_2.id == CONTROL_ID_S_FRAME) || (hdr->control.id_2.id == CONTROL_ID_U_FRAME)){
        ret = true;
    }
    return ret;
}
int HDLC::receiveSequence(){
    Header* hdr = header();
    int seq = 0;
    if(hdr->control.id_1.id == CONTROL_ID_I_FRAME)
        seq = hdr->control.i_frame_bits.recv_seq;
    else if(hdr->control.id_2.id == CONTROL_ID_S_FRAME)
        seq = hdr->control.s_frame_bits.recv_seq;
    return seq;
}
int HDLC::sendSequence(){
    Header* hdr = header();
    int seq = 0;
    if(hdr->control.id_1.id == CONTROL_ID_I_FRAME)
        seq = hdr->control.i_frame_bits.send_seq;
    return seq;
}
int HDLC::getSequenceMaxValue(){
    return 7;
}
int HDLC::build(Header& hdr, const void* data, int dataLength, bool makeCRC){
    m_dataLength = sizeof(Header) + dataLength + sizeof(uint16_t);
    m_data = new uint8_t[m_dataLength];
    cppframework::safe::s_memcpy(m_data, &hdr, sizeof(Header), m_dataLength);
    if(data)
		cppframework::safe::s_memcpy(&m_data[sizeof(Header)], data, dataLength, m_dataLength - sizeof(Header));
    uint16_t crc = 0;
    if(makeCRC)
        crc = communication::utils::CRC::crc16(0, m_data, m_dataLength - 2);
    uint16_t* pcrc = (uint16_t*)&m_data[m_dataLength - 2];
    *pcrc = crc;
    return 0;
}
std::string HDLC::toString(){
    std::stringstream buf;
    Header* hdr = header();
    
    if(hdr->control.id_1.id == CONTROL_ID_I_FRAME){
        buf <<"i-frame: n_s="<<(int)hdr->control.i_frame_bits.send_seq
            <<" n_r="<<(int)hdr->control.i_frame_bits.recv_seq
            <<" p_f="<<(int)hdr->control.i_frame_bits.pf;
        buf <<" dlen="<<dataLength();
    }else if(hdr->control.id_2.id == CONTROL_ID_S_FRAME){
        std::string func = "...";
        if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_RR) func = "RR ";
        else if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_RNR) func = "RNR";
        else if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_REJ) func = "REJ";
        else if(hdr->control.s_frame_bits.function == CONTROL_S_FRAME_SREJ) func = "SREJ";
        buf <<"s-frame: "<<func
            <<" n_r="<<(int)hdr->control.s_frame_bits.recv_seq
            <<" p_f="<<(int)hdr->control.s_frame_bits.id;
    }else if(hdr->control.id_2.id == CONTROL_ID_U_FRAME){
        std::string func = "...";
        if((hdr->control.u_frame_bits.function1 ==  CONTROL_U_RSET_F1) && (hdr->control.u_frame_bits.function2 ==  CONTROL_U_RSET_F2))
            func = "RSET";
        buf <<"u-frame: "<<func
            <<" p_f="<<(int)hdr->control.u_frame_bits.pf;
        buf <<" dlen="<<dataLength();
    }
    return buf.str();
}
std::shared_ptr<HDLC> HDLC::importMessage(const void* data, int dataLength){
    std::shared_ptr<HDLC> msg = std::make_shared<HDLC>();
    msg->importFromRaw(data, dataLength);
    return msg;
}
std::shared_ptr<HDLC> HDLC::buildREJMessage(int seq, bool crcEnable){
    std::shared_ptr<HDLC> msg = std::make_shared<HDLC>();
    Header bhdr;
    bhdr.address.bits.cast   = HEADER_ADDR_UNICAST;
    bhdr.address.bits.extend = HEADER_ADDR_NO_EXTEND;
    bhdr.address.bits.address= 0x01;
    bhdr.control.s_frame_bits.id = CONTROL_ID_S_FRAME;
    bhdr.control.s_frame_bits.pf = CONTROL_PF_FINAL;
    bhdr.control.s_frame_bits.function = CONTROL_S_FRAME_REJ;
    bhdr.control.s_frame_bits.recv_seq = seq;
    msg->build(bhdr, 0, 0, crcEnable);
    return msg;
}
std::shared_ptr<HDLC> HDLC::buildResetMessage(bool crcEnable){
    std::shared_ptr<HDLC> msg = std::make_shared<HDLC>();
    Header hdr;
    hdr.address.bits.cast   = HEADER_ADDR_UNICAST;
    hdr.address.bits.extend = HEADER_ADDR_NO_EXTEND;
    hdr.address.bits.address= 0x01;
    hdr.control.u_frame_bits.id = CONTROL_ID_U_FRAME;
    hdr.control.u_frame_bits.pf = CONTROL_PF_FINAL;
    hdr.control.u_frame_bits.function1 = CONTROL_U_RSET_F1;
    hdr.control.u_frame_bits.function2 = CONTROL_U_RSET_F2;
    msg->build(hdr, 0, 0, crcEnable);
    return msg;
}
std::shared_ptr<HDLC> HDLC::buildReceiveReadyMessage(int seq, bool crcEnable){
    std::shared_ptr<HDLC> msg = std::make_shared<HDLC>();
    Header bhdr;
    bhdr.address.bits.cast   = HEADER_ADDR_UNICAST;
    bhdr.address.bits.extend = HEADER_ADDR_NO_EXTEND;
    bhdr.address.bits.address= 0x01;
    bhdr.control.s_frame_bits.id = CONTROL_ID_S_FRAME;
    bhdr.control.s_frame_bits.pf = CONTROL_PF_FINAL;
    bhdr.control.s_frame_bits.function = CONTROL_S_FRAME_RR;
    bhdr.control.s_frame_bits.recv_seq = seq;
    msg->build(bhdr, 0, 0, crcEnable);
    return msg;
}
std::shared_ptr<HDLC> HDLC::buildReceiveReadyRequestMessage(bool crcEnable){
    std::shared_ptr<HDLC> msg = std::make_shared<HDLC>();
    Header bhdr;
    bhdr.address.bits.cast   = HEADER_ADDR_UNICAST;
    bhdr.address.bits.extend = HEADER_ADDR_NO_EXTEND;
    bhdr.address.bits.address= 0x01;
    bhdr.control.s_frame_bits.id = CONTROL_ID_S_FRAME;
    bhdr.control.s_frame_bits.pf = CONTROL_PF_POLL;
    bhdr.control.s_frame_bits.function = CONTROL_S_FRAME_RR;
    bhdr.control.s_frame_bits.recv_seq = 0;
    msg->build(bhdr, 0, 0, crcEnable);
    return msg;
}
std::shared_ptr<HDLC> HDLC::buildDataMessage(int sSeq, int rSeq, const void* data, int dataLength, bool crcEnable){
    Header hdr;
    hdr.address.bits.cast   = HEADER_ADDR_UNICAST;
    hdr.address.bits.extend = HEADER_ADDR_NO_EXTEND;
    hdr.address.bits.address= 0x01;
    hdr.control.i_frame_bits.id = CONTROL_ID_I_FRAME;
    hdr.control.i_frame_bits.pf = CONTROL_PF_FINAL;
    hdr.control.i_frame_bits.send_seq = sSeq;
    hdr.control.i_frame_bits.recv_seq = rSeq;
        
    std::shared_ptr<HDLC> msg = std::make_shared<HDLC>();
    msg->build(hdr, data, dataLength, crcEnable);
    return msg;
}
