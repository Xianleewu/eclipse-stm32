/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "unistd.h"
#include "usart.h"
#include "cJSON.h"
#include "delay.h"
#include "led.h"
#include "sim900a.h"

int main(void) {
	cJSON *json_root = NULL;
	char *json_str = NULL;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	delay_init();

	usart1_init(115200);
	usart2_init(9600);

	led_init();

	json_root = cJSON_CreateObject();

	cJSON_AddStringToObject(json_root, "imei", "345456547");
	cJSON_AddStringToObject(json_root, "rfid", "987979797");
	cJSON_AddStringToObject(json_root, "time", "889798990");
	cJSON_AddStringToObject(json_root, "led4", "red");
	cJSON_AddStringToObject(json_root, "led5", "blue");
	cJSON_AddStringToObject(json_root, "led6", "red");
	cJSON_AddStringToObject(json_root, "led7", "blue");
	cJSON_AddStringToObject(json_root, "led8", "red");
	cJSON_AddStringToObject(json_root, "led9", "blue");
	cJSON_AddStringToObject(json_root, "led10", "red");
	cJSON_AddStringToObject(json_root, "led11", "blue");

	json_str = cJSON_Print(json_root);
	if (json_str) {
		usart_printf(USART1, "\nHello eclipse from uart1:\n%s", json_str);
		usart_printf(USART2, "\nHello eclipse from uart2:\n%s", json_str);
		free(json_str);
		json_str = NULL;
	}

	while (1) {
		while(!sim900a_cmd_with_reply("AT", "OK", NULL, 1000)) {
			sim900a_close_net();
			PC_Usart("sim900a connecting...\n");
		}

		PC_Usart("sim900a test:%d\n", sim900a_cmd_with_reply("AT+CIPGSMLOC=2,1", "OK", NULL, 1000));
		PC_Usart("sim900a test:%d\n", sim900a_cmd_with_reply("AT+GSN", "OK", NULL, 1000));
		LED0 = !LED0;
		delay_ms(1000);
		LED0 = !LED0;
		sim900a_tcpudp_test(0, "123.184.19.134", "43135");
	}
}
