/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : mass_storage.h
* Author             : MCD Application Team
* version            : V1.0.0
* date               : 01-April-2011
* Description        : This file contains all the functions prototypes for the
*                      Mass Storage firmware driver.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASS_STORAGE_H
#define __MASS_STORAGE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Mass_Storage_Init(void);
void Mass_Storage_Start(void);

#endif /* __MASS_STORAGE_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
