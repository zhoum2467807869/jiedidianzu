#include "bsp_uart.h"

#include "usart.h"

//uint8_t RX_buf[100];
#include "bsp_bc28.h"
#include "string.h"
#include <stdarg.h>
uartst typeuart1 = {50,0,0,0};
uartst typeuart2 = {50,0,0,0};
uart3st typeuart3 = {500,0,0,0};

void Bsp_IDLE_Enable(void)
{
	//uart1 DMA 使能
	if(__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE) == HAL_OK)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_Receive_DMA(&huart1, typeuart1.RX_buf, typeuart1.max_len);
	}
	//uart2 DMA 使能
	if(__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE) == HAL_OK)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_Receive_DMA(&huart2, typeuart2.RX_buf, typeuart2.max_len);
	}
	//uart3 DMA 使能
	if(__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE) == HAL_OK)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);
		HAL_UART_Receive_DMA(&huart3, typeuart3.RX_buf, typeuart3.max_len);
	}
}



void BSP_IDLE_Uart1(void)
{
	uint32_t temp = 0;
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) == SET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);
		temp = READ_REG(huart1.Instance->SR);
		temp = READ_REG(huart1.Instance->DR);
		temp = READ_REG(hdma_usart1_rx.Instance->CNDTR);
		typeuart1.len = typeuart1.max_len - temp;
		typeuart1.RX_buf[typeuart1.len] = '\0';
		typeuart1.status = 1;
	}
}


void BSP_IDLE_Uart2(void)
{
	uint32_t temp = 0;
	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) == SET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);
		temp = READ_REG(huart2.Instance->SR);
		temp = READ_REG(huart2.Instance->DR);
		temp = READ_REG(hdma_usart2_rx.Instance->CNDTR);
		typeuart2.len = typeuart2.max_len - temp;
		typeuart2.RX_buf[typeuart2.len] = '\0';
		typeuart2.status = 1;
	}
}

void BSP_IDLE_Uart3(void)
{
	uint32_t temp = 0;
	if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) == SET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);
		HAL_UART_DMAStop(&huart3);
		temp = READ_REG(huart3.Instance->SR);
		temp = READ_REG(huart3.Instance->DR);
		temp = READ_REG(hdma_usart3_rx.Instance->CNDTR);
		typeuart3.len = typeuart3.max_len - temp;
		typeuart3.RX_buf[typeuart3.len] = '\0';
		uint16_t sum_size = 0;
		sum_size = strlen(nbiot_buf) + typeuart3.len;
		if(sum_size < 500) 
			strncat(nbiot_buf,(char *)typeuart3.RX_buf,typeuart3.len);
		else
			sprintf(nbiot_buf,"%s",(char *)typeuart3.RX_buf);
		
		if(strstr((const char*)nbiot_buf, (const char*)"+NSONMI:"))
		{
				NBiot_data.res++;
		}
		typeuart3.status = 1;
	}
}


static char sUART2[50];

int debuge_printf(const char *__format, ...)
{
    va_list args;
    int res;
    va_start(args,__format);
    res = vsprintf(sUART2,__format,args);
		HAL_UART_Transmit(&huart2,(uint8_t *)sUART2,res,res*2);
    va_end(args);
    return res;
}



















