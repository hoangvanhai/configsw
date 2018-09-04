/** @file .c
 *  @brief 
 *
 *  This is template file for coding style using 
 *  in SDR6188 project.
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

/*****************************************************************************/

 #include "AES.h"   

                                                                                                                                                                                                                                                                                             
 typedef byte   wa_ptr[4];                                                                                                                                                                               
 
 byte 	m_poly 	= 0x1b;    	// original : const byte 	m_poly = 0x1b;                                                                                                                                                                         
 
 int 		mod 		= 0;    		// true (1) if decryption order is modified    
  
const byte inv_tab[] =    				// Table of finite field inverses                                                 
 {                                                                                                     
     0x00, 0x01, 0x8d, 0xf6, 0xcb, 0x52, 0x7b, 0xd1,                                                   
     0xe8, 0x4f, 0x29, 0xc0, 0xb0, 0xe1, 0xe5, 0xc7,                                                   
     0x74, 0xb4, 0xaa, 0x4b, 0x99, 0x2b, 0x60, 0x5f,                                                   
     0x58, 0x3f, 0xfd, 0xcc, 0xff, 0x40, 0xee, 0xb2,                                                   
     0x3a, 0x6e, 0x5a, 0xf1, 0x55, 0x4d, 0xa8, 0xc9,                                                   
     0xc1, 0x0a, 0x98, 0x15, 0x30, 0x44, 0xa2, 0xc2,                                                   
     0x2c, 0x45, 0x92, 0x6c, 0xf3, 0x39, 0x66, 0x42,                                                   
     0xf2, 0x35, 0x20, 0x6f, 0x77, 0xbb, 0x59, 0x19,                                                   
     0x1d, 0xfe, 0x37, 0x67, 0x2d, 0x31, 0xf5, 0x69,                                                   
     0xa7, 0x64, 0xab, 0x13, 0x54, 0x25, 0xe9, 0x09,                                                   
     0xed, 0x5c, 0x05, 0xca, 0x4c, 0x24, 0x87, 0xbf,                                                   
     0x18, 0x3e, 0x22, 0xf0, 0x51, 0xec, 0x61, 0x17,                                                   
     0x16, 0x5e, 0xaf, 0xd3, 0x49, 0xa6, 0x36, 0x43,                                                   
     0xf4, 0x47, 0x91, 0xdf, 0x33, 0x93, 0x21, 0x3b,                                                   
     0x79, 0xb7, 0x97, 0x85, 0x10, 0xb5, 0xba, 0x3c,                                                   
     0xb6, 0x70, 0xd0, 0x06, 0xa1, 0xfa, 0x81, 0x82,                                                   
     0x83, 0x7e, 0x7f, 0x80, 0x96, 0x73, 0xbe, 0x56,                                                   
     0x9b, 0x9e, 0x95, 0xd9, 0xf7, 0x02, 0xb9, 0xa4,                                                   
     0xde, 0x6a, 0x32, 0x6d, 0xd8, 0x8a, 0x84, 0x72,                                                   
     0x2a, 0x14, 0x9f, 0x88, 0xf9, 0xdc, 0x89, 0x9a,                                                   
     0xfb, 0x7c, 0x2e, 0xc3, 0x8f, 0xb8, 0x65, 0x48,                                                   
     0x26, 0xc8, 0x12, 0x4a, 0xce, 0xe7, 0xd2, 0x62,                                                   
     0x0c, 0xe0, 0x1f, 0xef, 0x11, 0x75, 0x78, 0x71,                                                   
     0xa5, 0x8e, 0x76, 0x3d, 0xbd, 0xbc, 0x86, 0x57,                                                   
     0x0b, 0x28, 0x2f, 0xa3, 0xda, 0xd4, 0xe4, 0x0f,                                                   
     0xa9, 0x27, 0x53, 0x04, 0x1b, 0xfc, 0xac, 0xe6,                                                   
     0x7a, 0x07, 0xae, 0x63, 0xc5, 0xdb, 0xe2, 0xea,                                                   
     0x94, 0x8b, 0xc4, 0xd5, 0x9d, 0xf8, 0x90, 0x6b,                                                   
     0xb1, 0x0d, 0xd6, 0xeb, 0xc6, 0x0e, 0xcf, 0xad,                                                   
     0x08, 0x4e, 0xd7, 0xe3, 0x5d, 0x50, 0x1e, 0xb3,                                                   
     0x5b, 0x23, 0x38, 0x34, 0x68, 0x46, 0x03, 0x8c,                                                   
     0xdd, 0x9c, 0x7d, 0xa0, 0xcd, 0x1a, 0x41, 0x1c                                                    
 }; 
   
 //=============================================================================  
 //  multiply x by 0x02 in finite field GF(2^8)                                                                                                                                                                      
 byte FFmulX(const byte a)                                                                             
 {                                                                                                     
     return ((a << 1) ^ (a & 0x80 ? m_poly : 0))&0xff;                                                        
 }
 //=============================================================================    
  byte fwd_affine(byte x)                                                                               
 {   word w = x;                                                                                       
                                                                                                       
     w ^= (w << 1) ^ (w << 2) ^ (w << 3) ^ (w << 4);                                                                                                                                                       
     
     return  0x63 ^ (byte)((w ^ (w >> 8))&0xFF); // original: return  0x63 ^ (byte)(w ^ (w >> 8));                                                           
 } 
 //=============================================================================
  byte inv_affine(byte x)                                                                               
 {   word w = x;                                                                                       
                                                                                                       
     w = (w << 1) ^ (w << 3) ^ (w << 6);                                                               
                                                                                                       
     return 0x05 ^ (byte)((w ^ (w >> 8))&0x00FF); //original: return 0x05 ^ (byte)(w ^ (w >> 8));                                                             
 } 

 //=============================================================================
 // the SubWord transformation performed on the bytes in a column                                                                                                                                           
 word SubWord(const word x)                                                                            
 {                                                                                                     
     return  bytes2word(                                                                               
                     fwd_affine(inv_tab[bval(x,0)]),                                                   
                     fwd_affine(inv_tab[bval(x,1)]),                                                   
                     fwd_affine(inv_tab[bval(x,2)]),                                                   
                     fwd_affine(inv_tab[bval(x,3)])  );                                                
 }  
 //=============================================================================
 // the RotWord transformation performed on the bytes in a column                                      
                                                                                                       
 word RotWord(const word x)                                                                            
 {                                                                                                     
     return  upr(x,3);                                                                                 
 }  
 
  //=============================================================================    
 // the SubBytes transformation performed on all bytes in the state                                                                                                                                          
