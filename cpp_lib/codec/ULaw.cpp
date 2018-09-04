#include "ULaw.h"
using namespace cppframework::codec;
#include <cstdint>

ULaw::ULaw() : ICodec(){
    BIAS = 0x84;
    CLIP = 32635;
}
ULaw::~ULaw(){
    
}
int ULaw::encode(const void* din, int dinLen, void* dout, int doutMaxLen){
    const static int exp_lut[256] =
    {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    int ret = 0;
    
    int16_t *sin = (int16_t*)din;
    int      sinLen = dinLen / 2;
    uint8_t *sout = (uint8_t*)dout;
    
    for(int i = 0; (i < sinLen) && (doutMaxLen > 0); i++){
        int sample = *sin;
        /* Get the sample into sign-magnitude. */
        int sign = (sample >> 8) & 0x80;      /* set aside the sign */
        if (sign) sample = -sample;       /* get magnitude */
        if (sample > CLIP) sample = CLIP; /* clip the magnitude */

        /* Convert from 16 bit linear to ulaw. */
        sample = sample + BIAS;
        int exponent = exp_lut[( sample >> 7 ) & 0xFF];
        int mantissa = ( sample >> ( exponent + 3 ) ) & 0x0F;
        int ulawbyte = ~ ( sign | ( exponent << 4 ) | mantissa );
        #ifdef ZEROTRAP
        if (ulawbyte == 0) ulawbyte = 0x02;  /* optional CCITT trap */
        #endif
        *sout = ulawbyte;
        sin++;
        sout++;
        doutMaxLen--;
        ret++;
    }

    return ret;
}
int ULaw::decode(const void* din, int dinLen, void* dout, int doutMaxLen){
    const static int exp_lut[8] = { 0, 132, 396, 924, 1980, 4092, 8316, 16764 };
    int ret = 0;
    
    uint8_t *sin  = (uint8_t*)din;
    int16_t *sout = (int16_t*)dout;
    int      soutMaxLen = doutMaxLen / 2;
    
    for(int i = 0; (i < dinLen) && (soutMaxLen > 0); i++){
        unsigned int ulawbyte = *sin;
        ulawbyte = ~ ulawbyte;
        int sign = ( ulawbyte & 0x80 );
        int exponent = ( ulawbyte >> 4 ) & 0x07;
        int mantissa = ulawbyte & 0x0F;
        int sample = exp_lut[exponent] + ( mantissa << ( exponent + 3 ) );
        if (sign) sample = -sample;
        
        *sout = sample;
        sout++;
        sin++;
        soutMaxLen--;
        ret ++;
    }

    return ret * 2;
}

// end of file
