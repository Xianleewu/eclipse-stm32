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

#define GSM_DATA_RECORD_MAX_LEN		(1024)
#define GSM_IMEI_LEN				(15 + 1)

#define	sim900a_usart(fmt, ... )	usart_printf(USART2, fmt, ##__VA_ARGS__)
#define sim900a_send_byte(byte)		USART_SendData(USART2, byte)

typedef struct _gsm_data_record {
	char frame_buf[GSM_DATA_RECORD_MAX_LEN];
	int frame_len;
	char finished;
} gsm_data_record;

extern gsm_data_record gsm_global_data;

bool sim900a_cmd_with_reply(const char * cmd, const char * reply1,
		const char * reply2, uint32_t waittime);
void sim900a_tcpudp_test(uint8_t mode, uint8_t* ipaddr, uint8_t* port);
int sim900a_close_net();

#endif /* SIM900A_SIM900A_H_ */
