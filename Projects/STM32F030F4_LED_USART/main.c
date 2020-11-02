/**
  ****************************************************************************** 
  * IAP SPIFLASH TO STMFLASH FOR STM32F030XC
  * VERSION: V1.0
  * DATE:2019-09-10
  ******************************************************************************
	* P4_METER 
	* BJZL Technology Ltd. 
  ******************************************************************************
  */
//FLASH: 0X08000000~0x08007FFF,Loader:32K,
//FLASH：0x08008000~0x08020FFF,APP1:100K
//FLASH：0x08021000~0x08039FFF,APP1:100K
//FLASH：0x0803A000~0x0803FFFF,:24K
//
//     +-------+                +-----------------+
//	   | STM32 |			          | TCP/HTTP SERVER |
//     +---+---+                +------+----------+
//		   |        TCP SOCKET         |
//		   |-------------------------->|
//		   |                           |
//		   |  TCP <FIRMWARE URL MD5>   |
//		   |<--------------------------|
//       |                           |
//		   |  HTTP GET URL OF FIRMWARE |
//	     |-------------------------->|
//		   |                           |
//		   |  TCP <REPORT STATUS>      |
//       |-------------------------->|
//		      .		THE     END        .

#include "stdlib.h"
#include "stdint.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
#include "sim900a.h"
#include "beep.h"
#include "md5.h"
#include "spi_flash.h"
#include "motor.h"
#include "MB85RS16A.h"
#include "ATCmdAnalyse.h"


#define SERVER_IPADDR	"47.95.200.195"
#define SERVER_PORT		"6060"
#define FIRMWARE_URL	"47.95.200.195:8080/bin/userapp-mini.bin"
#define MAIN_DEBUG 1

updata_t updata;
REAL_DATA_PARAM_t REAL_DATA_PARAM;

//bool valueStatus = false;

extern MotorStatus_t gassembleStatus;

void tim7_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
//		RCC_ClocksTypeDef get_rcc_clock; 
//		RCC_GetClocksFreq(&get_rcc_clock); 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //TIM7时钟使能

    //TIM7初始化设置
    TIM_TimeBaseStructure.TIM_Period = 5000-1;//50ms 
    TIM_TimeBaseStructure.TIM_Prescaler = 480-1;
//	TIM_TimeBaseStructure.TIM_Prescaler = 1000;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

    TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );

    //TIM7中断分组配置

	  NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority=3;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update); //清除中断标志
    TIM_Cmd(TIM7, DISABLE); //停止定时器

}

void tim6_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
//		RCC_ClocksTypeDef get_rcc_clock; 
//		RCC_GetClocksFreq(&get_rcc_clock); 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //TIM7时钟使能

    //TIM7初始化设置
    TIM_TimeBaseStructure.TIM_Period = 1000-1;//50ms 65535
    TIM_TimeBaseStructure.TIM_Prescaler = 48000-1;
//	TIM_TimeBaseStructure.TIM_Prescaler = 1000;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    TIM_ITConfig( TIM6, TIM_IT_Update, ENABLE );

    //TIM7中断分组配置

	  NVIC_InitStructure.NVIC_IRQChannel=TIM6_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority=3;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update); //清除中断标志
    TIM_Cmd(TIM6, DISABLE); //停止定时器DISABLE

}
void sys_init()
{
    delay_init();
		__enable_irq();
    usart3_init(115200);
		tim7_init();
		tim6_init();
	
    led_init();
    beep_init();
	  Moto_Init();
		SPI_FLASH_Init();
	  SPI_FLASH_VCC_ON();
    usart1_init(115200);
	
	  delay_ms(1000);	
}

