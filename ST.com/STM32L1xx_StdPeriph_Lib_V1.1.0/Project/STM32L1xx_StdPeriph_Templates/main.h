/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   Header for main.c module
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include <stdio.h>

#if defined (USE_STM32L152_EVAL)
 #include "stm32l152_eval.h"
 #include "stm32l152_eval_lcd.h"
 #include "stm32l152_eval_spi_sd.h"
 #include "stm32l152_eval_i2c_tsensor.h"
 #include "stm32l152_eval_i2c_ee.h"
 #include "stm32l152_eval_glass_lcd.h"
#elif defined (USE_STM32L152D_EVAL)
 #include "stm32l152d_eval.h"
 #include "stm32l152d_eval_lcd.h"
 #include "stm32l152d_eval_spi_ee.h"
 #include "stm32l152d_eval_sdio_sd.h"
 #include "stm32l152d_eval_i2c_tsensor.h"
 /*#include "stm32l152d_eval_i2c_ee.h"*/
 #include "stm32l152d_eval_glass_lcd.h"
 #include "stm32l152d_eval_fsmc_sram.h"
 #include "stm32l152d_eval_fsmc_nor.h"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
