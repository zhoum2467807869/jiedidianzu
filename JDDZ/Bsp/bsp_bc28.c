#include "bsp_bc28.h"

#include "string.h"
#include "usart.h"

#define NBIOT_Delay(x) HAL_Delay(x)

char nbiot_buf[500];
char *strx = NULL, *extstrx = NULL;

NBiot_dts NBiot_data;
NBiot_pts NBiot_parameter;



uint8_t str_hex(uint8_t *str, uint8_t *hex)
{
	uint8_t len = 0;
	while(*str != '\0')
	{
		sscanf((char *)str, "%02x", (unsigned int *)hex++);
		str = str + 2;
		len++;
	}
	hex[len] = '\0';
	return len;
}








int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 2);
	return ch;
}

static void NBIOT_CLEAR_Buff(void)
{
	memset(nbiot_buf, 0, sizeof(nbiot_buf));
}





/**
  * @brief ���������Ϣ
  * @note
  * @retern null
  * @{
  */
#define Debug 0
void DEBUG_Log(char *str, uint16_t line, char *buf, uint8_t len)
{
#if Debug
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf,len,len);
	debuge_printf("FILE:%s,LINE:%d: %s\n", str, line, buf);
#else
	UNUSED(str);
	UNUSED(line);
	UNUSED(buf);
	UNUSED(len);
#endif
}



/**
  * @brief
  * @note
  * @retern null
  * @{
  */
uint8_t nbiot_send_cmd(char *cmd, char *ack, uint16_t waittime)
{
	uint8_t res = 0;
	NBIOT_CLEAR_Buff();
	printf("%s\r\n", cmd); //
	if(ack && waittime)		//
	{
		while(--waittime)	//
		{
			HAL_Delay(1);
			strx = strstr((const char*)nbiot_buf, (const char*)ack);;
			if(strx)
			{
				break;
			}
		}
		if(waittime == 0)res = 1;
	}
	return res;
}



/**
  * @brief BC28_Init
  * @note
  * @retern null
  * @{
  */
