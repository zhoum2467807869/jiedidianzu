#include "bsp_task.h"
#include "cJSON.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

__IO uint16_t WakeTime = 10;


__IO uint8_t ip_adress_bak[25] = "0,221.226.26.94,45678";
__IO uint8_t ip_adress[23] = "103.46.128.41,45916";

static char ID_Buf[8] = "000000";

void Device_Init(void)
{
	//初始化唤醒时间
	uint32_t tmp[2];
	FlASH_READ(DEVICE_WK_ADDR,tmp,2);
	if(tmp[0] == 0x88)
	{
		if(tmp[1] <65535 && tmp[1]>0)
		WakeTime = tmp[1];
	}
	//初始化id号
	char stmp;
	FlASH_READ_CHAR(DEVICE_ID_ADDR,(uint8_t *)&stmp,1);
	if(isdigit(stmp))
	{
		FlASH_READ_CHAR(DEVICE_ID_ADDR,(uint8_t *)ID_Buf,6);
	}
	//初始化ip地址
	uint8_t iptmp[25];
	FlASH_READ_CHAR(DEVICE_IP_ADDR,iptmp,25);
	stmp = 0;
	for(uint8_t i=0;i<25;i++)
	{
		if(isgraph(iptmp[i])) stmp++;
	}
	
	if(stmp > 11)
	{
		FlASH_READ_CHAR(DEVICE_IP_ADDR,(uint8_t *)ip_adress_bak,stmp);
	}	
}


uint8_t Task_Init(void)
{
	uint8_t state = 0;
	NBiot_data.res = 0;
	Device_Init();
	HAL_Delay(2000);
	if(INIT_Succes == BC28_Init())
	{
		if(TCP_Succes == BC28_ConTCP())
		{
			state = 1;
//			BC28_TCPSenddata("1234\n");
		}
	}
	
	return state;
}


void Task_JDDZ(void)
{
	uint8_t i = 5;
//	BSP_POWER(BSP_ON);
//	HAL_Delay(5000);
	while(get_jddz_value() == 0)
	{
		i--;
		if(i==0)break;
		HAL_Delay(2000);
	}
	
	char temp[10];
	sprintf(temp,"value:%d\n",jddz_value);
	BSP_POWER(BSP_OFF);
//	BC28_TCPSenddata(temp);
}

void Task_Sleep(void)
{
	NBiot_PSM();
	StandbyMode_Measure();
}


void Task_Send(void)
{
	cJSON *main = NULL;
	char *str = NULL;
	main = cJSON_CreateObject();
	if(main != NULL)
	{
		cJSON_AddStringToObject(main,"type","JDDZ");
		cJSON_AddStringToObject(main,"id",ID_Buf);
		cJSON_AddStringToObject(main,"vcc",(char *)NBiot_parameter.vcc);
		cJSON_AddStringToObject(main,"temp",(char *)NBiot_parameter.temp);
		cJSON_AddNumberToObject(main,"csq",NBiot_parameter.CSQ);
		cJSON_AddStringToObject(main,"imsi",(char *)NBiot_parameter.imsi);
																					
		cJSON_AddStringToObject(main,"ip",(char *)ip_adress_bak);
		cJSON_AddNumberToObject(main,"sensor",jddz_value);
		cJSON_AddNumberToObject(main,"waketime",WakeTime);
		
		
		
		
		str = cJSON_PrintUnformatted(main);
		if(str != NULL)
		{
			BC28_TCPSenddata(str);
			free(str);
		}
		cJSON_Delete(main);
	}
	else
	{
		debuge_printf("cjson send buf overflow\n");
	}


}
uint8_t nbiot_rec_buf[310];
void Task_Recive(void)
{
	//通过NBIOT接收数据解析
	if(NBiot_data.res > 0)
	{
		NBiot_data.res--;
		BC28_RECData();
		if(strlen((char *)NBiot_data.recdata) > 300)
		{
			debuge_printf("cjson recdata buf overflow\n");
			return;
		}
		uint8_t len = str_hex(NBiot_data.recdata, nbiot_rec_buf);
		DEBUG_Log(__FILE__,__LINE__,(char *)nbiot_rec_buf, len);
		
		cJSON *cjson = NULL;
		cJSON *item = NULL;
		cjson = cJSON_Parse((char *)nbiot_rec_buf);
		if(cjson != NULL)
		{
			if(cJSON_GetArraySize(cjson)<=4)
			{
				if(strncmp(cJSON_GetObjectItem(cjson,"id")->valuestring,ID_Buf,6)==0)
				{
					//检索关键字 “waketime”
					item = cJSON_GetObjectItem(cjson,"waketime");
					if(item != NULL)
					{
						if(item->type == cJSON_Number)
						{
							if(item->valueint >0 && item->valueint <65535)
							{
								if(WakeTime != item->valueint)
								{
									WakeTime = item->valueint;
									uint32_t tmp[2];
									tmp[0] = 0x88;
									tmp[1] = WakeTime;
									DEBUG_Log(__FILE__,__LINE__,"WakeTime", 0);
									FLASH_WRITE(DEVICE_WK_ADDR,tmp,2);
								}
							}
						}	
					}
					//检索关键字 “setid”
					item = cJSON_GetObjectItem(cjson,"setid");
					if(item != NULL)
					{
						if(item->type == cJSON_String)
						{
							if(strlen(item->valuestring)==6)
							{
								sprintf(ID_Buf,"%s",item->valuestring);
								FLASH_WRITE_CHAR(DEVICE_ID_ADDR,(uint8_t *)ID_Buf,6);
								DEBUG_Log(__FILE__,__LINE__,"setid", 0);
							}
						}	
					}
					//检索关键字 “ip”
					item = cJSON_GetObjectItem(cjson,"ip");
					if(item != NULL)
					{
						if(item->type == cJSON_String)
						{
							if(strlen(item->valuestring)<24)
							{
								sprintf((char *)ip_adress_bak,"%s",item->valuestring);
								FLASH_WRITE_CHAR(DEVICE_IP_ADDR,(uint8_t *)ip_adress_bak,25);
								DEBUG_Log(__FILE__,__LINE__,"ip", 0);
							}
						}	
					}
				}
			}
			cJSON_Delete(cjson);
		}
		else
		{
			debuge_printf("cjson Invalid data\n");
		}
	}
	//通过Uart2接收数据解析
	if(typeuart2.status == 1)
	{
		
		debuge_printf("%s",typeuart2.RX_buf);
		typeuart2.status = 0;
	}
}


