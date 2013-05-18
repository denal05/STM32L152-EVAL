/**
  ******************************************************************************
  * @file    PWR/CurrentConsumption/stm32l1xx_lowpowerrun_sram.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   This file provides firmware functions to manage the STM32L1xx 
  *          Low Power Run Mode from Internal SRAM.
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

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CurrentConsumption
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the system to enter Low Power Run mode for
  *         current consumption measurement purpose. The code is running from 
  *         Internal SRAM.
  *         The maximum clock when the system is in Low Power Run mode is ~128KHz.
  *         This mode can only be entered when Voltage Range 2 is selected.    
  * @param  None
  * @retval None
  */
#if defined ( __CC_ARM   )
/* With ARM Compiler, RAM functions are defined using the toolchain options. 
   For more details please refer to your toolchain documentation. */
  void

#elif defined ( __ICCARM__ )
 __ramfunc void

#elif defined   (  __GNUC__  )
void __attribute__((section(".data")))
#elif defined   (  __TASKING__  )
#pragma section_code_init on
#endif

LowPowerRunModeSRAM_Measure(void)
{

  /* This function should be called when the program is running from internal SRAM. */
  FLASH_RUNPowerDownCmd(ENABLE);

  /* Wait Until Key button pressed */
  while(((*(__IO uint32_t *)0x40020010) & 0x0001) == 0)
  {
  }

  /* Wait Until Key button pressed */
  while(((*(__IO uint32_t *)0x40020010) & 0x0001) != 0)
  {
  }

  /* This function should be called when the program is running from internal SRAM. */
  FLASH_RUNPowerDownCmd(DISABLE);  
}

#if defined (  __TASKING__  )
#pragma section_code_init restore
#endif


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