static uint8_t i = 0;
NBiot_status BC28_Init(void)
{
//		HAL_TIM_Base_Start_IT(&htim6);
//	uint8_t i = 0;
//���ģ���Ƿ������������
	i = 10;
	while(nbiot_send_cmd("AT", "OK", 400))
	{
		i--;
		if(i == 0)
		{
			return AT_Test;
		}
		DEBUG_Log(__FILE__, __LINE__, "NB-iot AT\r\n", 14);
	}

	//���ϵͳ�Ƿ��ڿ���״̬
	i = 3;
	while(nbiot_send_cmd("AT+CFUN=1", "OK", 2000))
	{
		i--;
		if(i == 0)
		{
			return AT_CFUN;
		}
		DEBUG_Log(__FILE__, __LINE__, "NB-iot CFUN\r\n", 14);
	}

	//���sim���Ƿ�Ӵ�����
	i = 3;
	while(1)
	{
		if(nbiot_send_cmd("AT+CIMI", "OK", 400) == 0)
		{
			char *str = NULL;
			str = strstr((const char*)nbiot_buf, "460");
			if(str != NULL)
			{
				strncpy((char *)NBiot_parameter.imsi, str, 15);

				break;
			}
		}
		i--;
		if(i == 0)
		{
			return AT_CIMI;
		}
		DEBUG_Log(__FILE__, __LINE__, "NB-iot CIMI\r\n", 14);
	}

	i = 3;
	while(nbiot_send_cmd("AT+CGATT=1", "OK", 400))
	{
		i--;
		if(i == 0)
		{
			break;
		}
	}
	//nbiot_send_cmd("AT+NBAND=5", "OK", 200);
	//Disable registration function of Huawei��s IoT platform
//Disable registration function of Huawei��s IoT platform
	i = 2;
	while(nbiot_send_cmd("AT+QREGSWT?", "+QREGSWT:2", 400))
	{
		i--;
		if(i == 0)
		{
			break;
		}
		DEBUG_Log(__FILE__, __LINE__, "NB-iot QREGSWT\r\n", 14);
		nbiot_send_cmd("AT+QREGSWT=2", "OK", 300);
	}
	//��ѯ��ѹ���¶�
	i = 2;
	while(1)
	{
		
		if(nbiot_send_cmd("AT+QCHIPINFO=ALL", "OK", 400)==0)
		{
			
			
			strx = strstr((const char*)nbiot_buf, (const char*)"TEMP");
			extstrx = strstr((const char*)nbiot_buf, (const char*)"VBAT");
			if(strx!=NULL && extstrx!=NULL)
			{
				strncpy((char *)NBiot_parameter.temp, strx+5, 2);
				strncpy((char *)NBiot_parameter.vcc, extstrx+5, 4);
			}
			break;	
		}
		i--;
		if(i == 0)
		{
			break;
		}
		DEBUG_Log(__FILE__, __LINE__, "NB-iot QREGSWT\r\n", 14);
		nbiot_send_cmd("AT+QREGSWT=2", "OK", 300);
	}
	
	


	//��ѯģ���ź�ǿ��

	i = 10;
	uint8_t csq_len = 0;
	NBiot_parameter.CSQ = 0;
	while((NBiot_parameter.CSQ == 99 || NBiot_parameter.CSQ == 0 ))
	{
		i--;
		if(i == 0)
		{
			break;
		}
		if(nbiot_send_cmd("AT+CSQ", "99", 400) == 0)
		{
			DEBUG_Log(__FILE__, __LINE__, "NB-iot CSQ\r\n", 14);
			strx = strstr((const char*)nbiot_buf, (const char*)"+CSQ"); //����CSQ
			extstrx = strstr((const char*)nbiot_buf, (const char*)",");
			csq_len = extstrx - strx;
			if(csq_len == 7)
			{
				NBiot_parameter.CSQ = (strx[5] - 0x30) * 10 + (strx[6] - 0x30); //��ȡCSQ
			}
			else
			{
				NBiot_parameter.CSQ = (strx[5] - 0x30);
			}
			if(NBiot_parameter.CSQ == 99) //˵��ɨ��ʧ��
			{
				DEBUG_Log(__FILE__, __LINE__, "NB-iot CSQ fail\r\n", 14);
				//return AT_CSQ;
			}
			else
			{
				debuge_printf("CSQ:%d\n", NBiot_parameter.CSQ);
			}
		}
		else
		{
			DEBUG_Log(__FILE__, __LINE__, "NB-iot CSQ\r\n", 14);

		}
		HAL_Delay(500);

	}

//��ѯ�����Ƿ��ųɹ�����Ҫ��
	i = 20;
	while(nbiot_send_cmd("AT+CGATT?", "+CGATT:1", 400))
	{
		i--;
		if(i == 0)
		{
			return AT_CGATT;
		}
		DEBUG_Log(__FILE__, __LINE__, "NB-iot CGATT?\r\n", 14);
		HAL_Delay(600);
	}

//Query the IP address of the module
	i = 3;
	while(nbiot_send_cmd("AT+CGPADDR", "+CGPADDR:0,", 400))
	{
		if(i == 0)
		{
			return AT_CGPADDR;
		}
		i--;
		DEBUG_Log(__FILE__, __LINE__, "NB-iot CGPADDR\r\n", 14);
	}

//ģ���ʼ���ɹ�
	DEBUG_Log(__FILE__, __LINE__, "NB-iot INIT_Succes\r\n", 14);
	return INIT_Succes;
}

/**
  * @brief	����TCP socket����
  * @note
  * @retern null
  * @{
  */
NBiot_status BC28_ConTCP(void)
{
	uint8_t i;
//���Թر��Ѿ����ڵ�socket����
	printf("AT+NSOCL=0\r\n");//�ر�socekt����
	HAL_Delay(300);
	printf("AT+NSOCL=1\r\n");//�ر�socekt����
	HAL_Delay(300);
	printf("AT+NSOCL=2\r\n");//�ر�socekt����
	HAL_Delay(300);
//    for(i=0;i<100;i++)
//    RxBuffer[i]=0;
	//Clear_Buffer();
//����һ��Socket
	i = 3;
	while(nbiot_send_cmd("AT+NSOCR=STREAM,6,0,1", "OK", 400))
	{
		i--;
		if(i == 0)
		{
			DEBUG_Log(__FILE__, __LINE__, "NB-iot SOCKET_Fail\r\n", 14);
			return SOCKET_Fail;
		}
	}

	NBiot_data.Socketnum = nbiot_buf[2];
	char AT_buf[40];
	if(ip_adress_bak[0] == '1')
	{
		sprintf(AT_buf, "AT+NSOCO=%c,%s", NBiot_data.Socketnum, ip_adress_bak + 2);
	}
	else
	{
		sprintf(AT_buf, "AT+NSOCO=%c,%s", NBiot_data.Socketnum, ip_adress);
	}

//���ӷ�����103.46.128.41,45916   221.226.26.94,45678
	i = 3;
	while(nbiot_send_cmd(AT_buf, "OK", 800))
	{
		i--;
		if(i == 0)
		{
			DEBUG_Log(__FILE__, __LINE__, "NB-iot Connect_Fail\r\n", 14);
			return Connect_Fail;
		}
	}

//���������ӳɹ�
	DEBUG_Log(__FILE__, __LINE__, "NB-iot TCP_Succes\r\n", 14);
	return TCP_Succes;
}

