/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : mass_storage.c
* Author             : MCD Application Team
* Version            : V1.2.0
* Date               : 24-January-2012
* Description        : This file provides a set of functions needed to manage the
*                      communication between the STM32L1xx USB and the SD Card.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
* LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t bmplen;
extern uint8_t Buffer1[];
extern uint16_t MAL_Init (uint8_t lun);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Mass_Storage_Init
* Description    : Initializes the peripherals used by the mass storage driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Mass_Storage_Init(void)
{
  /* Disable the Pull-Up*/
  USB_Cable_Config(DISABLE);
}

/*******************************************************************************
* Function Name  : Mass_Storage_Start
* Description    : Starts the mass storage demo.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Mass_Storage_Start (void)
{
  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);


  while (Menu_ReadKey() != NOKEY)
  {}

  /* Clear*/
  LCD_Clear(LCD_COLOR_WHITE);

  LCD_SetDisplayWindow(160, 223, 128, 128);

  Storage_OpenReadFile(160, 223, "STFILES/USB.BMP");
  /* Disable LCD Window mode */
  LCD_WindowModeDisable();

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);


  /* Display the "  Plug the USB   " message */
  LCD_DisplayStringLine(LCD_LINE_8, " Plug the USB Cable ");
  LCD_DisplayStringLine(LCD_LINE_9, "Exit:  Push JoyStick");

  Set_System();
  Set_USBClock();

  USB_Interrupts_Config();
  USB_Init();

  while (bDeviceState != CONFIGURED)
  {
    if (Menu_ReadKey() != NOKEY)
    {
      PowerOff();
      LCD_Clear(LCD_COLOR_WHITE);
      Menu_DisplayMenu();
      Demo_IntExtOnOffConfig(ENABLE);
      return;
    }
  }

  LCD_ClearLine(LCD_LINE_9);
  /* Display the "To stop Press SEL" message */
  LCD_DisplayStringLine(LCD_LINE_8, "  To stop Press SEL ");

  /* Loop until SEL key pressed */
  while (Menu_ReadKey() != SEL)
  {}
 
  PowerOff();

  /* Checks the availability of the bitmap files */
  Demo_CheckBitmapFilesStatus(); 
  
  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  Demo_IntExtOnOffConfig(ENABLE);
}

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
