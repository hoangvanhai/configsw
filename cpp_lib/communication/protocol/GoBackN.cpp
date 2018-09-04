#include "GoBackN.h"

#include <cstring>
#include <cstdarg>
#include <iomanip>

using namespace communication;
using namespace communication::protocol;

void GoBackN::logicThreadFunction(){
    bool needToWait = false;
    while(!m_term){
        if(needToWait)
            m_logicNotify.wait_for(100);
        needToWait = true;
        // Internal-command
        std::shared_ptr<Command> cmd = nullptr;
        std::shared_ptr<IPacket> msg = nullptr;
        do{
            // Pop all message in queue
            cmd = nullptr;
            {
                std::lock_guard<std::recursive_mutex> lock(m_lstCommandLock);
                if(m_lstCommand.size() > 0){
                    cmd = m_lstCommand.front();
                    m_lstCommand.pop_front();
                }
            }
            if(cmd){
                if(cmd->id() == Command_REJ){
                    // Send a REJ command
                    std::shared_ptr<REJ_Command> rej = std::dynamic_pointer_cast<REJ_Command>(cmd);
                    std::shared_ptr<IPacket> msg = buildREJMessage(rej->seq());
                    pushMessageToLowerLayer(msg);
                }
                else if(cmd->id() == Command_Reset){
                    // Reset internal-state
                    // Send a RSET command
                    resetCurrentState();
                    // send a RSET command
                    std::shared_ptr<IPacket> msg = buildResetMessage();
                    pushMessageToLowerLayer(msg);
                }
            }
        }while(cmd != nullptr);
        
        // Control-packet (S-Frame & U-Frame)
        do{
            msg = nullptr;
            // Pop all message from queue
            {
                std::lock_guard<std::recursive_mutex> lock(m_rxControlPacketLock);
                if(m_rxControlPacket.size() > 0){
                    msg = m_rxControlPacket.front();
                    m_rxControlPacket.pop_front();
                }
            }
            if(msg){
                if(msg->isReceiveReady()){
                    // RR-Frame as ACK
                    // remove all frame[id < seq] in tx window list
                    int seq = msg->receiveSequence();
                    removeMessageInTxWindow(seq);
                }else if(msg->isReceiveReadyRequest()){
                    // RR-Frame as ACK Request
                    // send a RR(F) with current nextSeq
                    msg= buildReceiveReadyMessage(m_rxNextSeq);
                    pushMessageToLowerLayer(msg);
                }else if(msg->isReject()){
                    // REJ-Frame
                    // remove all frame[id < seq] in tx window list
                    // resent frame in tx window
                    int seq = msg->receiveSequence();
                    removeMessageInTxWindow(seq);
                    for(auto msg : m_txWindowPacket){
                        pushMessageToLowerLayer(msg);
                    }
                }else if(msg->isReset()){
                    // RSET-Frame
                    // reset internal-state
                    resetCurrentState();
                }
            }
        }while(msg != nullptr);
        // Send RR-Frame as ACK
        msg = nullptr;
        {
            std::lock_guard<std::recursive_mutex> lock(m_rxWindowPacketLock);
            if((int)m_rxWindowPacket.size() > m_windowSize * 2 / 3){
                std::shared_ptr<IPacket> lastestMsg = m_rxWindowPacket.back();
                
                int seq = lastestMsg->sendSequence(); seq++;
                if(seq >= m_seqMax) seq = 0;
                msg= buildReceiveReadyMessage(seq);
                m_rxWindowPacket.clear();
            }
        }
        if(msg != nullptr)
            pushMessageToLowerLayer(msg);
        // Check to send data in queue
        if((int)m_txWindowPacket.size() >= m_windowSize){
            // txwindow full: check timeout and send a RR(p) command
            auto tp = std::chrono::high_resolution_clock::now();
			auto tpDiff = std::chrono::duration_cast<std::chrono::milliseconds>(tp - m_txIFrameTimePoint);
            int msDiff = tpDiff.count();
            m_txIFrameTimePoint = tp;
            if(msDiff >= m_txTimeout){
                msg = buildReceiveReadyRequestMessage();
                pushMessageToLowerLayer(msg);
            }
        }else{
            // Send data as I-Frame
            bool hasTx = false;
            do{
                // Pop message from queue
                msg = nullptr;
                {
                    std::lock_guard<std::recursive_mutex> lock(m_txMsgQueueLock);
                    if(m_txMsgQueue.size() > 0){
                        msg = m_txMsgQueue.front();
                        m_txMsgQueue.pop_front();
                    }
                }
                m_eventNotify.notify_all();
                if(msg){
                    // push packet to pending ACK list
                    // send packet as I-Frame
                    m_txWindowPacket.push_back(msg);
                    pushMessageToLowerLayer(msg);
                    hasTx = true;
                }
                if((int)m_txWindowPacket.size() >= m_windowSize)
                    break;
            }while((msg != nullptr) && !m_term);
            if(hasTx){
                m_txIFrameTimePoint = std::chrono::high_resolution_clock::now();
            }
        }
        // check has control-frame in queue
        {
            std::lock_guard<std::recursive_mutex> lock(m_rxControlPacketLock);
            if(m_rxControlPacket.size() > 0) needToWait = false;
        }   
    }
    
}
bool GoBackN::crcEnable(){
    return m_crcEnable;
}
int GoBackN::writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    int rlen = 0;
    
    std::shared_ptr<IPacket> msg = buildRawMessage(data, dataLength);
    if(!m_crcEnable || msg->checkValid()){
// //         printMesprintsage("rx:", msg);
        if(msg->isDataMessage()){
            // Received data packet
            int msgSeq = msg->sendSequence();
            if(m_rxState == RX_STATE_NORMAL){
                if(msgSeq == m_rxNextSeq){
                    // Correct RX-SEQ, push message to upper
                    push_I_MessageFromLower(msg);
                }else{
                    // Incorrect RX-SEQ
                    // push a REJ command
                    m_rxState = RX_STATE_REJ;
                    {
                        std::lock_guard<std::recursive_mutex> lock(m_lstCommandLock);
                        std::shared_ptr<Command> cmd = std::make_shared<REJ_Command>(m_rxNextSeq);
                        m_lstCommand.push_back(cmd);
                        m_logicNotify.notify_all();
                    }
                }
            }else{
                if(msgSeq == m_rxNextSeq){
                    // Continue normal state
                    m_rxState = RX_STATE_NORMAL;
                    push_I_MessageFromLower(msg);
                }else{
                    // Ignore data message in REJ RX state
//                     printDebug("REJ mode, ignore seq %d\r\n", msgSeq);
                }
            }
        }else if(msg->isControlMessage()){
            // Push all control packet to queue
            std::lock_guard<std::recursive_mutex> lock(m_rxControlPacketLock);
            m_rxControlPacket.push_back(msg);
            m_logicNotify.notify_all();
        }
        else{
//             printDebug("rx msg with id %02X\r\n", hdr->control.id_2.id);
        }
    }else{
//         printDebug("rx msg with incorrect crc, destroy msg\r\n");
    }
    
    return rlen;
}