/* void SubBytes2(byte state[][4], word Ncol)                                                             
 {   word col;                                                                                         
     for(col = 0; col < Ncol; ++col)                                                                   
     {                                                                                                 
         state[col][0] = fwd_affine(inv_tab[state[col][0]]);                                           
         state[col][1] = fwd_affine(inv_tab[state[col][1]]);                                           
         state[col][2] = fwd_affine(inv_tab[state[col][2]]);                                           
         state[col][3] = fwd_affine(inv_tab[state[col][3]]);                                           
     }                                                                                                 
 } */

 //=============================================================================    
 // the SubBytes transformation performed on all bytes in the state                                                                                                                                          
 void SubBytes(byte state[][4], word Ncol)                                                             
 {   word col;                                                                                         
     for(col = 0; col < Ncol; ++col)                                                                   
     {                                                                                                 
         state[col][0] = fwd_affine(inv_tab[state[col][0]]);                                           
         state[col][1] = fwd_affine(inv_tab[state[col][1]]);                                           
         state[col][2] = fwd_affine(inv_tab[state[col][2]]);                                           
         state[col][3] = fwd_affine(inv_tab[state[col][3]]);                                           
     }                                                                                                 
 }  

 //=============================================================================    
 // the inverse SubBytes transformation performed on all bytes in the state                                                                                                                  
 void InvSubBytes(byte state[][4], word Ncol)                                                          
 {   word col;                                                                                         
     for(col = 0; col < Ncol; ++col)                                                                   
     {                                                                                                 
         state[col][0] = inv_tab[inv_affine(state[col][0])];                                           
         state[col][1] = inv_tab[inv_affine(state[col][1])];                                           
         state[col][2] = inv_tab[inv_affine(state[col][2])];                                           
         state[col][3] = inv_tab[inv_affine(state[col][3])];                                           
     }                                                                                                 
 } 
 
 //=============================================================================   
 // the ShiftRows transformation performed on rows 1, 2 and 3 of the state                                                                                                                                 
 void ShiftRows(byte state[][4], const word Ncol, const word Shr[])                                    
 {   word    col, t[Mcol];                                                                             
                                                                                                       
     for(col = 0; col < Ncol; ++col)                                                                   
         t[col] = bytes2word(0, state[col][1], state[col][2], state[col][3]);                          
                                                                                                       
     for(col = 0; col < Ncol; ++col)                                                                   
     {                                                                                                 
         state[col][1] = bval(t[(col + Shr[1]) & 0x03], 1);                                            
         state[col][2] = bval(t[(col + Shr[2]) & 0x03], 2);                                            
         state[col][3] = bval(t[(col + Shr[3]) & 0x03], 3);                                            
     }                                                                                                 
 }
 
  //=============================================================================   
 // the ShiftRows transformation performed on rows 1, 2 and 3 of the state                                                                                                                                 
/* void ShiftRows2(byte state[][4], const word Ncol, const word Shr[])                                    
 {   word    col, t[Mcol];                                                                             
                                                                                                       
     for(col = 0; col < Ncol; ++col)                                                                   
         t[col] = bytes2word(0, state[col][1], state[col][2], state[col][3]);                          
                                                                                                       
     for(col = 0; col < Ncol; ++col)                                                                   
     {                                                                                                 
         state[col][1] = bval(t[(col + Shr[1]) & 0x03], 1);                                            
         state[col][2] = bval(t[(col + Shr[2]) & 0x03], 2);                                            
         state[col][3] = bval(t[(col + Shr[3]) & 0x03], 3);                                            
     }                                                                                                 
 }*/
 
 //=============================================================================     
 // the inverse ShiftRows transformation performed on rows 1, 2 and 3 of state                                                                      
 void InvShiftRows(byte state[][4], const word Ncol, const word Shr[])                                 
 {   word    col, t[Mcol];                                                                             
                                                                                                       
     for(col = 0; col < Ncol; ++col)                                                                   
         t[col] = bytes2word(0, state[col][1], state[col][2], state[col][3]);                          
                                                                                                       
     for(col = 0; col < Ncol; ++col)                                                                   
     {                                                                                                 
         state[(col + Shr[1]) & 0x03][1] = bval(t[col], 1);                                            
         state[(col + Shr[2]) & 0x03][2] = bval(t[col], 2);                                            
         state[(col + Shr[3]) & 0x03][3] = bval(t[col], 3);                                            
     }                                                                                                 
 }  
  
//=============================================================================    
// the MixColumns transformation performed on all columns of the state                                                                                                                                      
 void MixColumns(byte state[][4], word Ncol)                                                           
 {   word col;                                                                                         
     byte t, u;                                                                                        
     for(col = 0; col < Ncol; ++col)                                                                   
     {   t = state[col][0];                                                                            
         u = state[col][0] ^ state[col][1] ^ state[col][2] ^ state[col][3];                            
                                                                                                       
         state[col][0] ^= u ^ FFmulX(t ^ state[col][1]);                                               
         state[col][1] ^= u ^ FFmulX(state[col][1] ^ state[col][2]);                                   
         state[col][2] ^= u ^ FFmulX(state[col][2] ^ state[col][3]);                                   
         state[col][3] ^= u ^ FFmulX(state[col][3] ^ t);                                               
     }                                                                                                 
 } 
 
 //=============================================================================    
