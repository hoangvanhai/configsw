#ifndef GOBACKN_H
#define GOBACKN_H

#include "ILayer.h"
#include "../socket/SocketUtils.h"
#include <cstdint>
#include <thread>
#include <list>
#include "../../threading/Notify.h"

namespace communication{
    namespace protocol{
        class GoBackN : public ILayer{
        public:
            enum Attribute_ID{
                Attribute_Debug = 0,
                Attribute_WindowSize,
                Attribute_Timeout,
                Attribute_Packet
            };
            struct AttributeDebug{
                bool enable;
                int  index;
            };
            /**
             * @note W <= 2^k - 1, W = WindowSize, k = number of bits for sequence. k = 3 -> W <= 7
             */ 
            struct AttributeWindowSize{
                int  windowSize;
            };
            struct AttributeTimeout{
                int ackTimeout;
            };
            struct AttributePacket{
                bool checkValidEnable;
                int  seqMaxValue;
            };
            GoBackN();
            virtual ~GoBackN();
            /* Override ILayer */
            int setAttribute(int attrId, void* attr, int attrSize) override;
            int getAttribute(int attrId, void* attr, int attrSize) override;
            int writeToLower(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr) override;
            int writeToUpper(const void* data, int dataLength, std::shared_ptr<communication::Object> obj = nullptr) override;
            int waitEvent(int eventNeedToWait, int timeoutInMilisecond) override;
            int start() override;
            int stop() override;
            int reset() override;
            /* Virtual methods */
            virtual std::shared_ptr<IPacket> buildRawMessage(const void* data, int dataLength) = 0;
            virtual std::shared_ptr<IPacket> buildREJMessage(int seq) = 0;
            virtual std::shared_ptr<IPacket> buildResetMessage() = 0;
            virtual std::shared_ptr<IPacket> buildReceiveReadyMessage(int seq) = 0;
            virtual std::shared_ptr<IPacket> buildReceiveReadyRequestMessage() = 0;
            virtual std::shared_ptr<IPacket> buildDataMessage(int sSeq, int rSeq, const void* data, int dataLength) = 0;
            /*  */
            bool crcEnable();
        private:
            /* Variables */
            bool                                m_crcEnable;
            int                                 m_txSeq;
            int                                 m_seqMax;
            volatile bool                       m_term;
            cppframework::threading::Notify                       m_logicNotify;
            std::thread                         m_logicThread;
            void                                logicThreadFunction();

            std::list<std::shared_ptr<IPacket>> m_txMsgQueue;
            std::recursive_mutex                m_txMsgQueueLock;
            int                                 m_txMsgQueueMaxSize;
            cppframework::threading::Notify                       m_eventNotify;

            int                                 m_windowSize;
            std::list<std::shared_ptr<IPacket>> m_txWindowPacket;
            std::list<std::shared_ptr<IPacket>> m_rxWindowPacket;
            std::list<std::shared_ptr<IPacket>> m_rxControlPacket;
            std::recursive_mutex                m_rxWindowPacketLock;
            std::recursive_mutex                m_rxControlPacketLock;
            int                                 m_rxNextSeq;
            int                                 m_txTimeout;
            std::chrono::high_resolution_clock::time_point m_txIFrameTimePoint;
            enum RX_STATE{
                RX_STATE_NORMAL = 0,
                RX_STATE_REJ,           // Missing I-Frame, send a REJ command
            };
            enum RX_STATE                       m_rxState;
            enum Command_ID{
                Command_Reset = 0,
                Command_REJ,
            };
            class Command : public Object{
            public:
                explicit Command(int _id){ m_id = _id;}
                virtual ~Command(){}
                int id(){ return m_id; }
            private:
                int m_id;
            };
            class REJ_Command : public Command{
            public:
                explicit REJ_Command(int _seq) : Command(Command_REJ){
                    m_seq = _seq;
                }
                virtual ~REJ_Command(){}
                int seq(){ return m_seq; }
            private:
                int m_seq;
            };
            std::list<std::shared_ptr<Command>> m_lstCommand;
            std::recursive_mutex                m_lstCommandLock;
            
            int  makeTxSeq();
            void resetCurrentState();
            int  removeMessageInTxWindow(int seq);
            
            int  pushMessageToLowerLayer(std::shared_ptr<IPacket>& msg);
            int  pushMessageToUpperLayer(std::shared_ptr<IPacket>& msg);
            int  push_I_MessageFromLower(std::shared_ptr<IPacket>& msg);
        };
    }
}
#endif // GOBACKN_H
