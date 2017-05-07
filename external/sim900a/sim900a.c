/*
 * sim900a.c
 *
 *  Created on: 2017年5月7日
 *      Author: xianlee
 */

#include "stm32f10x.h"
#include "sim900a.h"
#include "usart.h"
#include "delay.h"
#include "led.h"


gsm_data_record gsm_global_data = { "\0", 0, 0};

static const char *modetbl[2] = { "TCP", "UDP" };

bool sim900a_cmd_with_reply (const char * cmd, const char * reply1, const char * reply2, uint32_t waittime)
{
	gsm_global_data.frame_len = 0;               //从新开始接收新的数据包

	if((uint32_t)cmd < 0xFF) {
		sim900a_send_byte((uint32_t)cmd);
	} else {
		sim900a_usart("%s\r\n", cmd );
	}

	if ((reply1 == NULL) && (reply2 == NULL))   //不需要接收数据
		return true;

	delay_ms(waittime);                 		//延时

	gsm_global_data.frame_buf[gsm_global_data.frame_len] = '\0';

	PC_Usart("sim900a return:\n%s\n", gsm_global_data.frame_buf);

	if ((reply1 != 0) && (reply2 != 0)) {
		return (( bool ) strstr(gsm_global_data.frame_buf, reply1)
				|| ( bool ) strstr(gsm_global_data.frame_buf, reply2));
	} else if (reply1 != 0) {
		return (( bool ) strstr(gsm_global_data.frame_buf, reply1));

	} else {
		return (( bool ) strstr(gsm_global_data.frame_buf, reply2));

	}
}

int sim900a_close_net()
{
	int ret = 0;
	ret = sim900a_cmd_with_reply("AT+CIPCLOSE=1", "CLOSE OK", NULL, 500);
	ret = sim900a_cmd_with_reply("AT+CIPSHUT", "SHUT OK", NULL, 500);

	return ret;
}

/* assume network status was connected */
int sim900a_net_send(char* data)
{
	if (sim900a_cmd_with_reply("AT+CIPSEND", ">", NULL, 1000)) {
		PC_Usart("CIPSEND DATA:%s\r\n", data);
		sim900a_usart("%s", data);
		delay_ms(10);
		if (sim900a_cmd_with_reply((char*) 0X1A, "OK", NULL, 1864)) {
			PC_Usart("send success!\n");
		} else {
			PC_Usart("send failed!\n");
			return -1;
		}
	} else {
		sim900a_cmd_with_reply((char*) 0X1B, NULL, NULL, 0);	//ESC,取消发送
		PC_Usart("sim 900a send data failed!\n");
		return -1;
	}

	return 1;
}

int sim900a_net_heartbeat(char *data)
{

	return 1;
}

int sim900a_tcpudp_connect(uint8_t mode, char* ipaddr, char* port)
{
	char net_info[256] = {0};

	snprintf(net_info, 256, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"", modetbl[mode],
			 ipaddr, port);

	if(!sim900a_cmd_with_reply(net_info, "OK", "ALREADY", 500)) {
		PC_Usart("Failed to connect to server!\n");
		return -1;
	}

	PC_Usart("Connected to the server successfully!\n");

	return 1;
}

