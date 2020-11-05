/*
 * sim900a.c
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */

#include "stm32f0xx.h"
#include "stdio.h"
#include "stdlib.h"
#include "sim900a.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "iap.h"
#include "ATCmdAnalyse.h"

#define ENABLE_TIMEOUT 0
extern updata_t updata;
gsm_data_record gsm_global_data = { "\0", 0, 0};

static const char *modetbl[2] = { "TCP", "UDP" };

void sim900a_vcc_init()
{
	GPIO_InitTypeDef GPIO_InitStructureC;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStructureC.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructureC.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructureC);
	GPIO_ResetBits(GPIOC, GPIO_Pin_10);
	delay_ms(1000);
	GPIO_SetBits(GPIOC, GPIO_Pin_10);
}

void sim900a_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructureC;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructureC.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructureC.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructureC);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void sim900a_poweron()
{
	if(sim900a_check_status()) {
		printf("%s %d:Sim900a is already power on!\n", __func__, __LINE__);
	} else {
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		delay_ms(1200);
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
		delay_ms(1000);
		printf("%s %d:Sim900a power on now!\n", __func__, __LINE__);
	}
}

int sim900a_check_status()
{
    int try_times = 5;

    /* hardware sync */
    do {
        if(sim900a_cmd_with_reply("AT", "OK", NULL, GSM_CMD_WAIT_SHORT)) {
            return 1;
        }
        delay_ms(100);
    } while(try_times--);

    return 0;
}

bool sim900a_cmd_with_reply (const char *cmd, const char * reply1, const char * reply2, uint32_t waittime)
{
    memset(&gsm_global_data, 0, sizeof(gsm_data_record));
    gsm_global_data.frame_len = 0;

    if((uint32_t)cmd < 0xFF) {
        sim900a_send_byte((uint32_t)cmd);
    } else {
        sim900a_usart("%s\r\n", cmd );
    }

    printf("--->%s\n", cmd);

    if ((reply1 == NULL) && (reply2 == NULL))
        return true;

#if ENABLE_TIMEOUT
    bool ret = 0;
    gsm_global_data.frame_buf[GSM_DATA_RECORD_MAX_LEN - 1] = '\0';
    TIME_LOOP_MS(waittime) {
        if ((reply1 != NULL) && (reply2 != NULL)) {
            ret = (( bool ) strstr(gsm_global_data.frame_buf, reply1)
                   || ( bool ) strstr(gsm_global_data.frame_buf, reply2));
        } else if (reply1 != 0) {
            ret = (( bool ) strstr(gsm_global_data.frame_buf, reply1));
        } else {
            ret = (( bool ) strstr(gsm_global_data.frame_buf, reply2));
        }

        if(ret) {
            break;
        }
    }

    return ret;
#else
    delay_ms(waittime);
		
		printf("%s",gsm_global_data.frame_buf);
		
		if(u8UartRecDone ==1) //如果串口接收完成标志位置1
		{
			if ((reply1 != 0) && (reply2 != 0)) {
					return (( bool ) strstr(gsm_global_data.frame_buf, reply1)
									|| ( bool ) strstr(gsm_global_data.frame_buf, reply2));
			} else if (reply1 != 0) {
					return (( bool ) strstr(gsm_global_data.frame_buf, reply1));
	
			} else {
					return (( bool ) strstr(gsm_global_data.frame_buf, reply2));
	
			}			
		}else{
			printf("%s receive null!\r\n", cmd);
			return 0;
		}

#endif
}

int sim900a_close_net()
{
    int ret = 0;
    ret = sim900a_cmd_with_reply("AT+CIPCLOSE=1", "CLOSE OK", NULL, GSM_CMD_WAIT_SHORT);
    ret = sim900a_cmd_with_reply("AT+CIPSHUT", "SHUT OK", NULL, GSM_CMD_WAIT_SHORT);

    return ret;
}

int sim900a_tcpudp_connect(uint8_t mode, char* ipaddr, char* port)
{
    char net_info[256] = {0};

    snprintf(net_info, 256, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"", modetbl[mode],
             ipaddr, port);

    /* 判断ALREADY返回值能跟明确的指明连接已经建立 */
    if(!sim900a_cmd_with_reply(net_info, "ALREADY", NULL, GSM_CMD_WAIT_LONG)) {
        return -1;
    }

    return 1;
}

