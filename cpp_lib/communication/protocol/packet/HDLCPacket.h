#ifndef __HDLC_PACKET_H__
#define __HDLC_PACKET_H__

#include "../ILayer.h"
#include <cstdint>
#include <memory>

namespace communication{
    namespace protocol{
        namespace packet{
            class HDLC : public communication::IPacket{
            public:
                HDLC();
                ~HDLC();
                int      importFromRaw(const void* raw, int rawLen) override;

                void*    rawData() override;
                int      rawDataLength() override;
                void*    data() override;
                int      dataLength() override;
                bool     checkValid() override;
                int      receiveSequence() override;
                int      sendSequence() override;
                std::string toString() override;

                bool     isReceiveReady() override;
                bool     isReceiveReadyRequest() override;
                bool     isReject() override;
                bool     isReset() override;
                bool     isDataMessage() override;
                bool     isControlMessage() override;
                
                static std::shared_ptr<HDLC> importMessage(const void* data, int dataLength);
                static std::shared_ptr<HDLC> buildREJMessage(int seq, bool crcEnable = true);
                static std::shared_ptr<HDLC> buildResetMessage(bool crcEnable = true);
                static std::shared_ptr<HDLC> buildReceiveReadyMessage(int seq, bool crcEnable = true);
                static std::shared_ptr<HDLC> buildReceiveReadyRequestMessage(bool crcEnable = true);
                static std::shared_ptr<HDLC> buildDataMessage(int sSeq, int rSeq, const void* data, int dataLength, bool crcEnable = true);
                static int getSequenceMaxValue();
                
				typedef struct
				#if defined(_MSC_VER)
					#pragma pack(push, 1)
				#elif defined(__linux__)
					__attribute__((packed))
				#endif
                {
                    union{
                        uint8_t val;
                        struct{
                            unsigned char extend : 1;
                            unsigned char address: 6;
                            unsigned char cast   : 1;
                        }bits;
                    }address;
                    union{
                        uint8_t val;
                        struct{
                            unsigned char id       : 1;
                            unsigned char send_seq : 3;
                            unsigned char pf       : 1;
                            unsigned char recv_seq : 3;
                        }i_frame_bits;
                        struct{
                            unsigned char id       : 2;
                            unsigned char function : 2;
                            unsigned char pf       : 1;
                            unsigned char recv_seq : 3;
                        }s_frame_bits;
                        struct{
                            unsigned char id       : 2;
                            unsigned char function1: 2;
                            unsigned char pf       : 1;
                            unsigned char function2: 3;
                        }u_frame_bits;
                        struct{
                            unsigned char id       : 1;
                            unsigned char reserver : 7;
                        }id_1;
                        struct{
                            unsigned char id       : 2;
                            unsigned char reserver : 6;
                        }id_2;
                    }control;
				}Header;
				#if defined(_MSC_VER)
				#pragma pack(pop)
				#endif

                enum HEADER_ADDR{
                    HEADER_ADDR_UNICAST    = 0,
                    HEADER_ADDR_MULTICAST  = 1,
                    HEADER_ADDR_EXTEND     = 0,
                    HEADER_ADDR_NO_EXTEND  = 1,
                };
                enum CONTROL_ID{
                    CONTROL_ID_I_FRAME = 0,
                    CONTROL_ID_S_FRAME = 1,
                    CONTROL_ID_U_FRAME = 3,
                };
                enum CONTROL_PF{
                    CONTROL_PF_FINAL = 0,
                    CONTROL_PF_POLL = 1,
                };
                enum CONTROL_S_FRAME{
                    CONTROL_S_FRAME_RR = 0,
                    CONTROL_S_FRAME_RNR = 1,
                    CONTROL_S_FRAME_REJ = 2,
                    CONTROL_S_FRAME_SREJ = 3,
                };
                enum CONTROL_U_FUNCTION{
                    CONTROL_U_RSET_F1 = 3,
                    CONTROL_U_RSET_F2 = 1,
                };
            private:
                Header* header();
				int build(Header& hdr, const void* data, int dataLength, bool makeCRC);

                uint8_t* m_data;
                int      m_dataLength;
            };

        }
    }
}

#endif // HDLCMESSAGE_H
