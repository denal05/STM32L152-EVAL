/**
  ******************************************************************************
  * @file    FLASH/Data_Program/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#if defined USE_STM32L152D_EVAL
  #include "stm32l152d_eval.h"
#elif defined USE_STM32L152_EVAL
  #include "stm32l152_eval.h"
#endif

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Data_Program
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define DATA_EEPROM_START_ADDR     0x08080000
#define DATA_EEPROM_END_ADDR       0x080803FF
#define DATA_EEPROM_PAGE_SIZE      0x8
#define DATA_32                    0x12345678
#define FAST_DATA_32               0x55667799

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus DataMemoryProgramStatus = PASSED;
uint32_t NbrOfPage = 0, j = 0, Address = 0;
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */
  
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);

  /* Unlock the FLASH PECR register and Data EEPROM memory */
  DATA_EEPROM_Unlock();
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);	
  
  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
      DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */  
  Address = DATA_EEPROM_START_ADDR;

  NbrOfPage = ((DATA_EEPROM_END_ADDR - Address) + 1 ) >> 2; 
  
  /* Erase the Data EEPROM Memory pages by Word (32-bit) */
  for(j = 0; j < NbrOfPage; j++)
  {
    FLASHStatus = DATA_EEPROM_EraseWord(Address + (4 * j));
  }
   
  /* Check the correctness of written data */
  while(Address < DATA_EEPROM_END_ADDR)
  {
    if(*(__IO uint32_t*)Address != 0x0)
    {
      DataMemoryProgramStatus = FAILED;
    }
    Address = Address + 4;
  }

  Address = DATA_EEPROM_START_ADDR;
  
  /* Program the Data EEPROM Memory pages by Word (32-bit) */
  while(Address <= DATA_EEPROM_END_ADDR )
  {
    FLASHStatus = DATA_EEPROM_FastProgramWord(Address, FAST_DATA_32);

    if(FLASHStatus == FLASH_COMPLETE)
    {
      Address = Address + 4;
    }
    else
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                           | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
    }
  }
  
  Address = DATA_EEPROM_START_ADDR;
  
  /* Check the correctness of written data */
  while(Address < DATA_EEPROM_END_ADDR)
  {
    if(*(__IO uint32_t*)Address != FAST_DATA_32)
    {
      DataMemoryProgramStatus = FAILED;
    }
	  Address = Address + 4;
  }

  /*  Data EEPROM Word program of DATA_32 at addresses defined by DATA_EEPROM_START_ADDR
      and DATA_EEPROM_END_ADDR */  
  Address = DATA_EEPROM_START_ADDR;

  while(Address <= DATA_EEPROM_END_ADDR )
  {
    FLASHStatus = DATA_EEPROM_ProgramWord(Address, DATA_32);

    if(FLASHStatus == FLASH_COMPLETE)
    {
      Address = Address + 4;
    }
    else
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                           | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
    }
  }

  Address = DATA_EEPROM_START_ADDR;

  /* Check the correctness of written data */
  while(Address < DATA_EEPROM_END_ADDR)
  {
    if(*(__IO uint32_t*)Address != DATA_32)
    {
      DataMemoryProgramStatus = FAILED;
    }
    Address = Address + 4;
  }
  
  if (DataMemoryProgramStatus != FAILED)
  {	
    /* OK */
    /* Turn on LD1 */
    STM_EVAL_LEDOn(LED1);
  }
  else
  { 
    /* KO */
    /* Turn on LD2 */
    STM_EVAL_LEDOn(LED2);     
  } 

  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
