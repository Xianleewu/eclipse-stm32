/*
 * delay.h
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */

#ifndef DELAY_DELAY_H_
#define DELAY_DELAY_H_

#include "stm32f10x.h"

void delay_init();
void delay_us(u32 nus);
void delay_ms(u16 nms);

#endif /* DELAY_DELAY_H_ */
