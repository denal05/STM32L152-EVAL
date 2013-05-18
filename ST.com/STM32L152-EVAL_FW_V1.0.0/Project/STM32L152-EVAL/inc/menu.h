/**
  ******************************************************************************
  * @file    menu.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-April-2011
  * @brief   This file contains all the functions prototypes for the menu navigation
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
#ifndef __MENU_H
#define __MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @addtogroup MENU
  * @{
  */

/** @defgroup MENU_Exported_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup MENU_Exported_Constants
  * @{
  */
#define  MAX_MENU_LEVELS 4
#define  NOKEY  0
#define  SEL    1
#define  RIGHT  2
#define  LEFT   3
#define  UP     4
#define  DOWN   5

/*!< LCD display update parameters */
#define MENU_STOP        0x10000000
#define MENU_PLAY        0x01000000
#define MENU_PAUSE       0x00100000 
#define MENU_PROGRESS    0x00001000
#define MENU_FRWD        0x00000100
#define MENU_ALL         0x11110100  
  
#define COMPSTATE_FULL    0x03
#define COMPSTATE_3_4     0x02
#define COMPSTATE_1_2     0x01
#define COMPSTATE_LOW     0x00
  
/**
  * @}
  */

/** @defgroup MENU_Exported_Macros
  * @{
  */
#define countof(a) (sizeof(a) / sizeof(*(a)))
/**
  * @}
  */

/** @defgroup MENU_Exported_Functions
  * @{
  */
void Menu_Init(void);
void Menu_DisplayMenu(void);
void Menu_SelFunc(void);
void Menu_UpFunc(void);
void Menu_DownFunc(void);
void Menu_ReturnFunc(void);
uint8_t Menu_ReadKey(void);
void Menu_IdleFunc(void);
void Menu_DisplayIcons(void);
void Menu_ShowMenuIcons(void);
void Menu_STM32Intro(void);
void Menu_MenuNavigationFunc(void);
void Menu_JumpConfFunc(void);
void Menu_ImagesViewerFunc(void);
void Menu_WavePlayerFunc(void);
void Menu_AboutFunc(void);
void Menu_ProductPres(void);
uint32_t Menu_CheckBitmapFiles(void);
void Menu_ThermometerFun(void);
uint8_t Menu_Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_H */
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
