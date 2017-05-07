/*
 * led.h
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */

#ifndef LED_LED_H_
#define LED_LED_H_

#include "stm32f10x.h"
#include "sys.h"

#ifdef ATK_V33
#define LED0 PAout(8)// PA8
#define LED1 PDout(2)// PD2
#else
#define LED0 PCout(13)	// PC13
#endif

void led_init(void);

#endif /* LED_LED_H_ */
