#include "bsp_rtc.h"

#include "rtc.h"


void StandbyMode_Measure(void)
{

//      /* Enable Power Clock*/
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Check and handle if the system was resumed from StandBy mode */
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		/* Clear Standby flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
	}

	/* Disable all used wakeup sources: WKUP pin */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

	/* Disable all used wakeup sources*/
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	/* Clear all related wakeup flags */
	//__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

//	  /* Check and Clear the Wakeup flag */
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_WU) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	}


//    /* Clear wake up Flag */
//    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
	/* Re-enable all used wakeup sources*/
	/* ## Setting the Wake up time ############################################*/
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, WakeTime, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
	{
//				Error_Handler();
	}
	/* Enable wakeup pin WKUP1 */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);
	
//	red_led(0);
//	blue_led(0);
	/*## Enter Standby Mode ####################################################*/
	/* Request to enter STANDBY mode  */
	HAL_PWR_EnterSTANDBYMode();

}
















