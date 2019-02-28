#include "bsp_gpio.h"





void BSP_RLED(bsp_led_s s)
{
	switch(s)
	{
		case BSP_ON:
			HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_RESET);
			break;
		case BSP_OFF:
			HAL_GPIO_WritePin(R_LED_GPIO_Port,R_LED_Pin,GPIO_PIN_SET);
			break;	
		case BSP_TOGGLE:
			HAL_GPIO_TogglePin(R_LED_GPIO_Port,R_LED_Pin);	
			break;
		default:break;
	}
}

void BSP_BLED(bsp_led_s s)
{
	switch(s)
	{
		case BSP_ON:
			HAL_GPIO_WritePin(B_LED_GPIO_Port,B_LED_Pin,GPIO_PIN_RESET);
			break;
		case BSP_OFF:
			HAL_GPIO_WritePin(B_LED_GPIO_Port,B_LED_Pin,GPIO_PIN_SET);
			break;	
		case BSP_TOGGLE:
			HAL_GPIO_TogglePin(B_LED_GPIO_Port,B_LED_Pin);	
			break;
		default:break;
	}
}

void BSP_POWER(bsp_led_s s)
{
	switch(s)
	{
		case BSP_ON:
			HAL_GPIO_WritePin(DY_EN_GPIO_Port,DY_EN_Pin,GPIO_PIN_SET);
			break;
		case BSP_OFF:
			HAL_GPIO_WritePin(DY_EN_GPIO_Port,DY_EN_Pin,GPIO_PIN_RESET);
			break;	
		case BSP_TOGGLE:
			HAL_GPIO_TogglePin(DY_EN_GPIO_Port,DY_EN_Pin);	
			break;
		default:break;
	}
}


void BSP_BC28_Reset(bsp_led_s s)
{
	switch(s)
	{
		case BSP_ON:
			HAL_GPIO_WritePin(BC28_R_GPIO_Port,BC28_R_Pin,GPIO_PIN_SET);
			break;
		case BSP_OFF:
			HAL_GPIO_WritePin(BC28_R_GPIO_Port,BC28_R_Pin,GPIO_PIN_RESET);
			break;	
		case BSP_TOGGLE:
			HAL_GPIO_TogglePin(BC28_R_GPIO_Port,BC28_R_Pin);	
			break;
		default:break;
	}
}










