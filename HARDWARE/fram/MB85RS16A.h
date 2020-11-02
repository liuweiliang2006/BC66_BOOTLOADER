#ifndef __MB85RS16A_H
#define __MB85RS16A_H			    
#include "stm32f0xx.h"           

#define MB85RS16A_WriteEnable   0x06		//����дʹ��������
#define MB85RS16A_WriteDisable  0x04		//д��ֹ
#define MB85RS16A_RDSR_INST  0x05		//��״̬�Ĵ��� 
#define MB85RS16A_WRSR_INST  0x01		//д״̬�Ĵ��� 
#define MB85RS16A_READ_INST  0x03		//���洢������ 
#define MB85RS16A_WRITE_INST 0x02		//д�洢������ 
#define MB85RS16A_STATUS_REG 0x00		//
#define MB85RS16A_INIT_STATE 0x09		//
#define MB85RS16A_RDID_INST  0x9F		//��������ID = 8������ID + 8�������� + 8��ƷID + 8��ƷID

#define MB_SPI_CS_ENABLE		GPIO_ResetBits(GPIOB,GPIO_Pin_12)//HAL_GPIO_WritePin(SPI_CS2_GPIO_Port, SPI_CS2_Pin, GPIO_PIN_RESET) //�͵�ƽʹ��
#define MB_SPI_CS_DISABLE		GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define MB_SPI_CS_READ		 	GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_12)//HAL_GPIO_ReadPin(SPI_CS2_GPIO_Port,SPI_CS2_Pin)

void SPI_Fram_Init(void);
uint32_t SPI_FRAM_ReadID(void);
void SPI_write_MB85RS16A(uint8_t data,uint16_t address);
uint8_t SPI_read_MB85RS16A(uint16_t address);
void MB85RS16A_WRITE(uint16_t add, uint8_t *buf, uint16_t len);
void MB85RS16A_READ(uint16_t add, uint8_t *buf, uint16_t len);

void TestMB85RS16A(void);

#endif