// the MixColumns transformation performed on all columns of the state                                                                                                                                      
/* void MixColumns2(byte state[][4], word Ncol)                                                           
 {   word col;                                                                                         
     byte t, u;                                                                                        
     for(col = 0; col < Ncol; ++col)                                                                   
     {   t = state[col][0];                                                                            
         u = state[col][0] ^ state[col][1] ^ state[col][2] ^ state[col][3];                            
                                                                                                       
         state[col][0] ^= u ^ FFmulX(t ^ state[col][1]);                                               
         state[col][1] ^= u ^ FFmulX(state[col][1] ^ state[col][2]);                                   
         state[col][2] ^= u ^ FFmulX(state[col][2] ^ state[col][3]);                                   
         state[col][3] ^= u ^ FFmulX(state[col][3] ^ t);                                               
     }                                                                                                 
 }*/ 

  //=============================================================================
  // the inverse MixColumns transformation performed on all columns of the state                        
                                                                                                     
 void InvMixColumns(byte state[][4], word Ncol)                                                        
 {   word col;                                                                                         
     byte t, u, v;                                                                                     
     for(col = 0; col < Ncol; ++col)                                                                   
     {   t = state[col][0];                                                                            
         u = state[col][0] ^ state[col][1] ^ state[col][2] ^ state[col][3];                            
                                                                                                       
         u ^= FFmulX(FFmulX(FFmulX(u)));                                                               
         v = u ^ FFmulX(FFmulX(state[col][1] ^ state[col][3]));                                        
         u ^= FFmulX(FFmulX(state[col][0] ^ state[col][2]));                                           
                                                                                                       
         state[col][0] ^= u ^ FFmulX(t ^ state[col][1]);                                               
         state[col][1] ^= v ^ FFmulX(state[col][1] ^ state[col][2]);                                   
         state[col][2] ^= u ^ FFmulX(state[col][2] ^ state[col][3]);                                   
         state[col][3] ^= v ^ FFmulX(state[col][3] ^ t);                                               
     }                                                                                                 
 } 
 
 //=============================================================================    
 // XorRoundKey - add the round key to the state                                                                                                                                                              
 void XorRoundKey(byte state[][4], const word rk[], const word Ncol)                                   
 {   word col, t;                                                                                      
     for(col = 0; col < Ncol; ++col)                                                                   
     {   t = rk[col];                                                                                  
                                                                                                       
         state[col][0] ^= bval(t,0);                                                                   
         state[col][1] ^= bval(t,1);                                                                   
         state[col][2] ^= bval(t,2);                                                                   
         state[col][3] ^= bval(t,3);                                                                   
     }                                                                                                 
 }
 
  //=============================================================================    
 // XorRoundKey - add the round key to the state                                                                                                                                                              
/* void XorRoundKey2(byte state[][4], const word rk[], const word Ncol)                                   
 {   word col, t;                                                                                      
     for(col = 0; col < Ncol; ++col)                                                                   
     {   t = rk[col];                                                                                  
                                                                                                       
         state[col][0] ^= bval(t,0);                                                                   
         state[col][1] ^= bval(t,1);                                                                   
         state[col][2] ^= bval(t,2);                                                                   
         state[col][3] ^= bval(t,3);                                                                   
     }                                                                                                 
 }*/
 
 //=============================================================================
 // input the state from the input byte array                                                                   
 void GetState(byte state[][4], const byte in[], const word Ncol)                                      
 {   word col, i;                                                                                      
     for(col = 0, i = 0; col < Ncol; ++col)                                                            
     {                                                                                                 
         state[col][0] = in[i++];                                                                      
         state[col][1] = in[i++];                                                                      
         state[col][2] = in[i++];                                                                      
         state[col][3] = in[i++];                                                                      
     }                                                                                                 
 } 
 
 //=============================================================================
 // input the state from the input byte array                                                                   
