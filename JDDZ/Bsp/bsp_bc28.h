#ifndef __BSP_BC28_H
#define __BSP_BC28_H

#include "main.h"


typedef enum
{
	INIT_Succes = 0,
	AT_Test = 1,
	AT_CFUN,
	AT_CIMI,
	AT_CSQ,
	AT_CGATT,
	AT_CGPADDR,
	TCP_Succes,
	SOCKET_Fail,
	Connect_Fail
} NBiot_status;



typedef struct
{
	uint8_t imsi[18];
	uint8_t CSQ;
	uint8_t vcc[10];
	uint8_t temp[10];
	uint8_t time[10];
}NBiot_pts;

typedef struct
{
	uint8_t Socketnum;   //sendsocket编号
	uint8_t RSocketnum;   //ressocket编号
	uint8_t reclen[10];   //获取到数据的长度
	uint8_t res;
	uint8_t recdatalen[10];
	uint8_t recdata[500];
	uint8_t len;
} NBiot_dts;


extern char nbiot_buf[500];
extern NBiot_pts NBiot_parameter;
extern NBiot_dts NBiot_data;


static uint16_t chartobuff(char *buf1, char *buf2);
static uint8_t char_to_hex(uint8_t ch);

uint8_t BC28_RECData(void);
void DEBUG_Log(char *str, uint16_t line, char *buf, uint8_t len);
NBiot_status BC28_ConTCP(void);
NBiot_status BC28_Init(void);
void BC28_TCPSenddata(char *data);
uint8_t str_hex(uint8_t *str, uint8_t *hex);
void NBiot_PSM(void);

#endif

