#include "bsp_flash.h"




#define FLASH_PAGE_TO_BE_PROTECTED OB_WRP1_PAGES240TO255

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Address = 0;
uint32_t PageError = 0;
uint32_t ProtectedPAGE = 0x0;
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/*Variable used to handle the Options Bytes*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;



HAL_StatusTypeDef FLASH_WRITE(uint32_t START_ADDR, uint32_t *Data, uint8_t len)
{
	uint32_t END_ADDR = START_ADDR + 4 * (len);
	uint32_t *p_data =NULL;
	HAL_StatusTypeDef state = HAL_OK;
	p_data = Data;
	/* Unlock the Flash to enable the flash control register access *************/
	__HAL_RCC_WWDG_CLK_DISABLE();
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

	/* Get pages write protection status ****************************************/
	HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

	/* Check if desired pages are already write protected ***********************/
	if((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
	{
		/* Restore write protected pages */
		OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
		OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
		OptionsBytesStruct.WRPSector0To31 = FLASH_PAGE_TO_BE_PROTECTED;
		if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
		{
			/* Error occurred while options bytes programming. **********************/
			state = HAL_ERROR;
		}
		/* Generate System Reset to load the new option byte values ***************/
		HAL_FLASH_OB_Launch();
	}
	/* Lock the Options Bytes *************************************************/
	HAL_FLASH_OB_Lock();

	/* The selected pages are not write protected *******************************/
	if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
	{
		/* Fill EraseInit structure************************************************/
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = START_ADDR;
		EraseInitStruct.NbPages     = 1;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
		{
			/*
			  Error occurred while page erase.
			  User can add here some code to deal with this error.
			  PageError will contain the faulty page and then to know the code error on this page,
			  user can call function 'HAL_FLASH_GetError()'
			*/
			state =  HAL_ERROR;
		}

		/* FLASH Word program of DATA_32 at addresses defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */
		Address = START_ADDR;
		uint32_t temp = 0;
		while (Address < END_ADDR)
		{
			temp = *p_data++;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, temp) == HAL_OK)
			{
				Address = Address + 4;
			}
			else
			{
				// Error occurred while writing data in Flash memory.
				state =  HAL_ERROR;
			}
		}
		  /* Check the correctness of written data */
    Address = START_ADDR;
		p_data = Data;
    while (Address < END_ADDR)
    {
      temp = *p_data;
			if((*(__IO uint32_t*) Address) != temp)
      {
        state =  HAL_ERROR;
      }
      Address += 4;
			p_data++;
    }
	}
	else
	{
		state =  HAL_ERROR;
	}
	/* Lock the Flash to disable the flash control register access (recommended
	   to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	__HAL_RCC_WWDG_CLK_ENABLE();
	return state;
}

HAL_StatusTypeDef FLASH_WRITE_CHAR(uint32_t START_ADDR, uint8_t  *Data, uint8_t len)
{
	uint32_t END_ADDR = START_ADDR + 4 * (len);
	uint8_t *p_data =NULL;
	HAL_StatusTypeDef state = HAL_OK;
	p_data = Data;
	/* Unlock the Flash to enable the flash control register access *************/
	__HAL_RCC_WWDG_CLK_DISABLE();
	HAL_FLASH_Unlock();

	/* Unlock the Options Bytes *************************************************/
	HAL_FLASH_OB_Unlock();

	/* Get pages write protection status ****************************************/
	HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

	/* Check if desired pages are already write protected ***********************/
	if((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
	{
		/* Restore write protected pages */
		OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
		OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
		OptionsBytesStruct.WRPSector0To31 = FLASH_PAGE_TO_BE_PROTECTED;
		if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
		{
			/* Error occurred while options bytes programming. **********************/
			state = HAL_ERROR;
		}
		/* Generate System Reset to load the new option byte values ***************/
		HAL_FLASH_OB_Launch();
	}
	/* Lock the Options Bytes *************************************************/
	HAL_FLASH_OB_Lock();

	/* The selected pages are not write protected *******************************/
	if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
	{
		/* Fill EraseInit structure************************************************/
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = START_ADDR;
		EraseInitStruct.NbPages     = 1;

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
		{
			/*
			  Error occurred while page erase.
			  User can add here some code to deal with this error.
			  PageError will contain the faulty page and then to know the code error on this page,
			  user can call function 'HAL_FLASH_GetError()'
			*/
			state =  HAL_ERROR;
		}

		/* FLASH Word program of DATA_32 at addresses defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */
		Address = START_ADDR;
		uint32_t temp = 0;
		while (Address < END_ADDR)
		{
			temp = *p_data++;
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, temp) == HAL_OK)
			{
				Address = Address + 4;
			}
			else
			{
				// Error occurred while writing data in Flash memory.
				state =  HAL_ERROR;
			}
		}
		  /* Check the correctness of written data */
    Address = START_ADDR;
		p_data = Data;
    while (Address < END_ADDR)
    {
      temp = *p_data;
			if((*(__IO uint32_t*) Address) != temp)
      {
        state =  HAL_ERROR;
      }
      Address += 4;
			p_data++;
    }
	}
	else
	{
		state =  HAL_ERROR;
	}
	/* Lock the Flash to disable the flash control register access (recommended
	   to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	__HAL_RCC_WWDG_CLK_ENABLE();
	return state;
}


/**
  * @brief  连续读取指定flash地址内容
  * @note
  * @retern null
  * @{
  */
void FlASH_READ(uint32_t Address, uint32_t *Data, uint8_t len)
{

	uint32_t START_ADDR = Address;
	uint32_t End__ADDR = Address + 4 * len;
	uint32_t *p_data = NULL;
	p_data = Data;

	while(START_ADDR < End__ADDR)
	{
		*p_data++ = (*(__IO uint32_t*) START_ADDR);
		START_ADDR += 4;
	}
}

void FlASH_READ_CHAR(uint32_t Address, uint8_t *Data, uint8_t len)
{

	uint32_t START_ADDR = Address;
	uint32_t End__ADDR = Address + 4 * len;
	uint8_t *p_data = NULL;
	p_data = Data;

	while(START_ADDR < End__ADDR)
	{
		*p_data++ = (*(__IO uint32_t*) START_ADDR);
		START_ADDR += 4;
	}
}





















