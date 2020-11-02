#include "bsp.h"

/********************************************************
//project:STM32F1  SPI2��дMB85RS256A��������
//date:2015.10.12
//author:chenxuying
//annotation:��������洢��mb85�����д����,ֱ�Ӷ�дһ���ֽ�
**********************************************************/

/* ˽�б��� ------------------------------------------------------------------*/
uint8_t Tx_Buffer_MB[] = " ��л��ѡ��Ӳʯstm32������\n�����Ǹ�������";//41��
uint8_t Rx_Buffer_MB[BufferSize2(Tx_Buffer_MB)];
extern TestStatus TransferStatus1;

extern REAL_DATA_PARAM  Parameter;


//void SPI_Fram_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	 GPIO_InitStruct.Pin = GPIO_PIN_12;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	MB_SPI_CS_DISABLE; 
//	
//}  
/**
  *�����ڴ���
  */
void TestMB85RS256A(void)
{
  uint32_t FlashID = 0;
	
  HAL_GPIO_WritePin(GPIOB,EN_BOOST_Pin,GPIO_PIN_SET); //�洢������д
  
  HAL_Delay(100);
  
  /* Get SPI Flash Type ID */
	FlashID = SPI_FRAM_ReadID();
  
  printf("FlashID is 0x%X\r\n",FlashID);
	
	/* Check the SPI Flash Type ID */
	if (FlashID != 0)  /* #define  FLASH_ID  0XEF4018 */
	{	
		printf("��⵽�����flash MB85RS16A !\n");
		//DisplayChar(1,1,"--Check OK--");
		HAL_Delay(500); 	 
		
		/* �����ͻ�����������д��flash�� */ 	
		MB85RS256A_WRITE(FLASH_WriteAddress,Tx_Buffer_MB, BufferSize2(Tx_Buffer_MB));
//		//SPI_FLASH_BufferWrite(Tx_Buffer_W25Q64, 252, BufferSize);
		printf("д�������Ϊ��%s ����Ϊ%d\n", Tx_Buffer_MB,BufferSize2(Tx_Buffer_MB));
//		//DisplayChar(1,1,"--WRITHE OK--");
		/* ���ո�д������ݶ������ŵ����ջ������� */
		MB85RS256A_READ(FLASH_ReadAddress,Rx_Buffer_MB, BufferSize2(Tx_Buffer_MB));
		printf("����������Ϊ��%s ����Ϊ%d\n", Rx_Buffer_MB,BufferSize2(Tx_Buffer_MB));
//		//DisplayChar(2,1,"--READ OK--");
		/* ���д�������������������Ƿ���� */
		TransferStatus1 = Buffercmp(Tx_Buffer_MB, Rx_Buffer_MB, BufferSize2(Tx_Buffer_MB));
		HAL_Delay(500);
		if( PASSED == TransferStatus1 )
		{    
			printf("����flash(MB85RS16A)���Գɹ�!\r");
			//DisplayChar(2,1,"--Test OK--");
		}
		else
		{        
			printf("����flash(MB85RS16A)����ʧ��!\r");
			//DisplayChar(2,1,"--Test Err--");
		}
	}
	else
	{    
		printf("��ȡ���� MB85RS16A ID!\n");
		//DisplayChar(1,1,"--Check Err--");
	}
}

//���������ͺ�ID
uint32_t SPI_FRAM_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0,Temp3 = 0;
	
  MB_SPI_CS_ENABLE;

  SPI_FLASH_SendByte(MB85RS256A_RDID_INST);

  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
	
	Temp3 = SPI_FLASH_SendByte(Dummy_Byte);

  MB_SPI_CS_DISABLE; 
  
  Temp = (Temp0 << 24) | (Temp1 << 16) | (Temp2 << 8)|Temp3;

  return Temp;
}

/**
  * ��������: д��һ���ֽڵ��ض���ַ�ռ�
  * �������: data������
  *           address����ַ
  * �� �� ֵ: ��
  */
void SPI_write_MB85RS256A(uint8_t data,uint16_t address)
{
	uint8_t addr_tempH,addr_tempL;
	addr_tempH = (uint8_t)((address&0xff00)>>8);   //��ȡ�߰�λ��ַ
	addr_tempL = (uint8_t)(address&0x00ff);        //��ȡ�Ͱ�λ��ַ
	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_WriteEnable);      //дʹ��
 	MB_SPI_CS_DISABLE;
// 	delay(1000);
 	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_WRITE_INST);     //д�洢������ 
	SPI_FLASH_SendByte(addr_tempH);                //д��߰�λ��ַ,����λ����
	SPI_FLASH_SendByte(addr_tempL);                //
	SPI_FLASH_SendByte(data);                      //д������
	MB_SPI_CS_DISABLE;
// 	MB_SPI_CS_ENABLE;
// 	SPIx_ReadWriteByte(MB85RS256A_WriteDisable);
// 	MB_SPI_CS_DISABLE;
}

/**
  * ��������: �����ض���ַ�ռ������
  * �������: address����ַ
  * �� �� ֵ: data������ 
  */
uint8_t SPI_read_MB85RS256A(uint16_t address)
{
	uint8_t dat,addr_tempH,addr_tempL;
	addr_tempH = (uint8_t)((address&0xff00)>>8);
	addr_tempL = (uint8_t)(address&0x00ff);
	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_READ_INST);      //��������
	SPI_FLASH_SendByte(addr_tempH);
	SPI_FLASH_SendByte(addr_tempL);
	dat=SPI_FLASH_SendByte(0x00);                   //��ȡ����,0xAA�����ȡ���������ʱ��
	MB_SPI_CS_DISABLE;
	return (dat);
}

void MB85RS256A_WRITE(uint16_t add, uint8_t *buf, uint16_t len)
{
	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_WriteEnable);                   /* step 1 .  WEL = 1            */
 	MB_SPI_CS_DISABLE;
// 	delay(1000);
 	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_WRITE_INST);
  SPI_FLASH_SendByte((uint8_t)(add>>8));
  SPI_FLASH_SendByte((uint8_t)(add));
  for (; len > 0; len--)
	{                                            				/* step 4 . send out bytes      */
     SPI_FLASH_SendByte(*buf++);
	}
	MB_SPI_CS_DISABLE;
}

void MB85RS256A_READ(uint16_t add, uint8_t *buf, uint16_t len)
{	
	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_READ_INST);
	SPI_FLASH_SendByte((uint8_t)(add>>8));
	SPI_FLASH_SendByte((uint8_t)(add));
	for (;len > 0; len--) 
	{
       *buf++ = (SPI_FLASH_SendByte(0x00));
	}
	MB_SPI_CS_DISABLE;
}

/*****************

ʵʱ����д��Fram

*******************/
void Real_data_Write(void)
{
	
	MB85RS256A_WRITE(0,(uint8_t*)(&Parameter),1764);//Fram ����ڴ�Ϊ1764�ֽ� ��������ַ�ᵼ�µ�ַѰַ��������
	
}
/*****************

ʵʱ���ݶ�ȡ

*******************/
void Real_data_Read(void)
{
	
	MB85RS256A_READ(0,(uint8_t*)(&Parameter),1500);
	
}