void sim900a_tcpudp_test(uint8_t mode, uint8_t* ipaddr, uint8_t* port) {
	u8 *p, *p1, *p2, *p3;
	u8 count = 0;
	u8 connectsta = 0;			//0,正在连接;1,连接成功;2,连接关闭;
	u8 hbeaterrcnt = 0;			//心跳错误计数器,连续5次心跳信号无应答,则重新连接
	u8 oldsta = 0XFF;
	p = malloc(100);			//申请100字节内存
	p1 = malloc(100);			//申请100字节内存

	sprintf((char*) p, "IP地址:%s 端口:%s", ipaddr, port);

	sprintf((char*) p, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"", modetbl[mode],
			ipaddr, port);
	if (!sim900a_cmd_with_reply(p, "OK", "ALREADY", 500)) {
		PC_Usart("failed to connect to server!\n");	 			//发送数据打印到串口
		return;		//发起连接
	}

	while (1) {

		sprintf((char*)p1,"ATK-SIM900A %s test %d  ", modetbl[mode], count);

		if (sim900a_cmd_with_reply("AT+CIPSEND", ">", NULL, 1000))	//发送数据
		{
			PC_Usart("CIPSEND DATA:%s\r\n", p1);	 			//发送数据打印到串口
			sim900a_usart("%s", p1);
			delay_ms(10);
			if (sim900a_cmd_with_reply((u8*)0X1A, "OK", NULL, 1864)) {
				//PC_Usart("send success!\n");
			} else {
				PC_Usart("send failed!\n");
			}
		} else {
			sim900a_cmd_with_reply((u8*) 0X1B, NULL, NULL, 0);	//ESC,取消发送
			PC_Usart("sim 900a send data failed!\n");
		}

		count++;

		delay_ms(1864);
		//delay_ms(1864);
		//delay_ms(1864);
		//delay_ms(1864);
//		oldsta = 0XFF;
//
//		if ((timex % 20) == 0) {
//			LED0 = !LED0;
//			count++;
//			if (connectsta == 2 || hbeaterrcnt > 8)	//连接中断了,或者连续8次心跳没有正确发送成功,则重新连接
//					{
//				sim900a_send_cmd("AT+CIPCLOSE=1", "CLOSE OK", 500);	//关闭连接
//				sim900a_send_cmd("AT+CIPSHUT", "SHUT OK", 500);		//关闭移动场景
//				sim900a_send_cmd(p, "OK", 500);						//尝试重新连接
//				connectsta = 0;
//				hbeaterrcnt = 0;
//			}
//			sprintf((char*) p1, "ATK-SIM900A %s测试 %d  ", modetbl[mode], count);
//			Show_Str(30 + 54, 100, 200, 12, p1, 12, 0);
//		}
//
//		if (connectsta == 0 && (timex % 200) == 0)//连接还没建立的时候,每2秒查询一次CIPSTATUS.
//				{
//			sim900a_send_cmd("AT+CIPSTATUS", "OK", 500);	//查询连接状态
//			if (strstr((const char*) USART2_RX_BUF, "CLOSED"))
//				connectsta = 2;
//			if (strstr((const char*) USART2_RX_BUF, "CONNECT OK"))
//				connectsta = 1;
//		}
//
//		if (connectsta == 1 && timex >= 600)	//连接正常的时候,每6秒发送一次心跳
//				{
//			timex = 0;
//			if (sim900a_send_cmd("AT+CIPSEND", ">", 200) == 0)	//发送数据
//					{
//				sim900a_send_cmd((u8*) 0X00, 0, 0);	//发送数据:0X00
//				delay_ms(20);						//必须加延时
//				sim900a_send_cmd((u8*) 0X1A, 0, 0);	//CTRL+Z,结束数据发送,启动一次传输
//			} else
//				sim900a_send_cmd((u8*) 0X1B, 0, 0);	//ESC,取消发送
//
//			hbeaterrcnt++;
//			printf("hbeaterrcnt:%d\r\n", hbeaterrcnt);	//方便调试代码
//		}
//
//		delay_ms(10);
//		if (USART2_RX_STA & 0X8000)		//接收到一次数据了
//				{
//			USART2_RX_BUF[USART2_RX_STA & 0X7FFF] = 0;	//添加结束符
//			printf("%s", USART2_RX_BUF);				//发送到串口
//			if (hbeaterrcnt)							//需要检测心跳应答
//			{
//				if (strstr((const char*) USART2_RX_BUF, "SEND OK"))
//					hbeaterrcnt = 0;							//心跳正常
//			}
//			p2 = (u8*) strstr((const char*) USART2_RX_BUF, "+IPD");
//			if (p2)							//接收到TCP/UDP数据
//			{
//				p3 = (u8*) strstr((const char*) p2, ",");
//				p2 = (u8*) strstr((const char*) p2, ":");
//				p2[0] = 0;							//加入结束符
//				sprintf((char*) p1, "收到%s字节,内容如下", p3 + 1);			//接收到的字节数
//				LCD_Fill(30 + 54, 115, 239, 130, WHITE);
//				POINT_COLOR = BRED;
//				Show_Str(30 + 54, 115, 156, 12, p1, 12, 0); //显示接收到的数据长度
//				POINT_COLOR = BLUE;
//				LCD_Fill(30, 130, 210, 319, WHITE);
//				Show_Str(30, 130, 180, 190, p2 + 1, 12, 0); //显示接收到的数据
//			}
//			USART2_RX_STA = 0;
//		}
//
//		if (oldsta != connectsta) {
//			oldsta = connectsta;
//			LCD_Fill(30 + 30, 80, 239, 80 + 12, WHITE);
//			Show_Str(30 + 30, 80, 200, 12, (u8*) cnttbl[connectsta], 12, 0); //更新状态
//		}
//		timex++;
	}

	free(p);
	free(p1);
}