void read_sflash()
{
	  uint32_t FlashID = 0;
	  char i;
//    FlashID = SPI_FLASH_ReadID();
		FlashID = SPI_FRAM_ReadID();
		printf("---P4Meter Bootloader Init!---\r\n");
		if(FlashID == 0x47F0101)
	  printf("FlashID is 0x%X\r\n", FlashID);
		delay_ms(500);
//		SPI_FLASH_BufferRead((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
		MB85RS16A_READ(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
		delay_ms(500);
		printf("UPDATAFLAG:0x%X\r\n", updata.BOOTFLAG);
		printf("FILESIZE:%d\r\n", updata.FILESIZE);
		printf("MD5:");
    for(i=0;i<16;i++) printf("%02x", updata.MD5CODE[i]);
		printf("\r\nURL:%s\r\n",updata.URL_ADDR);

		REAL_DATA_PARAM_READ();
}
	
int main(void)
{
		uint32_t whiletime_out = 10;
	  char jumpAPP = 0,copy_err = 0;
	
		uint8_t i;
	  int8_t UPDATAFLAG = 0;
		uint8_t result = NOTFOUND;
	  uint8_t keyValue = 0;
		char ver[16]={0xb0,0x34,0x48,0x75,0x18,0xca,0x8a,0xf5,0x0f,0x67,0x1e,0x40,0x4b,0xc2,0x9c,0x13};
		char *address = "39.101.176.192:8080/bin/test2.bin";
////								char *md5="b0 34 48 75 18 ca 8a f5 0f 67 1e 40 4b c2 9c 13";
		char *md5=ver;	
	
//		sim900a_gpio_init();
    sys_init(); 
	
		updata.BOOTFLAG = 0xAA;//置固件更新失败,重新联网下载
		MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));  
	
	  read_sflash();
//		while(1)
//		{
//			
//		}
	  gassembleStatus = motor_null;
	
	  if(strcmp(REAL_DATA_PARAM.TankLockStatus, "0")==0)
		{
				strcpy(REAL_DATA_PARAM.TankLockStatus,"1");
				gassembleStatus = motor_close;
			  printf("1\r\n");
			  REAL_DATA_PARAM_Write();
			
			  ENBOOST_PWR(1); //6.5伏电压,给传感器,电磁阀供电 1.1mA
				
				assemble_test();
				
				//ENBOOST_PWR(0); //6.5伏电压,给传感器,电磁阀供电 1.1mA
		}
