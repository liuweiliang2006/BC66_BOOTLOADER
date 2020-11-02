/*
 * led.h
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */

#ifndef LED_LED_H_
#define LED_LED_H_

#include "stm32f0xx.h"
#include "sys.h"

#define LED0 PDout(2)	// PC0
//#define LED1 PCout(1)	// PC1

void led_init(void);

#endif /* LED_LED_H_ */