void GoBackN::resetCurrentState(){
    {
        std::lock_guard<std::recursive_mutex> lock(m_txMsgQueueLock);
        m_txMsgQueue.clear();
        m_txWindowPacket.clear();
    }
    {
        std::lock_guard<std::recursive_mutex> lock(m_rxWindowPacketLock);
        m_rxWindowPacket.clear();
    }
    {
        std::lock_guard<std::recursive_mutex> lock(m_rxControlPacketLock);
        m_rxControlPacket.clear();
    }
    {
        std::lock_guard<std::recursive_mutex> lock(m_lstCommandLock);
        m_lstCommand.clear();
    }
    m_rxState = RX_STATE_NORMAL;
    m_txSeq   = 0;
    m_rxNextSeq = 0;
}

int  GoBackN::removeMessageInTxWindow(int seq){
    // From oldest to newest packet in window
    // Search matched seq
    // Remote all older packets than found_packet(seq)
    std::list<std::shared_ptr<IPacket>> tmpList;
    std::list<std::shared_ptr<IPacket>>::iterator iter = m_txWindowPacket.begin();
    while(iter != m_txWindowPacket.end()){
        if((*iter)->sendSequence() == seq){
            while(iter != m_txWindowPacket.end()){
                tmpList.push_back(*iter);
                ++iter;
            }
            break;
        }
        ++iter;
    }
    m_txWindowPacket.clear();
    for(auto msg : tmpList){
        m_txWindowPacket.push_back(msg);
    }
    return 0;
}

