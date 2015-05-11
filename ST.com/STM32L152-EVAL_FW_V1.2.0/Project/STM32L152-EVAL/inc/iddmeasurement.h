/**
  ******************************************************************************
  * @file    iddmeasurement.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
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
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#define EVAL_RESISTOR_STT5PF20V          8      /* 0.085 ohm  */
#define EVAL_RESISTOR_RATIO              100    /* R35 and R36 are multiplied by 100 */
#define EVAL_MAX9938_GAIN                50     /* Ampli-op gain = 50 */
#define ADC_CONVERT_RATIO                ((IDD_Measurement_VDD() * 1000)/ 4096)     /* (VDDmV / 0xFFF)* 1000 */

#define EVAL_RESISTOR_R36                115    /* 1,15 ohm  measured value on the STM32L152-EVAL instead of 1 ohm */
#define EVAL_RESISTOR_R35                100000 /* 1 Kohm measured value on the STM32L152-EVAL instead of 1 Kohm */

/* IDD measurement pin */
#define IDD_MEASUREMENT_PIN              GPIO_Pin_5           /* PA.05 */
#define IDD_MEASUREMENT_GPIO_PORT        GPIOA                /* GPIOA */
#define IDD_MEASUREMENT_ADC_CHANNEL      ADC_Channel_5        /* ADC channel5 */

/* IDD counter enable pin */
#define IDD_CNT_EN_PIN                   GPIO_Pin_13          /* PC.13 */
#define IDD_CNT_EN_GPIO_PORT             GPIOC                /* GPIOC */

/* IDD Wake up pin */
#define IDD_WAKEUP_PIN                   GPIO_Pin_0           /* PA.00 */
#define IDD_WAKEUP_GPIO_PORT             GPIOA                /* GPIOA */
#define IDD_WAKEUP_EXTI_LINE             EXTI_Line0           /* EXTI line 0 */
#define IDD_WAKEUP_IRQn                  EXTI0_IRQn           /* EXTI line 0 interrupt */
#define IDD_WAKEUP_EXTI_PORTSOURCE       EXTI_PortSourceGPIOA /* EXTI port source */
#define IDD_WAKEUP_EXTI_PINSOURCE        EXTI_PinSource0      /* EXTI pin source */

/* STM32L Low Power Modes definition */
#define RTC_STATE_OFF                    0
#define RTC_STATE_ON                     1

#define BIAS_CALIB_OFF                   0
#define BIAS_CALIB_ON                    1

#define STM32L_MODE_RUN                  0
#define STM32L_MODE_LP                   1
#define STM32L_MODE_BIAS                 2

#define STM32L_RUN                       0
#define STM32L_RUN_1M                    1
#define STM32L_RUN_LP                    2
#define STM32L_SLEEP                     3
#define STM32L_SLEEP_LP                  4
#define STM32L_STOP                      5
#define STM32L_STANDBY                   6

#define DATA_EEPROM_STBY_ADDR            0x08080004
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
/* STM32L Low Power Modes selection functions. -------------------------------*/
void IDD_Measurement_Run_HCLKTo32(void);
void IDD_Measurement_Run_HCLKToMSI_1MHz(void);
void IDD_Measurement_RunLP(void);
void IDD_Measurement_Sleep(void);
void IDD_Measurement_SleepLP(void);
void IDD_Measurement_Stop(void);
void IDD_Measurement_EnterStandby(void);
void IDD_Measurement(uint32_t lpmode, uint8_t RTCState, uint8_t CalibrationState);

/* STM32L ADC peripheral configuration and computation functions. ------------*/
void IDD_Measurement_ADC_Config(void);
uint16_t IDD_Measurement_ADC_Supply(void);
uint32_t IDD_Measurement_VDD(void);
uint32_t IDD_Measurement_ADC_ReadValue(void);
void IDD_Measurement_ADC_DisplayValue(uint32_t CurrentData, uint32_t mode);
void IDD_Measurement_Convert(uint8_t *ptr, uint32_t idd, uint32_t mode);
void IDD_Measurement_DisplayStandby(void);

/* IDD Measurement Circuit Bias measurement functions. -----------------------*/
void IDD_Measurement_Calibrate(void);
void IDD_Measurement_Bias_Reset(void);

/* STM32L System and Demonstration Save and Restore functions. ---------------*/
uint32_t IDD_Measurement_SelectRTCState(void);
void IDD_Measurement_SaveContext(void);
void IDD_Measurement_RestoreContext(void);
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

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
