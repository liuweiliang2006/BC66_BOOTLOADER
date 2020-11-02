#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f0xx.h"

#define SPI_FLASH_SPI                           SPI2
#define SPI_FLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_13                  /* PA.05 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_AHBPeriph_GPIOB
#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_14                  /* PA.06 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_15                  /* PA.07 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define SPI_FLASH_CS_PIN                        GPIO_Pin_4                  /* PC.04 */
#define SPI_FLASH_CS_GPIO_PORT                  GPIOA                       /* GPIOC */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_AHBPeriph_GPIOA


#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define SPI_FLASH_VCC_ON()       GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define SPI_FLASH_VCC_OFF()      GPIO_ResetBits(GPIOA, GPIO_Pin_11)

//#define  FLASH_BinFileBase     0x700000  //放置升级程序区域
//#define  FLASH_UpdataFlag      0x7FFFF0  //升级程序标志位

void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
void W25Q64_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);


u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */

