#ifndef CRC8_H
#define CRC8_H


#ifdef	__cplusplus
extern "C" {
#endif

/*
 * @filename:
 * @description
 *
 * @author
 * @Create date:
 * @Modification History:
 *
 *
*******************************************************************************/


#include <stdint.h>

/***************** Macros (Inline Functions) Definitions *********************/
#define ByteCRC8(crc, ch) 	(sa_bCRC8Table[crc ^ ch])

/************************** Function Prototypes ******************************/
uint8_t crc8(const uint8_t* pBuff, int nSize);
/************************** Variable Definitions *****************************/
extern const uint8_t sa_bCRC8Table[];
extern unsigned char g_lowCrcbyte, g_highCrcbyte;
void CheckCRC(char *str, char len);
uint16_t crc16(uint8_t *str, uint32_t len);
/*****************************************************************************/


#ifdef	__cplusplus
}
#endif


#endif // CRC8_H
