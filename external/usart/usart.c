/*
 * usart.c
 *
 *  Created on: 2017年5月6日
 *      Author: xianlee
 */
#include "stm32f10x.h"
#include "stdarg.h"
#include "led.h"
#include "sim900a.h"

void usart1_init(u32 bound) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//Usart1 NVI
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,
			ENABLE);
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void NVIC_Configuration_Uart2( void )
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void usart2_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_Configuration_Uart2();

	/* config USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Rx (PA.03) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

	USART_Cmd(USART2, ENABLE);

}

void usart_printf(USART_TypeDef* USARTx, char *Data, ...) {
	const char *s;
	int d;
	char buf[16];

	va_list ap;
	va_start(ap, Data);

	while (*Data != 0) {
		if (*Data == 0x5c) {
			switch (*++Data) {
			case 'r':
				USART_SendData(USARTx, 0x0d);
				Data++;
				break;

			case 'n':
				USART_SendData(USARTx, 0x0a);
				Data++;
				break;

			default:
				Data++;
				break;
			}
		} else if (*Data == '%') {
			switch (*++Data) {
			case 's':
				s = va_arg(ap, const char *);
				for (; *s; s++) {
					USART_SendData(USARTx, *s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
						;
				}
				Data++;
				break;

			case 'd':
				d = va_arg(ap, int);
				itoa(d, buf, 10);
				for (s = buf; *s; s++) {
					USART_SendData(USARTx, *s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
						;
				}
				Data++;
				break;
			default:
				Data++;
				break;
			}
		} else
			USART_SendData(USARTx, *Data++);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
			;
	}
}

#define USART_REC_LEN 200
uint16_t USART_RX_STA = 0;
uint8_t USART_RX_BUF[USART_REC_LEN];

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	LED0 = !LED0;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
			{
		Res = USART_ReceiveData(USART1);	//读取接收到的数据

		if ((USART_RX_STA & 0x8000) == 0)	//接收未完成
				{
			if (USART_RX_STA & 0x4000)	//接收到了0x0d
					{
				if (Res != 0x0a)
					USART_RX_STA = 0;	//接收错误,重新开始
				else
					USART_RX_STA |= 0x8000;	//接收完成了
			} else //还没收到0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA |= 0x4000;
				else {
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; //接收数据错误,重新开始接收
				}
			}
		}
	}
}

/**
  * @brief  This function handles USART2 Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void) {
	char ch;

	LED0 = !LED0;

	//读数据寄存器非空
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		//USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志.
		ch = USART_ReceiveData( USART2);
		 //预留1个字节写结束符
		if (gsm_global_data.frame_len < ( GSM_DATA_RECORD_MAX_LEN - 1)) {
			gsm_global_data.frame_buf[gsm_global_data.frame_len++] = ch;
		}
	}

    //数据帧接收完毕
	if (USART_GetITStatus( USART2, USART_IT_IDLE) == SET) {
		USART_ClearFlag(USART2, USART_FLAG_ORE);  //读SR
		USART_ReceiveData(USART2); //读DR
		gsm_global_data.finished = 1;
		//由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
		ch = USART_ReceiveData( USART2);
	}

}
