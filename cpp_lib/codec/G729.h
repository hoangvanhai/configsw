#ifndef __CPP_CODEC_G729_H__
#define __CPP_CODEC_G729_H__

#include "ICodec.h"

namespace cppframework{
    namespace codec{
        class G729 : public ICodec{
        public:
            G729();
            virtual ~G729();
            /* Override ICodec */
            /** @brief Encode 80 samples @ int16_t Linear to 80 bit g729
             */
            int encode(const void* din, int dinLen, void* dout, int doutMaxLen);
            int decode(const void* din, int dinLen, void* dout, int doutMaxLen);
        private:
            void* m_decoder;
            void* m_encoder;
        };
    }
}

#endif
