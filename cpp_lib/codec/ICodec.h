#ifndef __CPP_CODEC_H__
#define __CPP_CODEC_H__

namespace cppframework{
    namespace codec{
        class ICodec{
        public:
            ~ICodec(){}
            
            virtual int encode(const void* din, int dinLen, void* dout, int doutMaxLen) = 0;
            virtual int decode(const void* din, int dinLen, void* dout, int doutMaxLen) = 0;
        };
    }
}

#endif