bool  GPRS_SetAPN(char *Name,char *username,char *password)
{
    char str[64];
    uint8_t answer;
    sprintf(str,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n","kopagas","0","0");
    answer = sim900a_cmd_with_reply(str,"\r\nOK\r\n",NULL,GSM_CMD_WAIT_SHORT);
    if(answer == 1)
    {
        printf("\r\nGPRS_SetAPN(\"%s\",\"%s\",\"%s\") ---> OK\r\n",Name,username,password);
        return true;
    }
    else
    {
        printf("\r\nGPRS_SetAPN(\"%s\",\"%s\",\"%s\") ---> ERROR\r\n",Name,username,password);
        return false;
    }
}

/* assume network status was connected */
int sim900a_net_send(char* data)
{
    if (sim900a_cmd_with_reply("AT+CIPSEND", ">", NULL, GSM_CMD_WAIT_LONG)) {
        delay_us(500);
        sim900a_usart("%s", data);
        delay_us(50);
        if (sim900a_cmd_with_reply((char*) 0X1A, "OK", NULL, 1864)) {
            printf("send success!\n");
        } else {
            printf("send failed!\n");
            return -1;
        }
    } else {
        sim900a_cmd_with_reply((char*) 0X1B, NULL, NULL, GSM_CMD_WAIT_NONE);	//ESC,取消发送
        return -1;
    }

    return 1;
}

int sim900a_http_init()
{
    int ret = -1;
		int whiletime_out = 3;
					
    sim900a_cmd_with_reply("AT+HTTPTERM", NULL, NULL, GSM_CMD_WAIT_NORMAL);

    /* init network before init http */
    ret = sim900a_cmd_with_reply("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", NULL, GSM_CMD_WAIT_LONG);
    if(ret) {
        printf("Success to set to AT+SAPBR=3,1,GPRS!\n");
    } else {
        // actualy we already init network in tcp step
        printf("Failed to set to AT+SAPBR=3,1,GPRS!\n");
    }

    ret = sim900a_cmd_with_reply("AT+SAPBR=3,1,\"APN\",\"CMNET\"", "OK", NULL, GSM_CMD_WAIT_LONG);
    if(ret) {
        printf("Success to set to AT+SAPBR=3,1,APN!\n");
    } else {
        printf("Failed to set to AT+SAPBR=3,1,APN!\n");
    }

    ret = sim900a_cmd_with_reply("AT+SAPBR=1,1", "OK", NULL, GSM_CMD_WAIT_LONG);
    if(ret) {
        printf("Success to set to AT+SAPBR=1,1!\n");
    } else {
        printf("Failed to set to AT+SAPBR=1,1!\n");
    }

    do {
        ret = sim900a_cmd_with_reply("AT+HTTPINIT", "OK", NULL, GSM_CMD_WAIT_LONG);
        if(ret) {
            printf("Success to init to http!\n");
            break;
        } else {
            printf("Failed to init to http!\n");
        }
				whiletime_out--;
				if(whiletime_out == 0)break;
    } while(1);

    return 0;
}


/*BC66从服务器下载数据包后，并解析得出数据大小，函数返回数抱包大小*/
uint16_t u16FrameOverTime = 0;

int sim900a_http_get(char* url)
{
    char url_at[128] = {0};
    int ret = -1;
		int ret0 = -1;
		int ret1 = -1;
    int i = 0;
    int ret_start = 0;
		int whiletime_out = 100;
		char cmd_str[16] = {0};
		int result;
		char str[64];
		uint32_t u32FileSize = 0;
		char *position;
    sprintf(url_at,"AT+QFOTADL=\"http://%s\",1",url);

		result = NOTFOUND;
		whiletime_out = 10;
		do{
			result = sim900a_cmd_with_reply(url_at, "HTTPSTART", NULL, 5000);//是否附着网络
			delay_ms(300);
			if(result == FOUND)
			{
				u16FrameOverTime = 0;
				TIM_Cmd(TIM6, ENABLE);		
			}			
			whiletime_out--;
			if(whiletime_out == 0)break;
		}while(!result);			
    // waiting for action cmd return
    printf("\nDowloading: BC66 from server\r\n");

		while(u16FrameOverTime < 50) //50秒内有新数据
		{
			if (u8UartRecDone ==1)
			{
				u16FrameOverTime=0;
				u8UartRecDone =0;
				gsm_global_data.frame_len=0;
				printf("%s\r\n",gsm_global_data.frame_buf);
				position=strstr(gsm_global_data.frame_buf,"FILESIZE");
				if(position!= NULL)
				{
//					printf("%s\r\n",position);
					TIM_Cmd(TIM6, DISABLE);
					sscanf(position, "%[^,],%d", str,&u32FileSize);					
					break;
				}
				memset(gsm_global_data.frame_buf,0,GSM_DATA_RECORD_MAX_LEN);
			}			
		}
		updata.FILESIZE = u32FileSize;
		printf("File size %d\r\n",u32FileSize);
    return u32FileSize;
}

int sim900a_http_read(int start, int size, char** data)
{
    int ret = -1;
    int i = 0;
    int read_len = 0;
    int ret_start = 0;
    char read_range[48] = {0};
    char cmd_str[16] = {0};

    snprintf(read_range, 48, "AT+HTTPREAD=%d,%d", start, size);

    do {
        gsm_global_data.frame_len = 0;
        if(size == 0) {
            ret = sim900a_cmd_with_reply("AT+HTTPREAD", "+HTTPREAD", NULL, GSM_CMD_WAIT_LONG);
        } else {
            ret = sim900a_cmd_with_reply(read_range, "+HTTPREAD", NULL, size ? (size/10):2000);
        }

        if(ret) {
            printf("Success to read data from server!\n");
            break;
        } else {
            printf("Failed to read data, retry...!\n");
            delay_ms(1000);
        }
    } while(1);

    gsm_global_data.frame_buf[GSM_DATA_RECORD_MAX_LEN -1] = '\0';

    // 获取读取到的数据长度信息
    sscanf(gsm_global_data.frame_buf+ret_start, "%[^:]:%d", cmd_str, &read_len);
    // 长度信息所占长度
    i = snprintf(cmd_str, 16, "%d", read_len);

    // +HTTPREAD: xxxx
    *data = gsm_global_data.frame_buf+ret_start + 1 + 9 + 1 + i + 1;
#ifdef GSM_DEVICE_SIM800
    *data = gsm_global_data.frame_buf+ret_start + 1 + 9 + 1 + i + 1 + 1;
#endif

    printf("Read http data pos:%p,len:%d\n", *data, read_len);

    return read_len;
}

int M26_http_readAdd()
{
    int ret = -1;
    int i = 0;
    int read_len = 0;
    char read_range[48] = {0};
    char cmd_str[16] = {0};

    sim900a_cmd_with_reply("AT+QFOPEN=\"RAM:app1.bin\",0", "+QFOPEN", NULL, GSM_CMD_WAIT_LONG);

    gsm_global_data.frame_buf[GSM_DATA_RECORD_MAX_LEN -1] = '\0';

    // 获取读取到的数据长度信息
    sscanf(gsm_global_data.frame_buf, "%[^:]:%d", cmd_str, &read_len);
    // 长度信息所占长度
//    i = snprintf(cmd_str, 16, "%d", read_len);

    // +HTTPREAD: xxxx
//    *data = gsm_global_data.frame_buf+ret_start + 1 + 9 + 1 + i + 1;
//#ifdef GSM_DEVICE_SIM800
//    *data = gsm_global_data.frame_buf+ret_start + 1 + 9 + 1 + i + 1 + 1;
//#endif
    printf("Read http data str:%s\r\n", cmd_str);
    printf("Read http data pos:%d\r\n", read_len);

    return read_len;
}

char BC66_Data_Buffer[GSM_DATA_RECORD_MAX_LEN];
int M26_http_read(int start, int size, char** data)
{
    int ret = -1;
    int i = 0;
    int read_len = 0;
		char *ptFindPos=NULL;
    int ret_start = 0;
    char read_range[48] = {0};
    char cmd_str[16] = {0};
		
		u8UartRecDone = 0;
		u16FrameOverTime=0;
		//AT+QFLASHR=0,1024,1
//    snprintf(read_range, 48, "AT+QFLASHR=%d,%d,1", start, size);
		printf("updating...\r\n");
		sprintf(read_range,"AT+QFLASHR=%d,%d,1\r\n",start, size);//从BC66读取指令
    gsm_global_data.frame_len = 0;
		memset(gsm_global_data.frame_buf,0,GSM_DATA_RECORD_MAX_LEN); //发送指令前，清接收BUFFER
		USART_Cmd(USART2, ENABLE);//使能串口
//    ret = sim900a_cmd_with_reply(read_range, "0K", NULL, GSM_CMD_WAIT_LONG);
//		printf("%s\r\n", read_range);
		sim900a_usart("%s\r\n", read_range );
		TIM_Cmd(TIM6, ENABLE); //开启定时器，发完指令5秒内需有数据返回
		while(u16FrameOverTime < 5) //5秒内有新数据
		{
			if(u8UartRecDone ==1)
			{
				//串口接收到数据，关闭超时处理；失能串口2，防止处理期间，被打乱
				TIM_Cmd(TIM6, DISABLE);
				USART_Cmd(USART2, DISABLE);
				u8UartRecDone = 0;
				u16FrameOverTime = 0;  
				break;
			}
		}
		if(u16FrameOverTime == 0)
		{
			ptFindPos =  strstr(gsm_global_data.frame_buf,"\r\n>\r\n");
			ptFindPos = ptFindPos+strlen("\r\n>\r\n"); //指针指向数据的起始位置
			
			memcpy(BC66_Data_Buffer,ptFindPos,size);
			*data = BC66_Data_Buffer; //将数据地址返回给二级指针
		}
//		for(i=0;i<size;i++)
//		{
//			printf("%02X ",*(*data+i));
////			if(i%16 == 0)
////			{
////				printf("\r\n");
////			}
//		}
    return size;
}