/* void GetState2(byte state[][4], const byte in[], const word Ncol)                                      
 {   word col, i;                                                                                      
     for(col = 0, i = 0; col < Ncol; ++col)                                                            
     {                                                                                                 
         state[col][0] = in[i++];                                                                      
         state[col][1] = in[i++];                                                                      
         state[col][2] = in[i++];                                                                      
         state[col][3] = in[i++];                                                                      
     }                                                                                                 
 } */

 //=============================================================================    
 // output the state to the output byte array                                                                                                                                                                
 void PutState(byte state[][4], byte out[], const word Ncol)                                     
 {   word col, i;    
 	   i = 0;                                                                               
     for(col = 0 ; col < Ncol; ++col)    // for(col = 0, i = 0; col < Ncol; ++col)                                                        
     {                                                                                                 
         out[i++] = state[col][0];                                                                     
         out[i++] = state[col][1];                                                                     
         out[i++] = state[col][2];                                                                     
         out[i++] = state[col][3];                                                                     
     }                                                                                                 
 } 
 
  //=============================================================================    
 // output the state to the output byte array                                                                                                                                                                
 /*void PutState2(byte state[][4], byte out[], const word Ncol)                                     
 {   word col, i;    
 	   i = 0;                                                                               
     for(col = 0 ; col < Ncol; ++col)    // for(col = 0, i = 0; col < Ncol; ++col)                                                        
     {                                                                                                 
         out[i++] = state[col][0];                                                                     
         out[i++] = state[col][1];                                                                     
         out[i++] = state[col][2];                                                                     
         out[i++] = state[col][3];                                                                     
     }                                                                                                 
 } */
 
 //=============================================================================
 // Key Expansion - expand the key to form the key schedule from the                                   
 // user supplied key, where Nk is the key length in bits divided by                                   
 // 32 with a value of 4, 5, 6, 7 or 8                                                    
 /*aes_ret set_key2(const byte key[], const word n_bytes, const enum aes_key f, aes *cx)                                                                                                      
 {   word        i;                                                                                    
     byte        rc;                                                                                   
     const byte  *p;                                                                                   
                                                                                                       
		 cx->mode = (byte)f;                                                     
     cx->Nkey = n_bytes >> 2;                                                                          
     cx->Nrnd = Nr(cx->Nkey, nc);                                                                      
                                                                                                       
     i = 0; p = key;                                                                                   
     while (i < cx->Nkey)                                                                              
     {                                                                                                 
         cx->e_key[i] = bytes2word(p[0], p[1], p[2], p[3]);                                            
                                                                                                       
         i = i + 1; p += Nrow;                                                                         
     }                                                                                                 
                                                                                                       
     i = cx->Nkey; rc = 1;                                                                             
     while(i < nc * (cx->Nrnd + 1))                                                                    
     {                                                                                                 
         word    t = cx->e_key[i - 1];                                                                 
                                                                                                       
         if(i % cx->Nkey == 0)                                                                         
         {                                                                                             
             t = SubWord(RotWord(t)) ^ bytes2word(rc, 0, 0, 0); rc = FFmulX(rc);   //modify                    
         }                                                                                             
         else if(cx->Nkey == 8 && i % cx->Nkey == 4) //(cx->Nkey > 6 && i % cx->Nkey == 4)                                             
                                                                                                       
             t = SubWord(t);                                                                           
                                                                                                       
         cx->e_key[i] = cx->e_key[i - cx->Nkey] ^ t;                                                   
                                                                                                       
         i = i + 1;                                                                                    
     }                                                                                                 
                                                                                                       
     if(cx->mode != enc)                                                                               
     {                                                                                                 
         for(i = 0; i < (cx->Nrnd + 1) * nc; ++i)                                                      
             cx->d_key[i] = cx->e_key[i];                                                              
                                                                                                       
         for(i = 1; i < cx->Nrnd; ++i)                                                                 
             InvMixColumns((wa_ptr*)(cx->d_key + i * nc), nc);                                         
     }                                                                                                 
                                                                                                       
 //  reverse(nc, Nrnd, Nkey, e_key);                                                                   
                                                                                                       
     return aes_good;                                                                                  
 }*/
 
 //=============================================================================
 // Key Expansion - expand the key to form the key schedule from the                                   
 // user supplied key, where Nk is the key length in bits divided by                                   
 // 32 with a value of 4, 5, 6, 7 or 8                                                    
 aes_ret set_key(const byte key[], const word n_bytes, const enum aes_key f, aes *cx)                                                                                                      
 {   word        i;                                                                                    
     byte        rc;                                                                                   
     const byte  *p;                                                                                                                                                                                          	 
	 	
	 cx->mode = (byte)f;                                                     
     cx->Nkey = n_bytes >> 2;                                                                          
     cx->Nrnd = Nr(cx->Nkey, nc);                                                                      
                                                                                                          	 
     i = 0; p = key;                                                                                   
     while (i < cx->Nkey)                                                                              
     {                                                                                                 
         cx->e_key[i] = bytes2word(p[0], p[1], p[2], p[3]);                                            
                                                                                                       
         i = i + 1; p += Nrow;                                                                         
     }                                                                                                 
                                                                                                       
     i = cx->Nkey; rc = 1;                                                                             
     while(i < nc * (cx->Nrnd + 1))                                                                    
     {                                                                                                 
         word    t = cx->e_key[i - 1];                                                                 
                                                                                                       
         if(i % cx->Nkey == 0)                                                                         
         {                                                                                             
             t = SubWord(RotWord(t)) ^ bytes2word(rc, 0, 0, 0); rc = FFmulX(rc);   //modify                    
         }                                                                                             
         else if(cx->Nkey == 8 && i % cx->Nkey == 4) //(cx->Nkey > 6 && i % cx->Nkey == 4)                                             
                                                                                                       
             t = SubWord(t);                                                                           
                                                                                                       
         cx->e_key[i] = cx->e_key[i - cx->Nkey] ^ t;                                                   
                                                                                                       
         i = i + 1;                                                                                    
     }                                                                                                 
                                                                                                       
     if(cx->mode != enc)                                                                               
     {                                                                                                 
         for(i = 0; i < (cx->Nrnd + 1) * nc; ++i)                                                      
             cx->d_key[i] = cx->e_key[i];                                                              
                                                                                                       
         for(i = 1; i < cx->Nrnd; ++i)                                                                 
             InvMixColumns((wa_ptr*)(cx->d_key + i * nc), nc);                                         
     }                                                                                                 
                                                                                                       
 //  reverse(nc, Nrnd, Nkey, e_key);                                                                   
                                                                                                       
     return aes_good;                                                                                  
 }

 //=============================================================================     
 // The Forward Cipher                                                                                 
 aes_ret encrypt(const byte input[], byte output[], const aes *cx)                                                                                                                            
 {   byte state[Mcol][4];                                                                              
     word Shr[Nrow], rnd;                                                                              
                                                                                                                                                                                                                                                                      
     Shr[1] = 1; 
     Shr[2] = (nc == 8 ? 3 : 2); 
     Shr[3] = (nc  > 6 ? 4 : 3);   
                                                                                                    
     GetState(state, input, nc);                                                                       
     XorRoundKey(state, cx->e_key, nc);                                                                
                                                                                                       
     for(rnd = 1; rnd < cx->Nrnd; ++rnd)                                                               
     {                                                                                                 
         SubBytes(state, nc);                                                                          
         ShiftRows(state, nc, Shr);                                                                    
         MixColumns(state, nc);                                                                        
         XorRoundKey(state, cx->e_key + rnd * nc, nc);                                                 
     }                                                                                                 
                                                                                                       
     SubBytes(state, nc);                                                                              
     ShiftRows(state, nc, Shr);                                                                        
                                                                                                       
     XorRoundKey(state, cx->e_key + cx->Nrnd * nc, nc);                                                
     PutState(state, output, nc);                                                                      
                                                                                                       
     return aes_good;                                                                                  
 }   
 
 //=============================================================================     
 // The Forward Cipher                                                                                 
