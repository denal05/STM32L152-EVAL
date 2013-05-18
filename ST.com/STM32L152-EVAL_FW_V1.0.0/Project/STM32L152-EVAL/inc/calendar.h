/**
  ******************************************************************************
  * @file    calendar.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-April-2011
  * @brief   This file contains all the functions prototypes for the calendar 
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
#ifndef __CALENDAR_H
#define __CALENDAR_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @addtogroup CALENDAR
  * @{
  */

/** @defgroup CALENDAR_Exported_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Exported_Constants
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup CALENDAR_Exported_Functions
  * @{
  */
uint8_t Calendar_ReadDigit(uint8_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin);
void Calendar_Init(void);
void Calendar_TimeRegulate(void);
void Calendar_TimeAdjust(void);
void Calendar_TimeShow(void);
void Calendar_TimeDisplay(void);
void Calendar_DateRegulate(void);
void Calendar_DateAdjust(void);
void Calendar_DateDisplay(uint16_t nYear, uint8_t nMonth, uint8_t nDay);
void Calendar_DateShow(void);
void Calendar_DateUpdate(void);
void Calendar_AlarmRegulate_A(void);
void Calendar_AlarmAdjust_A(void);
void Calendar_AlarmPreAdjust_A(void);
void Calendar_AlarmDisplay_A(void);
void Calendar_AlarmShow_A(void);
void Calendar_AlarmRegulate_B(void);
void Calendar_AlarmAdjust_B(void);
void Calendar_AlarmPreAdjust_B(void);
void Calendar_AlarmDisplay_B(void);
void Calendar_AlarmShow_B(void);
void Calendar_RTC_Configuration(void);
void LCD_ShowTimeCalendar(void);
void LCD_ShowDateCalendar(void);

#ifdef __cplusplus
}
#endif

#endif /* __CALENDAR_H */
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
