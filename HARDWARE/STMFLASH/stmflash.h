#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

#define STM32_FLASH_SIZE 256
#define STM32_FLASH_WREN 1
#define STM32_FLASH_BASE 0x08000000
 
u16 STMFLASH_ReadHalfWord(u32 faddr);
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);

void Test_Write(u32 WriteAddr,u16 WriteData);

#endif
