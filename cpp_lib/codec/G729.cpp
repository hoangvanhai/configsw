#include "../cppframework.h"
#if CPP_CODEC_G729_ENABLED > 0

#include "G729.h"

#include <bcg729/decoder.h>
#include <bcg729/encoder.h>

using namespace cppframework::codec;

G729::G729(){
    m_decoder = initBcg729DecoderChannel();
    m_encoder = initBcg729EncoderChannel();
}
G729::~G729(){
    closeBcg729DecoderChannel((bcg729DecoderChannelContextStruct*)m_decoder);
    closeBcg729EncoderChannel((bcg729EncoderChannelContextStruct*)m_encoder);
}
int G729::encode(const void* din, int dinLen, void* dout, int doutMaxLen){
    bcg729Encoder((bcg729EncoderChannelContextStruct*)m_encoder, (int16_t*)din, (uint8_t*)dout);
    return 10;
}
int G729::decode(const void* din, int dinLen, void* dout, int doutMaxLen){
    uint8_t frameErasureFlag = 0;
    bcg729Decoder((bcg729DecoderChannelContextStruct*)m_decoder, (uint8_t*)din, frameErasureFlag, (int16_t*)dout);
    return 160;
}

#endif
