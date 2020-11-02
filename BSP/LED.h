//=============================================================================
//文件名称：LED.h
//功能概要：LED驱动头文件
//更新时间：2013-12-31
//=============================================================================
#ifndef __LED_H
#define __LED_H

#include "stm32f0xx.h"

#define LED_OFF                       GPIOB->BSRR = 0x02
#define LED_ON                        GPIOB->BRR = 0x02 
#define LED_TURN                      GPIOB->ODR ^= 0x02

void LED_Init(void);

#endif
