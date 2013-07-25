/**
  ******************************************************************************
  * @file    PWR/CurrentConsumption/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   Main program body
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
#include "stm32l1xx_ulp_modes.h"

#ifdef USE_STM32L152D_EVAL
  #include "stm32l152d_eval.h"
  #include "stm32l152d_eval_lcd.h"
#else
  #include "stm32l152_eval.h"
  #include "stm32l152_eval_lcd.h"
#endif


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
__IO uint32_t Counter = 0, BOROptionBytes = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */  

  /* Initialize the LCD */
  STM32L152_LCD_Init();	
  LCD_Clear( Blue );
  LCD_SetBackColor( Blue );
  LCD_SetTextColor( White );
  LCD_DisplayStringLine( Line0, " DELAY 5 SECONDS... " );
  for(Counter = 0; Counter < 0xFFFFFF; Counter++);
  LCD_Clear( Blue );
  LCD_DisplayStringLine( Line0, "   STM32L152-EVAL   " );
  LCD_DisplayStringLine( Line1, " StdPeriphLibV1.1.0 " );
  LCD_DisplayStringLine( Line2, "PWR/CurrentConsumptn" );
  LCD_DisplayStringLine( Line3, "                    " );
  LCD_DisplayStringLine( Line4, "INSTRUCTIONS:       " );
  LCD_DisplayStringLine( Line5, "Uncomment mode in   " );
  LCD_DisplayStringLine( Line6, "stm32l1xx_ulp_modes." );
  LCD_DisplayStringLine( Line7, "h file. Use external" );
  LCD_DisplayStringLine( Line8, "amperemeter to meas-" );
  LCD_DisplayStringLine( Line9, "ure IDD current.    " );
  
  /* Enable PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);
#ifdef BOR_MODIFY
  /* Get BOR Option Bytes */
  BOROptionBytes = FLASH_OB_GetBOR();

  if((BOROptionBytes & 0x0F) != BOR_LEVEL) 
  {
    /* Unlocks the option bytes block access */
    FLASH_OB_Unlock();

    /* Clears the FLASH pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);

    /* Select the desired V(BOR) Level ---------------------------------------*/
    FLASH_OB_BORConfig(BOR_LEVEL); 

    /* Launch the option byte loading */
    FLASH_OB_Launch();  
  }
#endif  

#if defined (LP_RUN_SRAM_MODE)
  /* Low Power Run Mode Entry:
      - System Running at MSI (~32KHz)
      - Flash 0 wait state
      - Voltage Range 2
      - Code running from Internal SRAM
      - All peripherals OFF
      - FLASH switched OFF
      - VDD from 1.65V to 3.6V
      - Current Consumption ~10.5uA
      - Wakeup using Key Button PA.00
   */
  LowPowerRunMode_Measure();
#elif defined (LP_RUN_FLASH_MODE)
  /* Low Power Run Mode Entry:
      - System Running at MSI (~32KHz)
      - Flash 0 wait state
      - Voltage Range 2
      - Code running from Internal FLASH
      - All peripherals OFF
      - VDD from 1.65V to 3.6V
      - Current Consumption ~25uA
      - Wakeup using Key Button PA.00
   */
  LowPowerRunMode_Measure();  
#elif defined (SLEEP_MODE)
  /* Sleep Mode Entry 
      - System Running at HSI (16MHz)
      - Flash 1 wait state
      - Voltage Range 2
      - Code running from Internal FLASH
      - Current Consumption ~1mA
      - Wakeup using EXTI Line (Key Button PA.00)
   */
  SleepMode_Measure();
#elif defined (LP_SLEEP_MODE)
  /* Low Power Sleep Mode Entry 
      - System Running at MSI (~32KHz)
      - Flash 0 wait state
      - Voltage Range 2
      - Code running from Internal FLASH
      - All peripherals OFF
      - VDD from 1.65V to 3.6V
      - Current Consumption ~4.07uA
      - Wakeup using EXTI Line (Key Button PA.00)
   */
  LowPowerSleepMode_Measure();
#elif defined (STOP_MODE)
  /* STOP Mode Entry 
      - Regulator in LP mode
      - LSI, HSI and HSE OFF
      - No IWDG
      - Current Consumption ~0.5uA
      - Wakeup using EXTI Line (Key Button PA.00)
   */
  StopMode_Measure();
#elif defined (STOP_RTC_LSE_MODE)
  /* STOP Mode with RTC on LSE Entry 
      - RTC Clocked by LSE external Clock (32.768KHz)
      - Regulator in LP mode
      - LSI, HSI and HSE OFF
      - No IWDG
      - Current Consumption ~1.6uA
      - Automatic Wakeup using RTC on LSE (4s)
   */
  StopRTCLSEMode_Measure();
#elif defined (STOP_RTC_LSI_MODE)
  /* STOP Mode with RTC on LSI Entry 
      - RTC Clocked by LSI
      - Regulator in LP mode
      - HSI and HSE OFF
      - No IWDG   
      - Current Consumption ~1.3uA
      - Automatic Wakeup using RTC on LSI (after ~4s)
   */
  StopRTCLSIMode_Measure();
#elif defined (STANDBY_MODE)
  /* STANDBY Mode Entry 
      - IWDG and LSI OFF
      - Current Consumption ~0.3uA
      - Wakeup using WakeUp Pin 1 (PA.00)
   */
  StandbyMode_Measure();
#elif defined (STANDBY_RTC_LSE_MODE)
  /* STANDBY Mode with RTC on LSE Entry 
      - RTC Clocked by LSE external Clock (32.768KHz)
      - IWDG and LSI OFF
      - Current Consumption ~1.3uA
      - Automatic Wakeup using RTC on LSE (after 4s)
   */
  StandbyRTCLSEMode_Measure();
#elif defined (STANDBY_RTC_LSI_MODE)
  /* STANDBY Mode with RTC on LSI Entry 
      - RTC Clocked by LSI
      - IWDG OFF
      - Current Consumption ~1.1uA
      - Automatic Wakeup using RTC on LSI (after ~4s)
  */
  StandbyRTCLSIMode_Measure();
#else
  /* Initialize LED1 on STM32L152-EVAL board */
  STM_EVAL_LEDInit(LED1);
  
  /* Infinite loop */
  while (1)
  {
    /* Toggle The LED1 */
    STM_EVAL_LEDToggle(LED1);

    /* Inserted Delay */
    for(Counter = 0; Counter < 0x5FF; Counter++);
  }
#endif
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
