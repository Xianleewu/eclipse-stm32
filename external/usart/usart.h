/*
 * usart.h
 *
 *  Created on: 2017年5月6日
 *      Author: xianlee
 */

#ifndef USART_USART_H_
#define USART_USART_H_

#define RX_BUF_MAX_LEN     2048

#define	PC_Usart( fmt, ... )	usart_printf( USART1, fmt, ##__VA_ARGS__ )

void usart1_init(u32 bound);
void usart2_init(u32 bound);
void usart_printf(USART_TypeDef* USARTx, char *Data, ...);

#endif /* USART_USART_H_ */
