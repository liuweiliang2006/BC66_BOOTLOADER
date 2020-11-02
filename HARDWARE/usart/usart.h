/*
 * usart.h
 *
 *  Created on: 2017年5月6日
 *      Author: xianlee
 */
#include "stdio.h"	
#include "sys.h" 

#ifndef USART_USART_H_
#define USART_USART_H_

#define AT_DEBUG						//开启GSM模块串口DEBUG
#define DEBUG_USART			USART3		//指定调试串口

extern u16 USART_RX_STA;         		//接收状态标记	
extern u16 USART_RX_CNT;				//接收的字节数	
extern uint8_t u8UartRecDone; //串口接收完成标志

void usart1_init(u32 bound);
void usart2_init(u32 bound);
void usart3_init(u32 bound);
void usart_printf(USART_TypeDef* USARTx, char *Data, ...);

#endif /* USART_USART_H_ */
