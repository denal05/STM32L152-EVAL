/**
  ******************************************************************************
  * @file    iddmeasurement.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-April-2011
  * @brief   This file contains all the functions prototypes for the idd measurement
  *          firmware driver.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IDDMEASUREMENT_H
#define __IDDMEASUREMENT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @addtogroup IDD_MEASUREMENT
  * @{
  */

/** @defgroup IDD_MEASUREMENT_Exported_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Exported_Constants
  * @{
  */
#define EVAL_RESISTOR_STT5PF20V                8      /* 0.085 ohm  */
#define EVAL_RESISTOR_RATIO                   100     /* R35 and R36 are multiplied by 100 */
#define EVAL_MAX9938_GAIN                     50      /* Ampli-op gain = 50 */
#define ADC_CONVERT_RATIO                     806     /* (3300mV / 0xFFF)* 1000 */
#define ADC_CONVERT_RATIO1000                805860   /* (3300mV / 0xFFF)* 1000000 */

#define EVAL_RESISTOR_R36                     115                          /* 1,15 ohm  measured value on the STM8L152-EVAL instead of 1 ohm */
#define EVAL_RESISTOR_R35                     100000                       /* 1 Kohm measured value on the STM8L152-EVAL instead of 1 Kohm */

/*IDD measurement pin*/
#define IDD_MEASUREMENT_PIN                   GPIO_Pin_5                  /* PA.5 */
#define IDD_MEASUREMENT_GPIO_PORT             GPIOA                       /* GPIOA */
#define IDD_MEASUREMENT_ADC_CHANNEL           ADC_Channel_5               /* ADC channel5*/

/* IDD counter enable pin*/
#define IDD_CNT_EN_PIN                        GPIO_Pin_13                 /* PC.13 */
#define IDD_CNT_EN_GPIO_PORT                  GPIOC                       /* GPIOC */

/* IDD Wake up pin*/
#define IDD_WAKEUP_PIN                        GPIO_Pin_0                  /* PA.00 */
#define IDD_WAKEUP_GPIO_PORT                  GPIOA                       /* GPIOA */
#define IDD_WAKEUP_EXTI_LINE                  EXTI_Line0                  /* Exti line 0 */
#define IDD_WAKEUP_IRQn                       EXTI0_IRQn                  /* Exti line 0 interrupt */
#define IDD_WAKEUP_EXTI_PORTSOURCE            EXTI_PortSourceGPIOA        /* Exti port source */
#define IDD_WAKEUP_EXTI_PINSOURCE             EXTI_PinSource0             /* Exti pin source */

/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Exported_Functions
  * @{
  */
void IDD_Measurement_Run_HCLKTo32(void);
void IDD_Measurement_Run_HCLKToMSI_1MHz(void);
void IDD_Measurement_Sleep(void);
void IDD_Measurement_SleepLP(void);
void IDD_Measurement_RunLP(void);
void IDD_Measurement_Stop(void);
void IDD_Measurement_Standby(void);
void IDD_Enter_Standby(void);
void IDD_Display_Standby(void);
void IDD_Measurement_SaveGPIOConfig(void);
void IDD_Measurement_RestoreGPIOConfig(void);
void IDD_Measurement_SYSCLKConfig_STOP(void);
#ifdef __cplusplus
}
#endif

#endif /* __IDDMEASUREMENT_H */
/**
  * @}
  */
  
/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