//		
//		delay_ms(1000);
//			iap_load_app(FLASH_APP1_ADDR);
		
		gassembleStatus = motor_null;
		keyValue = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
		//printf("keyValue:%d\r\n",keyValue);
	  if(keyValue == 0)
		{
			if(strcmp(REAL_DATA_PARAM.TankLockStatus, "1")==0)
      {
				strcpy(REAL_DATA_PARAM.TankLockStatus,"0");
				gassembleStatus = motor_open;
				REAL_DATA_PARAM_Write();
				printf("2\r\n");
				ENBOOST_PWR(1); //6.5伏电压,给传感器,电磁阀供电 1.1mA
				delay_ms(3000);
				assemble_test();
				
				ENBOOST_PWR(0); //6.5伏电压,给传感器,电磁阀供电 1.1mA
			}
//			else
//			{
//			  strcpy(REAL_DATA_PARAM.TankLockStatus,"1");
//				gassembleStatus = motor_close;
//				REAL_DATA_PARAM_Write();
//				printf("2\r\n");
//				ENBOOST_PWR(1); //6.5伏电压,给传感器,电磁阀供电 1.1mA
//				
//				assemble_test();
//				
//				ENBOOST_PWR(0); //6.5伏电压,给传感器,电磁阀供电 1.1mA
//			}
		}
	  
		if(updata.BOOTFLAG == 0xAA)
    {
			sim900a_vcc_init();
		  sim900a_gpio_init();
			sim900a_poweron();  //唤醒
			/* GPRS模块硬件同步 */
			
			result = NOTFOUND;
			whiletime_out = 10;
			do{
				result = sim900a_cmd_with_reply("AT", "OK", NULL, GSM_CMD_WAIT_SHORT); //AT指令
				if(result == FOUND)
				{
					//命令解析函数
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);
			
				
			result = NOTFOUND;
			whiletime_out = 10;
			do{
				result = sim900a_cmd_with_reply("AT+QSCLK=0", "OK", NULL, GSM_CMD_WAIT_SHORT); //失能BC66低功耗
				if(result == FOUND)
				{
					//命令解析函数
					
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);

			result = NOTFOUND;
			whiletime_out = 10;
			do{
				//查询信号指令 
				//Strong: RSRP ≥ -100 dbm, and RSRQ ≥ -7 dB;
				//Median: -100 dbm ≥ RSRP ≥ -110 dbm, and RSRQ ≥ -11 dB 
				//Weak: RSRP < -115 dbm, or RSRQ < -11 dBb
				result = sim900a_cmd_with_reply("AT+CESQ", "+CESQ", NULL, GSM_CMD_WAIT_SHORT); 
				if(result == FOUND)
				{
					//命令解析函数 分析信号强弱					
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);
			
			result = NOTFOUND;
			whiletime_out = 10;
			do{
				result = sim900a_cmd_with_reply("ATE0", "OK", NULL, GSM_CMD_WAIT_SHORT);   //关闭回显
				if(result == FOUND)
				{
					//命令解析函数 分析信号强弱					
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);
			
//			result = NOTFOUND;
//			whiletime_out = 10;
//			do{
//				result = sim900a_cmd_with_reply("AT+CEREG=1", "OK", NULL, GSM_CMD_WAIT_SHORT); //附着网络
//				if(result == FOUND)
//				{
//					//命令解析函数 分析信号强弱					
//				}
//				delay_ms(300);
//				whiletime_out--;
//				if(whiletime_out == 0)break;
//			}while(!result);			
			
			result = NOTFOUND;
			whiletime_out = 30;
			do{
				result = sim900a_cmd_with_reply("AT+CEREG?", "0,5", NULL, GSM_CMD_WAIT_SHORT);   //已注册 漫游状态
				if(result == NOTFOUND)
				{
					result = sim900a_cmd_with_reply("AT+CEREG?", "0,1", NULL, GSM_CMD_WAIT_SHORT);  //已注册，归属网络
				}
				if(result == FOUND)
				{
					//命令解析函数 分析信号强弱					
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);				

			result = NOTFOUND;
			whiletime_out = 10;
			do{
				result = sim900a_cmd_with_reply("AT+CGATT=1", "OK", NULL, GSM_CMD_WAIT_SHORT); //附着网络
				if(result == FOUND)
				{
					//命令解析函数 分析信号强弱					
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);

			result = NOTFOUND;
			whiletime_out = 10;
			do{
				result = sim900a_cmd_with_reply("AT+CGATT?", "+CGATT", NULL, GSM_CMD_WAIT_SHORT); //是否附着网络
				if(result == FOUND)
				{
					//命令解析函数 分析信号强弱					
				}
				delay_ms(300);
				whiletime_out--;
				if(whiletime_out == 0)break;
			}while(!result);			
			
				if(!(sim900a_cmd_with_reply("AT+CEREG?", "0,5", NULL, GSM_CMD_WAIT_SHORT) || \
							sim900a_cmd_with_reply("AT+CEREG?", "0,1", NULL, GSM_CMD_WAIT_SHORT)))
				{
					updata.BOOTFLAG = 0x22;
					MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
					printf("no connect,jump to app\r\n");
					iap_load_app(FLASH_APP2_ADDR);
//					__enable_irq();
//				  usart1_init(115200);
					printf("no connect,jump to app 2\r\n");
				}
				else
				{
//						whiletime_out = 20;
//						/* 配置当前场景 */
//						while(!sim900a_cmd_with_reply("AT+QIFGCNT=0", "OK", NULL, GSM_CMD_WAIT_SHORT)) {
//								delay_ms(300);
//								whiletime_out--;
//							
//								if(whiletime_out == 0)break; 
//						}
//						
//						whiletime_out = 20;
//						// 启动任务并设置接入点APN
//						while(!sim900a_cmd_with_reply("AT+QIREGAPP", "OK", NULL, GSM_CMD_WAIT_SHORT)) {
//								delay_ms(300);
//								whiletime_out--;
//								if(whiletime_out == 0)break; 
//						}
//						
//						whiletime_out = 20;
//						// 激活移动场景
//						while(!sim900a_cmd_with_reply("AT+QIACT", "OK", NULL, GSM_CMD_WAIT_LONG)) {
//								delay_ms(300);
//								whiletime_out--;
//								if(whiletime_out == 0)break; 
//						}
					
						while(1)
						{
//							for(i=0;i<strlen(address);i++)
//							{
//								updata.URL_ADDR[i] = *(address+i);
//							}
//								strcpy(updata.URL_ADDR,address);
								memcpy(updata.URL_ADDR,address,strlen(address));
								memcpy(updata.MD5CODE,md5,16*sizeof(char));
								UPDATAFLAG = update_from_sim900a(FLASH_APP1_ADDR, updata.URL_ADDR, updata.MD5CODE);
							  //UPDATAFLAG = update_from_sim900a(FLASH_APP1_ADDR, "47.95.200.195:8080/bin/APP1.bin", updata.MD5CODE);
								if(UPDATAFLAG == 0)
								{
										updata.BOOTFLAG = 0x22;//置固件更新成功标志位
		//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
										MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
										printf("Success to write new firmware, jump to app now!\r\n");					
		//								sim900a_net_send("Success to write new firmware, jump to app now!\n");
										iap_load_app(FLASH_APP1_ADDR);
									
										iap_load_app(FLASH_APP2_ADDR);	
										updata.BOOTFLAG = 0xAA;//置固件更新失败,重新联网下载
										jumpAPP = 1;
		//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
										MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
										printf("Jump to new firmware failed, reboot now!\r\n");
		//								sim900a_net_send("Success to write new firmware, jump to app now!\n");

										iap_load_app(FLASH_BASE);
										break;
								}
								else if(UPDATAFLAG == -1)
								{
									 updata.BOOTFLAG = 0x22;//置固件更新成功标志位
		//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
										MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
										printf("no Success to write new firmware, jump to app 1!\r\n");
		//								sim900a_net_send("Success to write new firmware, jump to app now!\n");
										iap_load_app(FLASH_APP1_ADDR);
										printf("no Success to write new firmware, jump to app 2!\r\n");
										iap_load_app(FLASH_APP2_ADDR);
									
										updata.BOOTFLAG = 0xAA;//置固件更新失败,重新联网下载
		//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
										MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
										printf("Jump to new firmware failed, reboot now!\r\n");
		//								sim900a_net_send("Success to write new firmware, jump to app now!\n");
										iap_load_app(FLASH_BASE);
										break;
								}
								else
								{
									copy_err++;
									if(copy_err>3)
									{
										#ifdef MAIN_DEBUG
										printf("---BOOTLOAD FALL---\r\n");
										#endif 
										
										iap_load_app(FLASH_APP2_ADDR);
										
										updata.BOOTFLAG = 0xAA;//置固件更新失败，原固件不可用
		//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
										MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));			
										iap_load_app(FLASH_BASE);
										break;		
									}	
								}					
						 }
			    }
			}
		  else if(updata.BOOTFLAG == 0x22) 
			{
				    printf("---BOOTLOAD 22 jump 2---\r\n");
						iap_load_app(FLASH_APP2_ADDR);
								
						updata.BOOTFLAG = 0xAA;
//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
						MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));	
				    printf("---BOOTLOAD 22 jump FLASH_BASE---\r\n");
						iap_load_app(FLASH_BASE);
			}
			else
			{
				printf("\r\n---JUMP TO APP %d---\r\n",jumpAPP);
				delay_ms(500);
				iap_load_app(FLASH_APP1_ADDR);//跳转到运行区
				printf("3\r\n");
				iap_load_app(FLASH_APP2_ADDR);
				printf("4\r\n");
				updata.BOOTFLAG = 0xAA;//置固件更新失败,重新联网下载
//								SPI_FLASH_BufferWrite((uint8_t*)&updata,BOOTFLAG_ADDR,sizeof(updata_t));
				MB85RS16A_WRITE(BOOTFLAG_ADDR,(uint8_t*)&updata,sizeof(updata_t));
				printf("Jump to new firmware failed, reboot now!\n");
//								sim900a_net_send("Success to write new firmware, jump to app now!\n");
				iap_load_app(FLASH_BASE);
				#ifdef MAIN_DEBUG
				printf("---NO PROGAM---\r\n");
				#endif 
			}	
			
			iap_load_app(FLASH_BASE);
}