/* aes_ret encrypt2(const byte input[], byte output[], const aes *cx)                                                                                                                            
 {   byte state[Mcol][4];                                                                              
     word Shr[Nrow], rnd;                                                                              
                                                                                                                                                                                                                                                                      
     Shr[1] = 1; Shr[2] = (nc == 8 ? 3 : 2); Shr[3] = (nc  > 6 ? 4 : 3);   
                                                                                                       
     GetState2(state, input, nc);                                                                       
     XorRoundKey2(state, cx->e_key, nc);                                                                
                                                                                                       
     for(rnd = 1; rnd < cx->Nrnd; ++rnd)                                                               
     {                                                                                                 
         SubBytes2(state, nc);                                                                          
         ShiftRows2(state, nc, Shr);                                                                    
         MixColumns2(state, nc);                                                                        
         XorRoundKey2(state, cx->e_key + rnd * nc, nc);                                                 
     }                                                                                                 
                                                                                                       
     SubBytes2(state, nc);                                                                              
     ShiftRows2(state, nc, Shr);                                                                        
                                                                                                       
     XorRoundKey2(state, cx->e_key + cx->Nrnd * nc, nc);                                                
     PutState2(state, output, nc);                                                                      
                                                                                                       
     return aes_good;                                                                                  
 }  */ 
 
 //=============================================================================     
 // The Inverse Cipher (in normal and modified equivalent form)                                                                            
 aes_ret decrypt(const byte input[], byte output[], const aes *cx)                     
 {   byte state[Mcol][4];                                                                              
     word Shr[Nrow], rnd;                                                                                                                                                                                                                                            
                                                                                                       
     Shr[1] = 1;
     Shr[2] = (nc == 8 ? 3 : 2); 
     Shr[3] = (nc  > 6 ? 4 : 3);                                                                       
                                                                                                       
     GetState(state, input, nc);                                                                       
     XorRoundKey(state, cx->e_key + cx->Nrnd * nc, nc);                                                
                                                                                                       
     for(rnd = cx->Nrnd - 1; rnd > 0; --rnd)                                                           
     {                                                                                                 
         if(mod)                                                                                       
         {                                                                                             
             InvSubBytes(state, nc);                                                                   
             InvShiftRows(state, nc, Shr);                                                             
             InvMixColumns(state, nc);                                                                 
             XorRoundKey(state, cx->d_key + rnd * nc, nc);                                             
         }                                                                                             
         else                                                                                          
         {                                                                                             
             InvShiftRows(state, nc, Shr);                                                             
             InvSubBytes(state, nc);                                                                   
             XorRoundKey(state, cx->e_key + rnd * nc, nc);                                             
             InvMixColumns(state, nc);                                                                 
         }                                                                                             
     }                                                                                                 
                                                                                                       
     if(mod)                                                                                           
     {                                                                                                 
         InvSubBytes(state, nc);                                                                       
         InvShiftRows(state, nc, Shr);                                                                 
     }                                                                                                 
     else                                                                                              
     {                                                                                                 
         InvShiftRows(state, nc, Shr);                                                                 
         InvSubBytes(state, nc);                                                                       
     }                                                                                                 
                                                                                                       
     XorRoundKey(state, cx->e_key, nc);                                                                
     PutState(state, output, nc);                                                                      
                                                                                                       
     return aes_good;                                                                                  
 }  
