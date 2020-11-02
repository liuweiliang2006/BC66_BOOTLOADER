#include "bsp.h"

/********************************************************
//project:STM32F1  SPI2读写MB85RS256A驱动代码
//date:2015.10.12
//author:chenxuying
//annotation:适用铁电存储器mb85锡类读写代码,直接读写一个字节
**********************************************************/

/* 私有变量 ------------------------------------------------------------------*/
uint8_t Tx_Buffer_MB[] = " 感谢您选用硬石stm32开发板\n今天是个好日子";//41个
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
  *测试内存用
  */
void TestMB85RS256A(void)
{
  uint32_t FlashID = 0;
	
  HAL_GPIO_WritePin(GPIOB,EN_BOOST_Pin,GPIO_PIN_SET); //存储器可以写
  
  HAL_Delay(100);
  
  /* Get SPI Flash Type ID */
	FlashID = SPI_FRAM_ReadID();
  
  printf("FlashID is 0x%X\r\n",FlashID);
	
	/* Check the SPI Flash Type ID */
	if (FlashID != 0)  /* #define  FLASH_ID  0XEF4018 */
	{	
		printf("检测到华邦串行flash MB85RS16A !\n");
		//DisplayChar(1,1,"--Check OK--");
		HAL_Delay(500); 	 
		
		/* 将发送缓冲区的数据写到flash中 */ 	
		MB85RS256A_WRITE(FLASH_WriteAddress,Tx_Buffer_MB, BufferSize2(Tx_Buffer_MB));
//		//SPI_FLASH_BufferWrite(Tx_Buffer_W25Q64, 252, BufferSize);
		printf("写入的数据为：%s 长度为%d\n", Tx_Buffer_MB,BufferSize2(Tx_Buffer_MB));
//		//DisplayChar(1,1,"--WRITHE OK--");
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		MB85RS256A_READ(FLASH_ReadAddress,Rx_Buffer_MB, BufferSize2(Tx_Buffer_MB));
		printf("读出的数据为：%s 长度为%d\n", Rx_Buffer_MB,BufferSize2(Tx_Buffer_MB));
//		//DisplayChar(2,1,"--READ OK--");
		/* 检查写入的数据与读出的数据是否相等 */
		TransferStatus1 = Buffercmp(Tx_Buffer_MB, Rx_Buffer_MB, BufferSize2(Tx_Buffer_MB));
		HAL_Delay(500);
		if( PASSED == TransferStatus1 )
		{    
			printf("串行flash(MB85RS16A)测试成功!\r");
			//DisplayChar(2,1,"--Test OK--");
		}
		else
		{        
			printf("串行flash(MB85RS16A)测试失败!\r");
			//DisplayChar(2,1,"--Test Err--");
		}
	}
	else
	{    
		printf("获取不到 MB85RS16A ID!\n");
		//DisplayChar(1,1,"--Check Err--");
	}
}

//读器件的型号ID
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
  * 函数功能: 写入一个字节到特定地址空间
  * 输入参数: data：数据
  *           address：地址
  * 返 回 值: 无
  */
void SPI_write_MB85RS256A(uint8_t data,uint16_t address)
{
	uint8_t addr_tempH,addr_tempL;
	addr_tempH = (uint8_t)((address&0xff00)>>8);   //获取高八位地址
	addr_tempL = (uint8_t)(address&0x00ff);        //获取低八位地址
	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_WriteEnable);      //写使能
 	MB_SPI_CS_DISABLE;
// 	delay(1000);
 	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_WRITE_INST);     //写存储器数据 
	SPI_FLASH_SendByte(addr_tempH);                //写入高八位地址,高三位忽略
	SPI_FLASH_SendByte(addr_tempL);                //
	SPI_FLASH_SendByte(data);                      //写入数据
	MB_SPI_CS_DISABLE;
// 	MB_SPI_CS_ENABLE;
// 	SPIx_ReadWriteByte(MB85RS256A_WriteDisable);
// 	MB_SPI_CS_DISABLE;
}

/**
  * 函数功能: 独处特定地址空间的数据
  * 输入参数: address：地址
  * 返 回 值: data：数据 
  */
uint8_t SPI_read_MB85RS256A(uint16_t address)
{
	uint8_t dat,addr_tempH,addr_tempL;
	addr_tempH = (uint8_t)((address&0xff00)>>8);
	addr_tempL = (uint8_t)(address&0x00ff);
	MB_SPI_CS_ENABLE;
	SPI_FLASH_SendByte(MB85RS256A_READ_INST);      //读操作码
	SPI_FLASH_SendByte(addr_tempH);
	SPI_FLASH_SendByte(addr_tempL);
	dat=SPI_FLASH_SendByte(0x00);                   //读取数据,0xAA给与读取数据所需的时钟
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

实时数据写入Fram

*******************/
void Real_data_Write(void)
{
	
	MB85RS256A_WRITE(0,(uint8_t*)(&Parameter),1764);//Fram 最大内存为1764字节 超过最大地址会导致地址寻址错误死机
	
}
/*****************

实时数据读取

*******************/
void Real_data_Read(void)
{
	
	MB85RS256A_READ(0,(uint8_t*)(&Parameter),1500);
	
}
