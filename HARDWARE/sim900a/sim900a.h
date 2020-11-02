/*
 * sim900a.h
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */

#ifndef SIM900A_SIM900A_H_
#define SIM900A_SIM900A_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdarg.h"
#include "stdbool.h"
#include "usart.h"

#define GSM_DEVICE_SIM800								/* 使用的sim800系列开启此宏 */
#define GSM_DATA_RECORD_MAX_LEN		(2048)			/* GSM模块接收缓冲区长度 	*/
#define GSM_IMEI_LEN				(15 + 1)			/* IMEI长度 				*/
#define GSM_CMD_WAIT_NONE			(0)					/* 发送指令不等待结果 		*/
#define GSM_CMD_WAIT_SHORT			(100)				/* 等待100ms 				*/
#define GSM_CMD_WAIT_NORMAL			(500)				/* 等待500ms 				*/
#define GSM_CMD_WAIT_LONG			(2000)				/* 等待2s 					*/
#define GSM_UART					(USART2)			/* GSM模块连接在串口3上     */

#define	sim900a_usart(fmt, ... )	usart_printf(GSM_UART, fmt, ##__VA_ARGS__)
#define sim900a_send_byte(byte)		USART_SendData(GSM_UART, byte)

typedef struct _gsm_data_record {
	char frame_buf[GSM_DATA_RECORD_MAX_LEN];
	int frame_len;
	char finished;
} gsm_data_record;

extern gsm_data_record gsm_global_data;
/* 发送命令并等待返回 */
bool sim900a_cmd_with_reply(const char * cmd, const char * reply1,
		const char * reply2, uint32_t waittime);
/* 进行网络连接 */
int sim900a_tcpudp_connect(uint8_t mode, char* ipaddr, char* port);
/* 关闭网络连接 */
int sim900a_close_net(void);
/* IO口初始化 */
void sim900a_vcc_init(void);
void sim900a_gpio_init(void);
/* 检查模块是否已经开启 */
int sim900a_check_status(void);
/* 模块开机 */
void sim900a_poweron(void);
/* 模块关机 */
void sim900a_poweroff(void);
/* 网络发送数据 */
int sim900a_net_send(char* data);
/* HTTP 初始化 */
int sim900a_http_init(void);
/* 执行http get请求 */
int sim900a_http_get(char* url);
/* 读取HTTP返回数据 */
int sim900a_http_read(int start, int size, char** data);

/* 读取HTTP返回数据 */
int M26_http_read(int start, int size, char** data);

/* 读取HTTP返回数据在模块中地址 */
int M26_http_readAdd();

#endif /* SIM900A_SIM900A_H_ */
