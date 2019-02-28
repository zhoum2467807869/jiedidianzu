#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H


#include "main.h"




typedef enum 
{
	BSP_ON     = 0x00,
  BSP_OFF  = 0x01,
	BSP_TOGGLE
} bsp_led_s;
 


void BSP_BC28_Reset(bsp_led_s s);
void BSP_POWER(bsp_led_s s);
void BSP_BLED(bsp_led_s s);	
void BSP_RLED(bsp_led_s s);




#endif