int GoBackN::writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj){
    int wlen = 0;
    
    // Push message to TX queue and notify logic thread
    {
        std::lock_guard<std::recursive_mutex> lock(m_txMsgQueueLock);
        if((int)m_txMsgQueue.size() < m_txMsgQueueMaxSize){
            std::shared_ptr<IPacket> msg = buildDataMessage(makeTxSeq(), 0, data, dataLength);
            m_txMsgQueue.push_back(msg);
            wlen = dataLength;
        }else{
//             printDebug("tx msg buffer is full\r\n");
        }
    }
    m_logicNotify.notify_all();
    
    return wlen;
}

int GoBackN::push_I_MessageFromLower(std::shared_ptr<IPacket>& msg){
    // write to message queue
    pushMessageToUpperLayer(msg);
    // push msg to rx window queue
    {
        std::lock_guard<std::recursive_mutex> lock(m_rxWindowPacketLock);
        m_rxWindowPacket.push_back(msg);
        m_logicNotify.notify_all();
    }
    m_rxNextSeq++;
    if(m_rxNextSeq >= m_seqMax) m_rxNextSeq = 0;
    return 0;
}

int GoBackN::waitEvent(int eventMustToWait, int timeoutInMilisecond){
    int event = 0;
    
    auto checkWritable = [&]() -> int{
        int revent = 0;
        std::lock_guard<std::recursive_mutex> lock(m_txMsgQueueLock);
        if((int)m_txMsgQueue.size() < m_txMsgQueueMaxSize){
            revent |= Event_Writable;
        }
        return revent;
    };
    
    if(eventMustToWait & Event_Writable)
        event = checkWritable();
    
    if(event == Event_Timeout){
//         printDebug("wait writable event\r\n");
        m_eventNotify.wait_for(timeoutInMilisecond);
        if(eventMustToWait & Event_Writable)
            event |= checkWritable();
    }
    
    return event;
}

int GoBackN::pushMessageToLowerLayer(std::shared_ptr<IPacket>& msg){
//     printMessage("tx:", msg);
    return m_wLower(msg->rawData(), msg->rawDataLength(), nullptr);
}

int GoBackN::pushMessageToUpperLayer(std::shared_ptr<IPacket>& msg){
    return m_wUpper(msg->data(), msg->dataLength(), nullptr);
}

GoBackN::GoBackN() : 
ILayer(),
m_rxState(RX_STATE_NORMAL)
{
    m_txSeq       = 0;
    m_seqMax      = 7;
    m_windowSize  = 6;
    m_txMsgQueueMaxSize = 10;
    m_crcEnable   = true;
    m_rxNextSeq   = 0;
    m_txTimeout   = 100;
    m_txIFrameTimePoint = std::chrono::high_resolution_clock::now();
    m_term = false;
}

GoBackN::~GoBackN(){
    stop();
}
int  GoBackN::makeTxSeq(){
    int seq = m_txSeq;
    m_txSeq++;
    if(m_txSeq >= m_seqMax) m_txSeq = 0;
    return seq;
}

int GoBackN::start(){
    m_term = false;
    m_logicThread = std::thread([&](){
        logicThreadFunction();
    });
    return 0;
}

int GoBackN::stop(){
    m_term = true;
    if(m_logicThread.joinable()) m_logicThread.join();
    return 0;
}

int GoBackN::reset(){
    
    // push Reset command to logic
    {
        std::lock_guard<std::recursive_mutex> lock(m_lstCommandLock);
        std::shared_ptr<Command> cmd = std::make_shared<Command>(Command_Reset);
        m_lstCommand.push_back(cmd);
        m_logicNotify.notify_all();
    }
    return 0;
}

int GoBackN::setAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    switch(attrId){
//         case Attribute_Debug:{
//             struct AttributeDebug* db = (struct AttributeDebug*)attr;
//             err = 0;
//             break;
//         }
        case Attribute_WindowSize:{
            struct AttributeWindowSize* ws = (struct AttributeWindowSize*)attr;
            m_windowSize = ws->windowSize;
            m_txMsgQueueMaxSize = ws->windowSize * 2;
            err = 0;
            break;
        }
        case Attribute_Timeout:{
            struct AttributeTimeout* timeout = (struct AttributeTimeout*)attr;
            m_txTimeout = timeout->ackTimeout;
            err = 0;
            break;
        }
        case Attribute_Packet:{
            struct AttributePacket* pkg = (struct AttributePacket*)attr;
            m_seqMax = pkg->seqMaxValue;
            m_crcEnable = pkg->checkValidEnable;
            err = 0;
            break;
        }
        default:
            break;
    }
    
    return err;
}

int GoBackN::getAttribute(int attrId, void* attr, int attrSize){
    int err = -1;
    
    return err;
}
