#include "bsp_jddz.h"

#include "bsp_uart.h"
#include "string.h"
#include "crc16.h"
/*00 03 00 01 00 02 94 1A
	  1���豸��ַ��1~255  0Ϊ�㲥��ַ��
		2�����ܴ��룺03 ��ȡ��06 ���ã�
		2����ַ0��01Ϊ��ȡ����ֵ��
					02Ϊ�����豸��ַ��
					03 Ϊ���ñ���ֵ��
		3������ �����ݸ�ʽΪʮ�����ƣ�
							������ֵ��Ϊ02�����ȡ2�ֽ����ݣ�
							�����豸��ַʱ��Ϊ��ַ���ݣ���8�ŵ�ַ��Ϊ00 08��
							���ñ���ֵʱΪ�����������ݣ��ֱ���Ϊ1ŷķ��
							��23ŷķʱΪ00 17��  


8�Ŵӻ���Ӧ��
08 03 02 0A D2 E2 B8        0AD2 - 2770  
����ֵΪ�� 27.7ŷķ
*/

uint8_t send_jddz_hex[8] = {0x00,0x03,0x00,0x01,0x00,0x02,0x94,0x1A};
uint16_t jddz_value;
uint8_t get_jddz_value(void)
{
	uint8_t state = 0;
	uint16_t crc1 = 0,crc2 = 0;
	HAL_UART_Transmit(&huart1,send_jddz_hex,8,15);
	HAL_Delay(50);
	jddz_value = 0;
	if(typeuart1.status == 1)
	{
		crc1 = usMBCRC16(typeuart1.RX_buf,5);
		crc2 = typeuart1.RX_buf[6]<<8|typeuart1.RX_buf[5];
		if(crc1 == crc2)
		{
			jddz_value = typeuart1.RX_buf[3]<<8|typeuart1.RX_buf[4];
//			debuge_printf("UART1:%x,%x,%x,%x,%x,%x,%x\n",typeuart1.RX_buf[0],typeuart1.RX_buf[1],typeuart1.RX_buf[2],typeuart1.RX_buf[3],typeuart1.RX_buf[4],typeuart1.RX_buf[5],typeuart1.RX_buf[6]);
			state = 1;
		
//			debuge_printf("FILE:%s,LINE:%d: jddz = %d\n",__FILE__,__LINE__,jddz_value);
		}
		typeuart1.status = 0;
		
		memset(typeuart1.RX_buf,0,sizeof(typeuart1.RX_buf));
	}	
	
	return state;
}
















