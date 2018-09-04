#ifndef __CPP_CODEC_ULAW_H__
#define __CPP_CODEC_ULAW_H__

#include "ICodec.h"

namespace cppframework{
    namespace codec{
        class ULaw : public ICodec{
        public:
            ULaw();
            virtual ~ULaw();
            /* Override ICodec */
            /** @brief Encode int16_t Linear array to int8_t PCMU array
             * 
             */
            int encode(const void* din, int dinLen, void* dout, int doutMaxLen);
            int decode(const void* din, int dinLen, void* dout, int doutMaxLen);
        private:
              int BIAS;   /* define the add-in bias for 16 bit samples */
              int CLIP;
        };
    }
}

#endif
