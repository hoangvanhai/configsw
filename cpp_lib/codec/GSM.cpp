#include "../cppframework.h"
#if CPP_CODEC_GSM_ENABLED > 0

#include "GSM.h"

/* Copy from gsm.h for resolve unreference error */
#ifdef __cplusplus
extern "C" {
#endif
#include <cstdio>
#define    GSM_P( protos )      protos
typedef struct gsm_state *      gsm;
typedef short                   gsm_signal;        /* signed 16 bit */
typedef unsigned char           gsm_byte;
typedef gsm_byte                gsm_frame[33];     /* 33 * 8 bits     */

#define    GSM_MAGIC            0xD                /* 13 kbit/s RPE-LTP */

#define    GSM_PATCHLEVEL       16
#define    GSM_MINOR            0
#define    GSM_MAJOR            1

#define    GSM_OPT_VERBOSE      1
#define    GSM_OPT_FAST         2
#define    GSM_OPT_LTP_CUT      3
#define    GSM_OPT_WAV49        4
#define    GSM_OPT_FRAME_INDEX  5
#define    GSM_OPT_FRAME_CHAIN  6

extern gsm  gsm_create  GSM_P((void));
extern void gsm_destroy GSM_P((gsm));

extern int  gsm_print   GSM_P((FILE *, gsm, gsm_byte  *));
extern int  gsm_option  GSM_P((gsm, int, int *));

extern void gsm_encode  GSM_P((gsm, gsm_signal *, gsm_byte  *));
extern int  gsm_decode  GSM_P((gsm, gsm_byte   *, gsm_signal *));

extern int  gsm_explode GSM_P((gsm, gsm_byte   *, gsm_signal *));
extern void gsm_implode GSM_P((gsm, gsm_signal *, gsm_byte   *));
#ifdef __cplusplus
}
#endif

using namespace cppframework::codec;

GSM::GSM(){
    m_encode = gsm_create();
    m_decode = gsm_create();
}
GSM::~GSM(){
    if(m_encode)
        gsm_destroy((gsm)m_encode);
    if(m_decode)
        gsm_destroy((gsm)m_decode);
    m_encode = 0;
    m_decode = 0;
}
int GSM::encode(const void* din, int dinLen, void* dout, int doutMaxLen){
    if(m_encode){
        gsm_encode((gsm)m_encode, (gsm_signal*)din, (gsm_byte*)dout);
        return 33;
    }
    return 0;
}
int GSM::decode(const void* din, int dinLen, void* dout, int doutMaxLen){
    if(m_decode){
        gsm_decode((gsm)m_decode, (gsm_byte*)din, (gsm_signal*)dout);
        return 160 * 2;
    }
    return 0;
}

#endif