/**
* @brief	����TCP����
  * @note
  * @retern null
  * @{
  */
static char buf[500];
void BC28_TCPSenddata(char *data)
{
	uint16_t buflen;
	if(strlen(data) < 250)
	{
		memset(buf, '\0', 500);
		buflen = chartobuff((char *)data, buf);
		NBIOT_CLEAR_Buff();
		//wait_for_bc28(1);
		printf("AT+NSOSD=%c,%d,%s\r\n", NBiot_data.Socketnum, buflen, buf); //���Ͷ�Ӧ���ݳ����Լ�����,
		HAL_Delay(300);
		strx = strstr((const char*)nbiot_buf, (const char*)"OK"); //����OK
		while(strx == NULL)
		{
			HAL_Delay(300);
			strx = strstr((const char*)nbiot_buf, (const char*)"OK"); //����OK
			if(strstr((const char*)nbiot_buf, (const char*)"ERROR") != NULL)
			{
				break;
			}
		}
		NBIOT_CLEAR_Buff();
	}

}
/**
  * @brief
  * @note
  * @retern null
  * @{
  */
uint8_t BC28_RECData(void)
{
	char i;
	//static char nexti;
	strx = strstr((const char*)nbiot_buf, (const char*)"+NSONMI:"); //����+NSONMI:���������յ�UDP���������ص�����
	if(strx)
	{
		NBiot_data.RSocketnum = strx[8]; //���
		for(i = 0;; i++)
		{
			if(strx[i + 10] == 0x0D)
				break;
			NBiot_data.reclen[i] = strx[i + 10]; //����
		}
		NBiot_data.reclen[i] = '\0';
		NBIOT_CLEAR_Buff();

		HAL_Delay(300);
		printf("AT+NSORF=%c,%s\r\n", NBiot_data.RSocketnum, NBiot_data.reclen); //�����Լ����
		HAL_Delay(300);
		strx = strstr((const char*)nbiot_buf, (const char*)","); //��ȡ����һ������
		strx = strstr((const char*)(strx + 1), (const char*)","); //��ȡ���ڶ�������
		strx = strstr((const char*)(strx + 1), (const char*)","); //��ȡ������������
		NBiot_data.len = 0;
		for(i = 0;; i++)
		{
			if(strx[i + 1] == ',')
				break;
			NBiot_data.recdatalen[i] = strx[i + 1]; //��ȡ���ݳ���
			NBiot_data.len = char_to_hex(NBiot_data.recdatalen[i]) + NBiot_data.len * 10;
		}

		strx = strstr((const char*)(strx + 1), (const char*)","); //��ȡ�����ĸ�����
		for(i = 0;; i++)
		{
			if(strx[i + 1] == ',')
				break;
			NBiot_data.recdata[i] = strx[i + 1]; //��ȡ��������
		}
		NBiot_data.recdata[i] = '\0';
		NBIOT_CLEAR_Buff();
		HAL_Delay(1);
		return 1;
	}
	return 0;
}



/**
  * @brief �ַ�תhex�ַ�
  * @note
  * @retern null
  * @{
  */
static uint16_t chartobuff(char *buf1, char *buf2)
{
	uint16_t len;
	len = strlen(buf1);
	char temp[2], *temp1;
	temp1 = buf2;
	uint16_t i;
	for(i = 0; i < len; i++)
	{
		sprintf(temp, "%02x", buf1[i]);
		temp1 = strncpy(temp1, temp, 2) + 2;
	}
	return len;
}


/**
  * @brief  �ַ�ת��Ӧ��hex
  * @note
  * @retern null
  * @{
  */
static uint8_t char_to_hex(uint8_t ch)
{
	uint8_t hex;
	if(ch >= 0x30 && ch <= 0x39)
	{
		hex = ch - 0x30;
	}
	return hex;
}



void NBiot_PSM(void)
{
	uint8_t  i = 1;
	debuge_printf("FILE:%s,LINE:%d: device end\n", __FILE__, __LINE__);
	char AT_buf[11];
	sprintf(AT_buf, "AT+NSOCL=%c", NBiot_data.Socketnum);
	while(nbiot_send_cmd(AT_buf, "OK", 400))
	{
		i--;
		if(i == 0)
		{
			break;
		}
	}
	i = 1;
	while(nbiot_send_cmd("AT+CGATT=0", "OK", 400))
	{
		i--;
		if(i == 0)
		{
			break;
		}
	}
	i = 2;
	while(nbiot_send_cmd("AT+CFUN=0", "OK", 2000))
	{
		i--;
		if(i == 0)
		{
			break;
		}
	}
}


