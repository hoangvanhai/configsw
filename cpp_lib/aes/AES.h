#ifndef AES_H_
#define AES_H_

/** @file .h
 *  @brief header file.
 *
 *  
 *
 *  @author Tung Dang (TungDH2)
 *  @note 
 *  @bug 
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  	 Date        Changes
 * ----- -------- ---------- -----------------------------------------------
 * 0.01  TungDH2  06/12/2011 
 * 
 * 
 *</pre>
 ******************************************************************************/

/***************************** Include Files *********************************/
 #include <stdio.h>
 
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

#define BIG_ENDIAN      1   // do not change
#define LITTLE_ENDIAN   2   // do not change

#define BLOCK_SIZE  16
#define INTERNAL_BYTE_ORDER BIG_ENDIAN
#define EXTERNAL_BYTE_ORDER BIG_ENDIAN

//============================================
 #if defined(BLOCK_SIZE)                                                                               
 #define nc   (Ncol)                                                                                   
 #else                                                                                                 
 #define nc   (cx->Ncol)                                                                               
 #endif     
 #define Nr(Nk,Nc)	((Nk > Nc ? Nk : Nc) + 6)
//============================================

#if !defined(BLOCK_SIZE) || (BLOCK_SIZE == 32)
#define KS_LENGTH   120
#define RC_LENGTH    29
#else
#define KS_LENGTH   (4 * BLOCK_SIZE)
#define RC_LENGTH   ((9 * BLOCK_SIZE) / 8 - 8)
#endif

typedef unsigned char   byte;           // must be an 8-bit storage unit
typedef unsigned long   word;           // must be a 32-bit storage unit
typedef short           aes_ret;        // function return value

#define aes_bad     0
#define aes_good    1

#if(INTERNAL_BYTE_ORDER == LITTLE_ENDIAN)

#define upr(x,n)        (((x) << 8 * (n)) | ((x) >> (32 - 8 * (n))))
#define ups(x,n)        ((x) << 8 * (n))
#define bval(x,n)       (((byte)((x) >> 8 * (n)))&0xff)
#define byte_swap(x)    (upr(x,1) & 0x00ff00ff | upr(x,3) & 0xff00ff00)
#define bytes2word(b0, b1, b2, b3)  \
		((word)(b3) << 24 | (word)(b2) << 16 | (word)(b1) << 8 | (b0))
#else

#define upr(x,n)        (((x) >> 8 * (n)) | ((x) << (32 - 8 * (n))))
#define ups(x,n)        ((x) >> 8 * (n)))
#define bval(x,n)       (((byte)((x) >> 24 - 8 * (n)))&0xff)
#define byte_swap(x)    (upr(x,3) & 0x00ff00ff | upr(x,1) & 0xff00ff00)
#define bytes2word(b0, b1, b2, b3)  \
		((word)(b0) << 24 | (word)(b1) << 16 | (word)(b2) << 8 | (b3))
#endif

#if(INTERNAL_BYTE_ORDER == EXTERNAL_BYTE_ORDER)

#define word_in(x)      *(word*)(x)
#define word_out(x,v)   *(word*)(x) = (v)

#else

#define word_in(x)      byte_swap(*(word*)(x))
#define word_out(x,v)   *(word*)(x) = byte_swap(v)

#endif

enum aes_const  {   Nrow =  4,  // the number of rows in the cipher state
					Mcol =  8,  // maximum number of columns in the state
#if defined(BLOCK_SIZE)         // set up a statically defined block size
					Ncol =  BLOCK_SIZE / 4,
					Shr0 =  0,  // the cyclic shift values for rows 0, 1, 2 & 3
					Shr1 =  1,
					Shr2 =  BLOCK_SIZE == 32 ? 3 : 2,
					Shr3 =  BLOCK_SIZE == 32 ? 4 : 3
#endif
				};

enum aes_key    {   enc  =  1,  // set if encryption is needed
					dec  =  2,  // set if decryption is needed
					both =  3   // set if both are needed
				};


typedef struct
{
	word    Nkey;               // the number of words in the key input block
	word    Nrnd;               // the number of cipher rounds
	word    e_key[KS_LENGTH];   // the encryption key schedule
	word    d_key[KS_LENGTH];   // the decryption key schedule
	byte    mode;               // encrypt, decrypt or both
} aes;

aes_ret set_key(const byte key[], const word n_bytes, const enum aes_key f, aes *cx);
aes_ret encrypt(const byte in_blk[], byte out_blk[], const aes *cx);
aes_ret decrypt(const byte in_blk[], byte out_blk[], const aes *cx);

/*****************************************************************************/



#endif /*AES_H_*/
