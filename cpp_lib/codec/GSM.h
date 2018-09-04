/**
 * GSM 06.10 use libgsm
 * http://www.quut.com/berlin/gsm
 */
#ifndef __CPP_CODEC_GSM_H__
#define __CPP_CODEC_GSM_H__

#include "ICodec.h"

namespace cppframework{
    namespace codec{
        class GSM : public ICodec{
        public:
            GSM();
            virtual ~GSM();
            /* Override ICodec */
            /** @brief Encode 160 samples @ int16_t Linear to 33 byte gsm
             */
            int encode(const void* din, int dinLen, void* dout, int doutMaxLen);
            int decode(const void* din, int dinLen, void* dout, int doutMaxLen);
        private:
            void* m_encode;
            void* m_decode;
        };
    }
}

#endif

