#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "main.h"

#include "usart.h"
typedef struct
{
	uint8_t max_len;
	uint8_t RX_buf[50];
	uint8_t len;
	uint8_t status;
}uartst;

typedef struct
{
	uint16_t max_len;
	uint16_t len;
	uint8_t RX_buf[500];
	
	uint8_t status;
}uart3st;


extern uartst typeuart1;

extern uartst typeuart2;

extern uart3st typeuart3;





void Bsp_IDLE_Enable(void);


void BSP_IDLE_Uart1(void);

void BSP_IDLE_Uart2(void);

void BSP_IDLE_Uart3(void);

int debuge_printf(const char *__format, ...);






#endif
