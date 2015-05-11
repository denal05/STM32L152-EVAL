/**
  ******************************************************************************
  * @file    storage.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
  * @brief   This file includes the Storage (FatFs) driver for the STM32L152-EVAL
  *          demo.
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
#include "main.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @defgroup STORAGE
  * @brief This file includes the Storage (FatFs) driver for the STM32L152-EVAL
  *        demo.
  * @{
  */

/** @defgroup STORAGE_Private_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup STORAGE_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup STORAGE_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STORAGE_Private_Variables
  * @{
  */
FATFS fs;
FILINFO fno;
DIR dir;
FIL F;
UINT BytesWritten;
UINT BytesRead;
extern const uint8_t SlidesCheck[];
extern uint8_t Buffer1[];
/**
  * @}
  */


/** @defgroup STORAGE_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup STORAGE_Private_Functions
  * @{
  */

/**
  * @brief  SDCARD Initialisation for FatFs
  * @param  None
  * @retval err : Error status (0=> success, 1=> fail)
  */
uint32_t Storage_Init(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  SD_Init();

  SYSCFG_EXTILineConfig(SD_DETECT_EXTI_PORT_SOURCE, SD_DETECT_EXTI_PIN_SOURCE);

  /*!< Configure the SD Card detect Pin as input with interrupt */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = SD_DETECT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /****************** FatFs Volume Acess ******************************/

  if (f_mount(0, &fs))
  {
    return 1;
  }
  return 0;
}

/**
  * @brief  Open a file and copy its content to a buffer
  * @param  DirName: the Directory name to open
  * @param  FileName: the file name to open
  * @param  BufferAddress: A pointer to a buffer to copy the file to
  * @param  FileLen: the File lenght
  * @retval err: Error status (0=> success, 1=> fail)
  */
uint32_t Storage_OpenReadFile(uint8_t Xpoz, uint16_t Ypoz, const char* BmpName)
{
  uint32_t index = 0, size = 0, width = 0, height = 0, i1 = 0;
  uint32_t BmpAddress;
  FIL F1;

  f_open (&F1, BmpName, FA_READ);
  f_read (&F1, Buffer1, 26, &BytesRead);

  BmpAddress = (uint32_t)Buffer1;

  /* Read bitmap size */
  size = *(uint16_t *) (BmpAddress + 2);
  size |= (*(uint16_t *) (BmpAddress + 4)) << 16;

  /* Get bitmap data address offset */
  index = *(uint16_t *) (BmpAddress + 10);
  index |= (*(uint16_t *) (BmpAddress + 12)) << 16;

  /* Read bitmap width */
  width = *(uint16_t *) (BmpAddress + 18);
  width |= (*(uint16_t *) (BmpAddress + 20)) << 16;

  /* Read bitmap height */
  height = *(uint16_t *) (BmpAddress + 22);
  height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

  size = (size - index) / 2;

  /* Synchronize f_read right in front of the image data */
  f_read (&F1, Buffer1, index - 26, &BytesRead);

  /*Setup the window*/
  LCD_SetDisplayWindow(Xpoz, Ypoz, height, width);

  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  LCD_WriteReg(R3, 0x1008);
  LCD_WriteRAM_Prepare();

  do
  {
    if (size*2 < 512)
    {
      i1 = size * 2;
    }
    else
    {
      i1 = 512;
    }
    size -= i1 / 2;
    f_read (&F1, Buffer1, i1, &BytesRead);

    for (index = 0; index < i1 / 2; index++)
    {
      LCD_WriteRAM(*(uint16_t *)BmpAddress);
      BmpAddress += 2;  /*jump on next pixel*/
    }
    BmpAddress = (uint32_t)Buffer1;
  }
  while (size > 0);

  f_close (&F1);

  LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (LCD_DIR_HORIZONTAL : increment, LCD_DIR_VERTICAL : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  LCD_WriteReg(LCD_REG_3, 0x1018);
  
  return 1;
}

/**
  * @brief  Open a file and copy its content to a buffer
  * @param  DirName: the Directory name to open
  * @param  FileName: the file name to open
  * @param  BufferAddress: A pointer to a buffer to copy the file to
  * @param  FileLen: the File lenght
  * @retval err: Error status (0=> success, 1=> fail)
  */
uint32_t Storage_CheckBitmapFile(const char* BmpName, uint32_t *FileLen)
{

  if (f_mount(0, &fs))
  {
    return 1;
  }
  if (f_open (&F, BmpName, FA_READ))
  {
    return 2;
  }

  f_read (&F, Buffer1, 6, &BytesRead);

  if (Menu_Buffercmp((uint8_t *)SlidesCheck, (uint8_t *) Buffer1, 6) != 0)
  {
    return 3;
  }
  return 0;
}

/**
  * @brief  List up to 25 file on the root directory with extension .BMP
  * @param  None
  * @retval The number of the found files
  */
uint32_t Storage_GetDirectoryBitmapFiles (const char* DirName, char* Files[])
{
  uint32_t i = 0, j = 0;
  FRESULT res;

  f_mount(0, &fs);
  res = f_opendir(&dir, DirName);

  if (res == FR_OK)
  {
    i = strlen(DirName);
    for (;;)
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) break;
      if (fno.fname[0] == '.') continue;

      if (!(fno.fattrib & AM_DIR))
      {
        do
        {
          i++;
        }
        while (fno.fname[i] != 0x2E);


        if (j < MAX_FILES)
        {
          if ((fno.fname[i + 1] == 'B') && (fno.fname[i + 2] == 'M') && (fno.fname[i + 3] == 'P'))
          {
            sprintf (Files[j], "%-11.11s", fno.fname);
            j++;
          }
        }
        i = 0;
      }
    }
  }

  f_mount (0, NULL);

  return j;
}

/**
  * @brief  List up to 25 file on the root directory with extension .WAV
  * @param  None
  * @retval The number of the found files
  */
uint32_t Storage_GetDirectoryWaveFiles (const char* DirName, char* Files[])
{
  uint32_t i = 0, j = 0;
  FRESULT res;

  f_mount(0, &fs);
  res = f_opendir(&dir, DirName);

  if (res == FR_OK)
  {
    i = strlen(DirName);
    for (;;)
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) break;
      if (fno.fname[0] == '.') continue;

      if (!(fno.fattrib & AM_DIR))
      {
        do
        {
          i++;
        }
        while (fno.fname[i] != 0x2E);
        if (j < MAX_FILES)
        {
          if ((fno.fname[i + 1] == 'W') && (fno.fname[i + 2] == 'A') && (fno.fname[i + 3] == 'V'))
          {
            sprintf (Files[j], "%-12.12s", fno.fname);
            j++;
          }
        }
        i = 0;
      }
    }
  }

  f_mount (0, NULL);

  return j;
}


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
