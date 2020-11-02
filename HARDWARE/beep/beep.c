/*
 * beep.c
 *
 *  Created on: 2017年5月16日
 *      Author: xianlee
 */

#include "stm32f0xx.h"
#include "delay.h"
#include "sys.h"
#include "beep.h"

void beep_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void beep_ms(unsigned int nms)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	delay_ms(nms);
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

void beep_start()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

void beep_stop()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}
