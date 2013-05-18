/**
  ******************************************************************************
  * @file    PWR/CurrentConsumption/stm32l1xx_ulp_modes.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   Header for stm32l1xx_ulp_modes.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L1xx_ULP_MODES_H
#define __STM32L1xx_ULP_MODES_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

#ifdef USE_STM32L152D_EVAL
#include "stm32l152d_eval.h"
#else
#include "stm32l152_eval.h"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if !defined (LP_RUN_SRAM_MODE) && !defined (LP_RUN_FLASH_MODE) && !defined (SLEEP_MODE)\
 && !defined (LP_SLEEP_MODE) && !defined (STOP_MODE) && !defined (STOP_RTC_LSE_MODE)\
 && !defined (STOP_RTC_LSI_MODE) && !defined (STANDBY_MODE) && !defined (STANDBY_RTC_LSE_MODE)\
 && !defined (STANDBY_RTC_LSI_MODE)
/* Uncomment the corresponding line to select the STM32L1xx Ultra Low Power mode */
//#define LP_RUN_SRAM_MODE
//#define LP_RUN_FLASH_MODE
//#define SLEEP_MODE
//#define LP_SLEEP_MODE
//#define STOP_MODE
//#define STOP_RTC_LSE_MODE
//#define STOP_RTC_LSI_MODE
//#define STANDBY_MODE
//#define STANDBY_RTC_LSE_MODE
//#define STANDBY_RTC_LSI_MODE
#endif


#if !defined (LP_RUN_SRAM_MODE) && !defined (LP_RUN_FLASH_MODE) && !defined (SLEEP_MODE)\
 && !defined (LP_SLEEP_MODE) && !defined (STOP_MODE) && !defined (STOP_RTC_LSE_MODE)\
 && !defined (STOP_RTC_LSI_MODE) && !defined (STANDBY_MODE) && !defined (STANDBY_RTC_LSE_MODE)\
 && !defined (STANDBY_RTC_LSI_MODE)
 #error "Please select first the target STM32L1xx Ultra Low Power mode to be measured (in stm32l1xx_ulp_modes.h file)"
#endif

#define BOR_MODIFY
/* Select the BOR level */
#ifdef BOR_MODIFY
#define BOR_LEVEL OB_BOR_OFF  /*!< BOR is disabled at power down, the reset is asserted when the VDD power supply reachs the PDR(Power Down Reset) threshold (1.5V) */
//#define BOR_LEVEL OB_BOR_LEVEL1  /*!< BOR Reset threshold levels for 1.7V - 1.8V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL2  /*!< BOR Reset threshold levels for 1.9V - 2.0V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL3  /*!< BOR Reset threshold levels for 2.3V - 2.4V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL4  /*!< BOR Reset threshold levels for 2.55V - 2.65V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL5  /*!< BOR Reset threshold levels for 2.8V - 2.9V VDD power supply */
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LowPowerRunMode_Measure(void);
void LowPowerRunModeSRAM_Measure(void);
void SleepMode_Measure(void);
void LowPowerSleepMode_Measure(void);
void StopMode_Measure(void);
void StopRTCLSEMode_Measure(void);
void StopRTCLSIMode_Measure(void);
void StandbyMode_Measure(void);
void StandbyRTCLSEMode_Measure(void);
void StandbyRTCLSIMode_Measure(void);

#endif /* __STM32L1xx_ULP_MODES_H */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
