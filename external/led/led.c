/*
 * led.c
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */


#include "led.h"

void led_init(void)
{
#ifdef ATK_V33
	RCC->APB2ENR|=1<<2;
	RCC->APB2ENR|=1<<5;

	GPIOA->CRH&=0XFFFFFFF0;
	GPIOA->CRH|=0X00000003;
    GPIOA->ODR|=1<<8;

	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000300;
	GPIOD->ODR|=1<<2;
#else
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
#endif
}
