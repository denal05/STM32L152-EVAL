/**
  ******************************************************************************
  * @file    iddmeasurement.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the IDD measurement circuit for the STM32L152-EVAL
  *          demonstration:
  *           + STM32L Low Power Modes selection
  *           + STM32L ADC peripheral configuration and computation
  *           + IDD Measurement Circuit Bias measurement
  *           + STM32L System and Demonstration Save and Restore   
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

/** @defgroup IDD_MEASUREMENT
  * @brief    This file includes the IDD measurement driver for the STM32L152-EVAL
  *           demonstration.
  * @{
  */

/** @defgroup IDD_MEASUREMENT_Private_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Private_Defines
  * @{
  */
#define DATA_EEPROM_BIAS_ADDR  0x08080000

#if   defined ( __TASKING__ )
#define __NOP()  __nop()
#endif

/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Private_Variables
  * @{
  */

uint8_t Str[20];
__IO uint8_t LowPowerStatus = 0x00;

__IO uint32_t RCC_AHBENR = 0, RCC_APB2ENR = 0, RCC_APB1ENR = 0;


uint32_t GPIOA_MODER = 0, GPIOB_MODER = 0, GPIOC_MODER = 0, GPIOD_MODER = 0,
         GPIOE_MODER = 0xA8000000, GPIOH_MODER = 0x10;

uint32_t GPIOA_PUPDR = 0x64000000, GPIOB_PUPDR = 0x00000100, GPIOC_PUPDR = 0, GPIOD_PUPDR = 0,
                       GPIOE_PUPDR = 0, GPIOH_PUPDR = 0;



#define RCC_APB2Periph_CLOCK    RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1 | RCC_APB2Periph_SPI1 |\
                                RCC_APB2Periph_SYSCFG | RCC_APB2Periph_TIM9 | RCC_APB2Periph_TIM10 |\
                                RCC_APB2Periph_TIM11

#define RCC_APB1Periph_CLOCK    RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 |\
                                RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7 | RCC_APB1Periph_WWDG |\
                                RCC_APB1Periph_SPI2 | RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 |\
                                RCC_APB1Periph_I2C1 | RCC_APB1Periph_I2C2 | \
                                RCC_APB1Periph_DAC  | RCC_APB1Periph_LCD  | RCC_APB1Periph_COMP |\
                                RCC_APB1Periph_USB


#define RCC_AHBPeriph_CLOCK    RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOD |\
                               RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH |\
                               RCC_AHBPeriph_CRC | RCC_AHBPeriph_FLITF | RCC_AHBPeriph_DMA1 |\
                               RCC_AHBPeriph_SRAM
/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup IDD_MEASUREMENT_Private_Functions
  * @{
  */

/**
  * @brief  This function configures the system to be clocked by external High-Speed
  *         oscillator (HSE) with the PLL at 32MHz for current consumption 
  *         measurement purpose.
  *         Run Mode at 32MHz (HSE+PLL) from FLASH:
  *         ======================================
  *           - System Running at 32MHz
  *           - Flash 1 wait state  
  *           - Voltage Range 1
  *           - Code running from Internal FLASH
  *           - All peripherals OFF
  *           - VDD from 1.65V to 3.6V
  *           - Current Consumption ~12mA
  * @param  None
  * @retval None
  */
void IDD_Measurement_Run_HCLKTo32(void)
{
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString("  RUN   ");
  LCD_DisplayStringLine(LCD_LINE_0, "Run at System Clock ");
  LCD_DisplayStringLine(LCD_LINE_1, "   32MHz and all    ");
  LCD_DisplayStringLine(LCD_LINE_2, "  peripherals OFF    ");

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_RUN, RTC_STATE_ON, BIAS_CALIB_OFF);
}

/**
  * @brief  This function configures the system to be clocked by internal Multi-Speed
  *         oscillator (MSI) at 1MHz for current consumption measurement purpose.
  *         Run Mode at 1MHz (MSI) from FLASH
  *         =================================
  *           - System Running at MSI (~1MHz)
  *           - Flash 0 wait state  
  *           - Voltage Range 2
  *           - Code running from Internal FLASH
  *           - All peripherals OFF
  *           - VDD from 1.65V to 3.6V
  *           - Current Consumption ~0.6mA
  * @param  None
  * @retval None
  */
void IDD_Measurement_Run_HCLKToMSI_1MHz(void)
{
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, "Run at MSI 1MHz and ");
  LCD_DisplayStringLine(LCD_LINE_1, "all peripherals OFF.");

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_RUN_1M, RTC_STATE_ON, BIAS_CALIB_OFF);  
}

/**
  * @brief  This function configures the system to enter Low Power Run mode for
  *         current consumption measurement purpose.
  *         The maximum clock when the system is in Low Power Run mode is ~128KHz.
  *         This mode can only be entered when Voltage Range 2 is selected. 
  *         Low Power Run Mode from FLASH
  *         =============================
  *           - System Running at MSI (~32KHz)
  *           - Flash 0 wait state  
  *           - Voltage Range 2
  *           - Code running from Internal FLASH
  *           - All peripherals OFF
  *           - RTC could be ON or OFF  
  *           - VDD from 1.65V to 3.6V
  *           - Current Consumption ~25uA
  * @param  None
  * @retval None
  */
void IDD_Measurement_RunLP(void)
{
  uint8_t rtcstate;

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString(" RUN LP ");

  rtcstate =  IDD_Measurement_SelectRTCState();

  LCD_DisplayStringLine(LCD_LINE_0, " MCU in RUN LP Mode ");
  LCD_DisplayStringLine(LCD_LINE_1, "Run at MSI 32KHz    ");
  LCD_DisplayStringLine(LCD_LINE_2, "from Flash memory.  ");
  LCD_DisplayStringLine(LCD_LINE_3, "All peripherals OFF ");
  LCD_DisplayStringLine(LCD_LINE_4, "all I/O in Analog   ");

  if(rtcstate == RTC_STATE_ON)
  {
    LCD_DisplayStringLine(LCD_LINE_5, "input and RTC ON.   ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_5, "input and RTC OFF.   ");
  }

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_RUN_LP, rtcstate, BIAS_CALIB_OFF); 
}

/**
  * @brief  This function configures the system to enter Sleep mode for
  *         current consumption measurement purpose.
  *         Sleep Mode
  *         ==========  
  *            - System Running at HSI (16MHz)
  *            - Flash 1 wait state  
  *            - Voltage Range 2
  *            - Code running from Internal FLASH
  *            - Current Consumption ~1mA 
  *            - Wakeup using EXTI Line (Key Button PA.00)   
  * @param  None
  * @retval None
  */
void IDD_Measurement_Sleep(void)
{
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString(" SLEEP  ");
  LCD_DisplayStringLine(LCD_LINE_0, "     SLEEP Mode     ");
  LCD_DisplayStringLine(LCD_LINE_2, "Run at System Clock ");
  LCD_DisplayStringLine(LCD_LINE_3, "16MHz, RTC ON, all  ");
  LCD_DisplayStringLine(LCD_LINE_4, "peripherals OFF and ");
  LCD_DisplayStringLine(LCD_LINE_5, "I/Os in Analog input");

  LCD_DisplayStringLine(LCD_LINE_8, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_9, "continue.           ");
  while (Menu_ReadKey() == NOKEY)
  {}

  LCD_DisplayStringLine(LCD_LINE_0, " MCU in SLEEP Mode  ");
  LCD_DisplayStringLine(LCD_LINE_8, "Press KEY button to ");
  LCD_DisplayStringLine(LCD_LINE_9, "wakeup.             ");

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_SLEEP, RTC_STATE_ON, BIAS_CALIB_OFF);
}

/**
  * @brief  This function configures the system to enter Low Power Sleep mode for
  *         current consumption measurement purpose.
  *         The maximum clock when the system is in Low Power Run mode is ~128KHz.
  *         This mode can only be entered when Voltage Range 2 is selected. 
  *         Low Power Sleep Mode
  *         ====================   
  *           - System Running at MSI (~32KHz)
  *           - Flash 0 wait state  
  *           - Voltage Range 2
  *           - Code running from Internal FLASH
  *           - All peripherals OFF
  *           - RTC could be ON or OFF  
  *           - VDD from 1.65V to 3.6V
  *           - Current Consumption ~4.07uA
  *           - Wakeup using EXTI Line (Key Button PA.00)
  * @param  None
  * @retval None
  */
void IDD_Measurement_SleepLP(void)
{
  uint8_t rtcstate;

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString("SLEEP LP");

  rtcstate =  IDD_Measurement_SelectRTCState();

  LCD_DisplayStringLine(LCD_LINE_0, "MCU will be in SLEEP");
  LCD_DisplayStringLine(LCD_LINE_1, "       LP Mode      ");
  LCD_DisplayStringLine(LCD_LINE_2, "Run at MSI 32KHz all");
  LCD_DisplayStringLine(LCD_LINE_3, "peripherals OFF,    ");
  LCD_DisplayStringLine(LCD_LINE_4, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_5, "input and  RTC ON.  ");

  if(rtcstate == RTC_STATE_ON)
  {
    LCD_DisplayStringLine(LCD_LINE_5, "input and  RTC ON.  ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_5, "input and  RTC OFF.  ");
  }

  LCD_DisplayStringLine(LCD_LINE_8, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_9, "continue.           ");
  while (Menu_ReadKey() == NOKEY)
  {}

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_SLEEP_LP, rtcstate, BIAS_CALIB_OFF);
}

/**
  * @brief  This function configures the system to enter Stop mode for current 
  *         consumption measurement purpose.
  *         STOP Mode
  *         =========   
  *           - RTC could be ON or OFF
  *           - Regulator in LP mode
  *           - LSI, HSI and HSE OFF
  *           - No IWDG
  *           - Current Consumption ~0.5uA
  * @param  None
  * @retval None
  */
void IDD_Measurement_Stop(void)
{
  uint8_t rtcstate;

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString("  STOP  ");

  rtcstate =  IDD_Measurement_SelectRTCState();
  
  LCD_DisplayStringLine(LCD_LINE_0, "MCU will be in STOP "); 
  LCD_DisplayStringLine(LCD_LINE_1, "        Mode        ");
  LCD_DisplayStringLine(LCD_LINE_2, "All peripherals OFF ");
  LCD_DisplayStringLine(LCD_LINE_3, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_4, "input and  RTC ON.  ");

  if(rtcstate == RTC_STATE_ON)
  {
    LCD_DisplayStringLine(LCD_LINE_4, "input and  RTC ON.  ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_4, "input and  RTC OFF.  ");
  }

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_STOP, rtcstate, BIAS_CALIB_OFF); 
}

/**
  * @brief  This function configures the system to enter Standby mode for 
  *         current consumption measurement purpose.
  *         STANDBY Mode
  *         ============
  *           - RTC could be ON or OFF
  *           - IWDG and LSI OFF
  *           - Current Consumption ~0.3uA 
  *           - Automatic Wakeup using RTC clocked by LSE (after 4s)
  * @param  None
  * @retval None
  */
void IDD_Measurement_EnterStandby(void)
{
  uint8_t rtcstate;

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString("STANDBY ");

  rtcstate =  IDD_Measurement_SelectRTCState();

  LCD_DisplayStringLine(LCD_LINE_0, "   MCU will be in   ");
  LCD_DisplayStringLine(LCD_LINE_1, "    STANDBY Mode    ");
  LCD_DisplayStringLine(LCD_LINE_2, "All peripherals OFF ");
  LCD_DisplayStringLine(LCD_LINE_3, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_4, "input and RTC ON.   ");

  if(rtcstate == RTC_STATE_ON)
  {
    LCD_DisplayStringLine(LCD_LINE_4, "input and RTC ON.   ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_4, "input and RTC ON.   ");
  }

  /* STM32L Low Power Mode Selection */
  IDD_Measurement(STM32L_STANDBY, rtcstate, BIAS_CALIB_OFF);
}

/**
  * @brief  Enter the MCU selected low power modes.
  * @param  lpmode: selected MCU low power modes. This parameter can be one of the
  *         following values:
  *              @arg STM32L_RUN: Run mode at 32MHz.
  *              @arg STM32L_RUN_1M: Run mode at 1MHz.
  *              @arg STM32L_RUN_LP: Low power Run mode at 32KHz.  
  *              @arg STM32L_SLEEP: Sleep mode at 16MHz.
  *              @arg STM32L_SLEEP_LP: Low power Sleep mode at 32KHz.
  *              @arg STM32L_STOP: Stop mode with or without RTC.
  *              @arg STM32L_STANDBY: Standby mode with or without RTC.
  * @param  RTCState: RTC peripheral state during low power modes. This parameter
  *         is valid only for STM32L_RUN_LP, STM32L_SLEEP_LP, STM32L_STOP and
  *         STM32L_STANDBY. This parameter can be one of the following values:
  *              @arg RTC_STATE_ON: RTC peripheral is ON during low power modes.
  *              @arg RTC_STATE_OFF: RTC peripheral is OFF during low power modes.
  * @param  CalibrationState: Bias Calibration mode selection state during low 
  *         power modes. 
  *         This parameter can be one of the following values:
  *              @arg BIAS_CALIB_OFF: Bias Calibration mode is selected during 
  *                   low power modes.
  *              @arg BIAS_CALIB_ON: Bias Calibration mode isn't selected during 
  *                   low power modes.
  * @retval None
*/
void IDD_Measurement(uint32_t lpmode, uint8_t RTCState, uint8_t CalibrationState)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  uint16_t mode = STM32L_MODE_LP, adcdata, i;

  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);

  /* Disable Leds toggling */
  Demo_LedShow(DISABLE);

  /* Save the RCC configuration */
  RCC_AHBENR   = RCC->AHBENR;
  RCC_APB2ENR  = RCC->APB2ENR;
  RCC_APB1ENR  = RCC->APB1ENR;

  /* Disable PVD */
  PWR_PVDCmd(DISABLE);

  /* Wait until JoyStick is pressed */
  while (Menu_ReadKey() != NOKEY)
  {}

  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
     Input mode ...*/
  IDD_Measurement_SaveContext();

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  RCC->AHBENR = 0x05;
  RCC->APB2ENR = 0x00;
  RCC->APB1ENR = 0x10000000;  /* PWR APB1 Clock enable */
  
  switch(lpmode)
  {
    /*=========================================================================*
     *                        RUN MODE 32MHz (HSE + PLL)                       *
     *========================================================================*/
    case STM32L_RUN:
    {  
      mode = STM32L_MODE_RUN;
      /* Needed delay to have a correct value on capacitor C25.
        Running NOP during waiting loop will decrease the current consumption. */
      for (i = 0;i < 0xFFFF; i++) 
      {
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
      }
    }
    break;

    /*=========================================================================*
     *                        RUN MODE MSI 1MHz                                *
     *========================================================================*/
    case STM32L_RUN_1M:
    {      
      mode = STM32L_MODE_RUN;
      
      /* Reconfigure the System Clock at MSI 1 MHz */
      SetHCLKToMSI_1MHz();
        
      /* Needed delay to have a correct value on capacitor C25.
        Running NOP during waiting loop will decrease the current consumption. */
      for (i = 0;i < 0x3FFF; i++) 
      {
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
      }
    }
    break;

    /*=========================================================================*
     *                        RUN LOW POWER MODE MSI 32KHz                     *
     *========================================================================*/    
    case STM32L_RUN_LP:
    {                
      if(!RTCState)
      {  
        RCC_LSEConfig(RCC_LSE_OFF);
      }
      else
      {
        if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
        {
          /* RTC Configuration ************************************************/
          /* Reset RTC Domain */
          RCC_RTCResetCmd(ENABLE);
          RCC_RTCResetCmd(DISABLE);
          
          /* Enable the LSE OSC */
          RCC_LSEConfig(RCC_LSE_ON);
          
          /* Wait till LSE is ready */
          while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
          {}
          
          /* Select the RTC Clock Source */
          RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
          
          /* Enable the RTC Clock */
          RCC_RTCCLKCmd(ENABLE);
          
          /* Wait for RTC APB registers synchronisation */
          RTC_WaitForSynchro();
        }        
      }
      /* Configure the System Clock at MSI 32 KHz */
      SetHCLKToMSI_64KHz(); 
      RCC_HCLKConfig(RCC_SYSCLK_Div2); 
      
      /* Clear IDD_CNT_EN pin */
      GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
      
      /* Enter low power run mode */
      PWR_EnterLowPowerRunMode(ENABLE);
      
      /* Waiting wake-up interrupt */
      /* Needed delay to have a correct value on capacitor C25.
        Running NOP during waiting loop will decrease the current consumption. */
      do 
      {
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
        __NOP();  __NOP();    __NOP();  __NOP();
      }while(LowPowerStatus == 0x00);

      /* Exit low power run mode before setting the clock to 32MHz */
      PWR_EnterLowPowerRunMode(DISABLE);
      while(PWR_GetFlagStatus(PWR_FLAG_REGLP) != RESET)
      {
      }  
    }       
    break;

    /*=========================================================================*
     *                        SLEEP MODE HSI 16MHz                             * 
     *========================================================================*/        
    case STM32L_SLEEP:
    {            
      mode = STM32L_MODE_RUN;      
      /* Enable Ultra low power mode */
      PWR_UltraLowPowerCmd(ENABLE);
      
      /* Diable FLASH during SLeep LP */
      FLASH_SLEEPPowerDownCmd(ENABLE);
            
      RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_CLOCK, ENABLE);
      RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_CLOCK, ENABLE);
      RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_CLOCK, ENABLE);
      
      /* Configure the System Clock to 16MHz */
      SetHCLKToHSI();
      
      Demo_SysTickConfig();
      Demo_Delay(5);
      
      /* Request to enter SLEEP mode with regulator on */
      PWR_EnterSleepMode(PWR_Regulator_ON, PWR_STOPEntry_WFI); 
    }   
    break;

    /*=========================================================================*
     *                      SLEEP LOW POWER MODE MSI 32KHz                     *
     *========================================================================*/     
    case STM32L_SLEEP_LP:
    {            
      if(!RTCState)
      {  
        RCC_LSEConfig(RCC_LSE_OFF);
      }
      else
      {
        if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
        {
          /* RTC Configuration ************************************************/
          /* Reset RTC Domain */
          RCC_RTCResetCmd(ENABLE);
          RCC_RTCResetCmd(DISABLE);
          
          /* Enable the LSE OSC */
          RCC_LSEConfig(RCC_LSE_ON);
          
          /* Wait till LSE is ready */
          while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
          {}
          
          /* Select the RTC Clock Source */
          RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
          
          /* Enable the RTC Clock */
          RCC_RTCCLKCmd(ENABLE);
          
          /* Wait for RTC APB registers synchronisation */
          RTC_WaitForSynchro();
        }        
      }
      
      /* Enable Ultra low power mode */
      PWR_UltraLowPowerCmd(ENABLE);
      
      /* Diable FLASH during SLeep LP */
      FLASH_SLEEPPowerDownCmd(ENABLE);
      
      /* Disable HSI clock before entering Sleep LP mode */ 
      RCC_HSICmd(DISABLE);
      /* Disable HSE clock */
      RCC_HSEConfig(RCC_HSE_OFF);
      /* Disable LSI clock */
      RCC_LSICmd(DISABLE);
      
      RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_CLOCK, ENABLE);
      RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_CLOCK, ENABLE);
      RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_CLOCK, ENABLE);
      
      /* Clear IDD_CNT_EN pin */
      GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
      
      /* Reconfigure the System Clock at MSI 64 KHz */
      SetHCLKToMSI_64KHz();
      RCC_HCLKConfig(RCC_SYSCLK_Div2); 
      
      /* Request to enter SLEEP mode with regulator low power */
      PWR_EnterSleepMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);        
    }
    break;
    
    /*=========================================================================*
     *                          STOP LOW POWER MODE                            *
     *========================================================================*/    
    case STM32L_STOP:
    {                 
      /* Enable Ultra low power mode */
      PWR_UltraLowPowerCmd(ENABLE);

      if(!RTCState)
      {  
        RCC_LSEConfig(RCC_LSE_OFF);
      }
      else
      {
        if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
        {
          /* RTC Configuration ************************************************/
          /* Reset RTC Domain */
          RCC_RTCResetCmd(ENABLE);
          RCC_RTCResetCmd(DISABLE);
          
          /* Enable the LSE OSC */
          RCC_LSEConfig(RCC_LSE_ON);
          
          /* Wait till LSE is ready */
          while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
          {}
          
          /* Select the RTC Clock Source */
          RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
          
          /* Enable the RTC Clock */
          RCC_RTCCLKCmd(ENABLE);
          
          /* Wait for RTC APB registers synchronisation */
          RTC_WaitForSynchro();
        }        
      }
      
      /* Clear IDD_CNT_EN pin */
      GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
      
      /* Request to enter STOP mode with regulator in low power */
      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
      
      /* Reset the counter by programming IDD_CNT_EN High in less than 70ms after
      the wakeup to avoid 1Kohm to be connected later on VDD_MCU */
      GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
      
      /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
         as system clock source (HSE and PLL are disabled in STOP mode) */
      IDD_Measurement_SYSCLKConfig_STOP();      
    }
    break;

    /*=========================================================================*
     *                          STANDBY LOW POWER MODE                         *
     *========================================================================*/        
    case STM32L_STANDBY:
    {      
      if (RTC_GetFlagStatus(RTC_FLAG_INITS) == RESET)
      {
        /* RTC Configuration **************************************************/
        /* Reset RTC Domain */
        RCC_RTCResetCmd(ENABLE);
        RCC_RTCResetCmd(DISABLE);
        
        /* Enable the LSE OSC */
        RCC_LSEConfig(RCC_LSE_ON);
        
        /* Wait till LSE is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {}
        
        /* Select the RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        
        /* Enable the RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        
        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
      } 
      
      RTC_OutputTypeConfig(RTC_OutputType_PushPull);
      RTC_OutputConfig(RTC_Output_WakeUp, RTC_OutputPolarity_High);  
      
      /* To configure PC13 WakeUP output */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;  
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
      GPIO_Init(GPIOC, &GPIO_InitStructure); 
      GPIO_PinAFConfig(GPIOC, GPIO_PinSource13, GPIO_AF_RTC_AF1);
      
      PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
      
      PWR_UltraLowPowerCmd(ENABLE); 
      
      RTC_ClearFlag(RTC_FLAG_WUTF | RTC_FLAG_ALRBF | RTC_FLAG_ALRAF | RTC_FLAG_TAMP1F | RTC_FLAG_TSF); 
      RTC_ITConfig(RTC_IT_WUT, DISABLE);

      if(!RTCState)
      {  
        RCC_LSEConfig(RCC_LSE_OFF);
      }
      
      /* Clear Wake Up flag */
      PWR_ClearFlag(PWR_FLAG_WU);  
      
      /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
      PWR_EnterSTANDBYMode();
    }     
    break; 
  }

  /* Configure the System Clock to 32MHz */
  SetHCLKTo32();
      
  /* Reset lowpower status variable*/  
  LowPowerStatus = 0x00;
      
  RCC->AHBENR = RCC_AHBENR;
  RCC->APB2ENR = RCC_APB2ENR;
  RCC->APB1ENR = RCC_APB1ENR;
      
  /* Reset the counter by programming IDD_CNT_EN High in less than 70ms after
     the wakeup to avoid 1Kohm to be connected later on VDD_MCU */
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
  
  /* Measure the Voltage using the ADC */    
  adcdata = IDD_Measurement_ADC_ReadValue();
  
  /* Write the ADC converted value in the DATA EEPROM memory for Bias Measurement */
  if(CalibrationState == BIAS_CALIB_ON)
  {
    /* Unlock EEPROM write access*/
    DATA_EEPROM_Unlock();
  
    /* Store the value in EEPROM for application needs */
    DATA_EEPROM_ProgramHalfWord(DATA_EEPROM_BIAS_ADDR, adcdata);
  
    /* Lock back EEPROM write access */
    DATA_EEPROM_Lock(); 
  }

  IDD_Measurement_ADC_DisplayValue(adcdata, mode);
    
  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);
      
  /* Enable PVD */
  PWR_PVDCmd(ENABLE);
      
  /* Restore Demonstration Context. */
  IDD_Measurement_RestoreContext();
  
  LCD_SetBackColor(LCD_COLOR_GREEN);  
  LCD_DisplayStringLine(LCD_LINE_6, "STM32L LowPower Mode");          
  LCD_DisplayStringLine(LCD_LINE_7, Str);

  LCD_DisplayStringLine(LCD_LINE_8, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_9, "continue.           ");  

  /* Wait until Joystick pressed. */
  while (Menu_ReadKey() == NOKEY)
  {} 
  
  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  LCD_Clear(LCD_COLOR_WHITE);
  
  LCD_GLASS_DisplayString(" STM32L ");

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu(); 
}

/**
  * @brief  ADC configuration for automatic IDD measurement.
  * @param  None
  * @retval None
  */
void IDD_Measurement_ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOA Clock */  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure IDD Measurement pin (ADC Channelxx) as analog input -----------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);
  
/* ADC1 configuration --------------------------------------------------------*/
  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);

  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}
    
/* Enable ADC clock ----------------------------------------------------------*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

/* de-initialize ADC ---------------------------------------------------------*/
  ADC_DeInit(ADC1);

/*  ADC configured as follows:
  - NbrOfChannel = 1 - ADC_Channel_5
  - Mode = Single ConversionMode(ContinuousConvMode Enabled)
  - Resolution = 12Bits
  - Prescaler = /1
  - Sampling time 192 */

    /* ADC Configuration */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel4 configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_192Cycles);

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Wait until ADC1 ON status */
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {
  }
}

/**
  * @brief  Internal VREF measurement using ADC Channel 17.
  * @param  None
  * @retval Internal VREF ADC converted value.
  */ 
uint16_t IDD_Measurement_ADC_Supply(void)
{
  uint16_t adcdata = 0x0, i = 0x00;

  /* ADC1 configuration ------------------------------------------------------*/
  IDD_Measurement_ADC_Config();
 
  ADC_TempSensorVrefintCmd(ENABLE);

  /* ADC1 regular channel 17 for VREF configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_192Cycles);

  /* Start ADC conversion by software */
  ADC_SoftwareStartConv(ADC1);  

  /* Initialize result */
  for(i = 4; i > 0; i--)
  {
    /* Wait until end-of-conversion */
    while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0)
    {
    }

    /* Read ADC conversion result */
    adcdata += ADC_GetConversionValue(ADC1);
  }
	
  /* Deinitialize ADC */
  ADC_TempSensorVrefintCmd(DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
  
  return (adcdata >> 2);
}

/**
  * @brief  Compute the VDD voltage using the VREF ADC converted value.
  * @param  None   
  * @retval VDD voltage value.
  */
uint32_t IDD_Measurement_VDD(void)
{
  uint16_t adcdata;
  
  /* Read the VREF value on ADC converted value. */
  adcdata = IDD_Measurement_ADC_Supply();	
  
  /* Compute the VDD in mV using the theorical VREF value */
  adcdata = (1224 * 4096) / adcdata;
	
  return adcdata;
}

/**
  * @brief  Read the ADC converted value.
  * @param  None
  * @retval ADC converted value.
  */
uint32_t IDD_Measurement_ADC_ReadValue(void)
{
  uint16_t adcvalue = 0x0, i = 0x00;

  /* ADC1 configuration ------------------------------------------------------*/
  IDD_Measurement_ADC_Config();

  /* Start ADC1 Conversion using Software trigger */
  ADC_SoftwareStartConv(ADC1);
     
  for (i = 4; i > 0; i--)
  {  
    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}
    /* Read ADC conversion result */
    adcvalue += ADC_GetConversionValue(ADC1);
  }
  return  (adcvalue >> 2);
}

/**
  * @brief  Compute the IDD value using the ADC converted value and format its
  *         display.
  * @param  CurrentData: Voltage ADC converted value.
  * @param  mode: Current STM32L selected low power modes and adapt the corresponding
  *         computation formula. This parameter can be one of the following values:
  *             @arg STM32L_MODE_RUN: used for STM32L_RUN, STM32L_RUN_1M and
  *                  STM32L_SLEEP low power modes.        
  *             @arg STM32L_MODE_LP: used for STM32L_RUN_LP, STM32L_SLEEP_LP,
  *                  STM32L_STOP and STM32L_STANDBY low power modes.    
  * @retval None
  */
void IDD_Measurement_ADC_DisplayValue(uint32_t CurrentData, uint32_t mode)
{
  uint32_t vdd, idd;
  
  if((mode != STM32L_MODE_RUN) && (mode != STM32L_MODE_BIAS))
  {
    /* Substract the Bias current value already stored in the DATA EEPROM */  
    if (CurrentData > (*(__IO uint16_t *)DATA_EEPROM_BIAS_ADDR))
    {
      CurrentData -= (*(__IO uint16_t *)DATA_EEPROM_BIAS_ADDR);  
    }
  }   
  /* Calculate voltage value in uV over capacitor C25 for IDD measurement */
  vdd = (uint32_t)((uint32_t)CurrentData * (uint32_t)ADC_CONVERT_RATIO);
  
  /* Calculate the current consumption */
  idd = (uint32_t) (uint32_t)((vdd * EVAL_RESISTOR_RATIO ) / EVAL_MAX9938_GAIN);

  if(mode != STM32L_MODE_RUN)
  {
    idd *= 1000;
    idd /= (EVAL_RESISTOR_R35 + EVAL_RESISTOR_R36 + EVAL_RESISTOR_STT5PF20V);
  }
  else
  {
    idd /= EVAL_RESISTOR_R36;
  }
  
  IDD_Measurement_Convert(Str, idd, mode);  
}

/**
  * @brief  Convert the IDD current value to a string.
  * @param  ptr: pointer the global string holding text to be displayed.
  * @param  idd: IDD measured value.
  * @param  mode: Current STM32L selected low power modes and adapt the corresponding
  *         computation formula. This parameter can be one of the following values:
  *             @arg STM32L_MODE_RUN: used for STM32L_RUN, STM32L_RUN_1M and
  *                  STM32L_SLEEP low power modes.        
  *             @arg STM32L_MODE_LP: used for STM32L_RUN_LP, STM32L_SLEEP_LP,
  *                  STM32L_STOP and STM32L_STANDBY low power modes.    
  * @retval None
  */
void IDD_Measurement_Convert(uint8_t *ptr, uint32_t idd, uint32_t mode)
{
  uint8_t current1000, current100, current10, current1, current01, current001, digit = 'u';
  
  /* x current value */
  current1000 = (uint8_t)(idd / 100000);
  /* Hundreds current value */
  current100 = (uint8_t)((idd % 100000) / 10000);
  /* Tens current value */
  current10 = (uint8_t)((idd % 10000) / 1000);
  /* 1mA current value */
  current1 = (uint8_t)((idd % 1000) / 100);
  /* 0.1 mA current value */
  current01 = (uint8_t)((idd % 100 ) / 10);
  /* 0.01 mA current value */
  current001 =  (uint8_t)(idd % 10);

  if(mode == STM32L_MODE_RUN)
  {
    digit = 'm';
  }
  /* Format the string */
  sprintf ((char*)ptr, "   IDD: %d%d%d,%d%d%d%cA   ", current1000, current100, current10, current1, current01, current001, digit);   
}

/**
  * @brief  Displays the Standby IDD value after woken-up from Standby mode.
  * @param  None
  * @retval None
  */
void IDD_Measurement_DisplayStandby(void)
{   
  while (Menu_ReadKey() != NOKEY)
  {}
 
  IDD_Measurement_ADC_DisplayValue((*(__IO uint16_t *)DATA_EEPROM_STBY_ADDR), STM32L_MODE_LP);
  
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
    
  LCD_DisplayStringLine(LCD_LINE_1, "    Standby Mode    ");
  LCD_DisplayStringLine(LCD_LINE_3, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_6, "continue.           ");
  
  while (Menu_ReadKey() == NOKEY)
  {}

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);
}

/**
  * @brief  Selects the RTC State during low power modes. RTC could be ON or OFF.
  * @param  None
  * @retval RTC state during low power modes.
  *            - RTC_STATE_ON: RTC ON during low power modes.
  *            - RTC_STATE_OFF: RTC OFF during low power modes.    
  */
uint32_t IDD_Measurement_SelectRTCState(void)
{
  uint8_t pressedkey = NOKEY;
  
  LCD_DisplayStringLine(LCD_LINE_0, "   RTC ON or OFF    ");
  LCD_DisplayStringLine(LCD_LINE_1, "ON: Press SEL       ");
  LCD_DisplayStringLine(LCD_LINE_2, "OFF: Press Any Key  ");

  pressedkey = Menu_ReadKey();
  while (pressedkey == NOKEY)
  {
    pressedkey = Menu_ReadKey();
  }

  if(pressedkey == SEL)
  {
    return RTC_STATE_ON;   
  }
  else
  {
    return RTC_STATE_OFF;    
  }  
}

/**
  * @brief  Save Demonstration context (GPIOs Configurations, peripherals,...).
  * @param  None
  * @retval None
  */
void IDD_Measurement_SaveContext(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Deinitialize all peripherals used in the Demo */
  DAC_DeInit();
  ADC_DeInit(ADC1);
  DMA_DeInit(DMA1_Channel2);
  SPI_DeInit(SPI1);
  TIM_DeInit(TIM2);
  EXTI_DeInit();
  TIM_DeInit(TIM6);
  LCD_DeInit();
  SYSCFG_DeInit();
  I2C_DeInit(I2C1);
  COMP_DeInit();
    
  GPIOA_MODER = GPIOA->MODER;
  GPIOB_MODER = GPIOB->MODER;
  GPIOC_MODER = GPIOC->MODER;
  GPIOD_MODER = GPIOD->MODER;
  GPIOE_MODER = GPIOE->MODER;
  GPIOH_MODER = GPIOH->MODER;

  GPIOA_PUPDR = GPIOA->PUPDR;
  GPIOB_PUPDR = GPIOB->PUPDR;
  GPIOC_PUPDR = GPIOC->PUPDR;
  GPIOD_PUPDR = GPIOD->PUPDR;
  GPIOE_PUPDR = GPIOE->PUPDR;
  GPIOH_PUPDR = GPIOH->PUPDR;

  /* Configure all GPIO port pins in Analog Input mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOH, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
  
  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(IDD_WAKEUP_EXTI_PORTSOURCE, IDD_WAKEUP_EXTI_PINSOURCE);

  /* Configure IDD_WAKEUP pin as input pull-down -----------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_WAKEUP_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_WAKEUP_GPIO_PORT, &GPIO_InitStructure);

  /* Configure the IDD_WAKEUP EXTI Line */
  EXTI_ClearITPendingBit(IDD_WAKEUP_EXTI_LINE);

  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IDD_WAKEUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
}

/**
  * @brief  Restore Demonstration context (GPIOs Configurations, peripherals,...).
  * @param  None
  * @retval None
  */
void IDD_Measurement_RestoreContext(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
    
  GPIOA->MODER = GPIOA_MODER;
  GPIOB->MODER = GPIOB_MODER;
  GPIOC->MODER = GPIOC_MODER;
  GPIOD->MODER = GPIOD_MODER;
  GPIOE->MODER = GPIOE_MODER;
  GPIOH->MODER = GPIOH_MODER;

  GPIOA->PUPDR = GPIOA_PUPDR;
  GPIOB->PUPDR = GPIOB_PUPDR;
  GPIOC->PUPDR = GPIOC_PUPDR;
  GPIOD->PUPDR = GPIOD_PUPDR;
  GPIOE->PUPDR = GPIOE_PUPDR;
  GPIOH->PUPDR = GPIOH_PUPDR;

  /* At this stage the system has resumed from STOP mode ---------------------*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IDD_WAKEUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /*!< LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /*!< Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /*!< LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* LCD GLASS Initialization */
  LCD_GLASS_Init();

  /* GPIO Configuration */
  Demo_GPIOConfig();

  /* Interrupt Configuration */
  Demo_InterruptConfig();

  /* Configure the systick */
  Demo_SysTickConfig();

  /*------------------- Drivers Initialization -------------------------------*/
  /* Initialize the LEDs toggling */
  Demo_LedShowInit();

  /* Initialize the LCD */
  STM32L152_LCD_Init();

  /* If HSE is not detected at program startup */
  if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
    /* Generate NMI exception */
    SCB->ICSR |= SCB_ICSR_NMIPENDSET;
  }

  /* Initialize Wave player application */
  WavePlayer_Init();

  /* Initialize the Thermometer application */
  LM75_Init();

  /* Enable Leds toggling */
  Demo_LedShow(ENABLE);

  /* COMP2 Configuration */
  Demo_COMPConfig();
  
  /* Enable Leds toggling */
  Demo_LedShow(ENABLE);
  Demo_SysTickConfig();    
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
void IDD_Measurement_SYSCLKConfig_STOP(void)
{
  ErrorStatus HSEStartUpStatus;

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x0C)
    {}
  }
}

/**
  * @brief  Bias measurement using IDD stop mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Calibrate(void)
{
  uint16_t pressedkey = 0x00;

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);
  
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, "Current Bias Value  "); 
  LCD_DisplayStringLine(LCD_LINE_1, "stored in DATAEEPROM");
  IDD_Measurement_ADC_DisplayValue((*(__IO uint16_t *)DATA_EEPROM_BIAS_ADDR), STM32L_MODE_BIAS);
  LCD_DisplayStringLine(LCD_LINE_2, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "  To Start Bias     ");
  LCD_DisplayStringLine(LCD_LINE_6, "measurement press   ");  
  LCD_DisplayStringLine(LCD_LINE_7, "SEL push-button.    ");    
  LCD_DisplayStringLine(LCD_LINE_8, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_9, "exit.               ");

  pressedkey = Menu_ReadKey();
  while (pressedkey == NOKEY)
  {
    pressedkey = Menu_ReadKey();
  }
   
  if (pressedkey != SEL)
  {
    /* Enable the JoyStick interrupts */
    Demo_IntExtOnOffConfig(ENABLE);
    /* Display the previous menu */
    Menu_DisplayMenu();
    return;
  }
  
  LCD_Clear(LCD_COLOR_WHITE);  
  LCD_DisplayStringLine(LCD_LINE_0, "MCU will be in STOP "); 
  LCD_DisplayStringLine(LCD_LINE_1, "  Bias Measurement  ");
  LCD_DisplayStringLine(LCD_LINE_2, " Make sure that JP4 ");
  LCD_DisplayStringLine(LCD_LINE_3, " in position 1<->2  ");
  
  LCD_DisplayStringLine(LCD_LINE_7, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_8, "continue.           ");
  
  LCD_GLASS_DisplayString("  BIAS  ");
  
  while (Menu_ReadKey() == NOKEY)
  {}
  
  IDD_Measurement(STM32L_STOP, RTC_STATE_OFF, BIAS_CALIB_ON);
}

/**
  * @brief  Reset the Bias measurement value stored in Data EEPROM.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Bias_Reset(void)
{
  uint8_t pressedkey = 0;
  
  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);
  
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, "Current Bias Value  "); 
  LCD_DisplayStringLine(LCD_LINE_1, "stored in DATAEEPROM");
  IDD_Measurement_ADC_DisplayValue((*(__IO uint16_t *)DATA_EEPROM_BIAS_ADDR), STM32L_MODE_BIAS);
  LCD_DisplayStringLine(LCD_LINE_2, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "  To reset Bias     ");
  LCD_DisplayStringLine(LCD_LINE_6, "measurement press   ");  
  LCD_DisplayStringLine(LCD_LINE_7, "SEL push-button.    ");    
  LCD_DisplayStringLine(LCD_LINE_8, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_9, "exit.               ");

  pressedkey = Menu_ReadKey();
  while (pressedkey == NOKEY)
  {
    pressedkey = Menu_ReadKey();
  }
   
  if (pressedkey != SEL)
  {
    /* Enable the JoyStick interrupts */
    Demo_IntExtOnOffConfig(ENABLE);
    /* Display the previous menu */
    Menu_DisplayMenu();
    return;
  }
 
  LCD_Clear(LCD_COLOR_WHITE);

  /* Unlock EEPROM write access. */
  DATA_EEPROM_Unlock();
  
  /* Store the value in EEPROM for application needs. */
  DATA_EEPROM_ProgramHalfWord(DATA_EEPROM_BIAS_ADDR, 0x0);
  
  /* Lock back EEPROM write access. */
  DATA_EEPROM_Lock();

  LCD_DisplayStringLine(LCD_LINE_1, "  Bias Measurement  ");
  LCD_DisplayStringLine(LCD_LINE_2, " Bias Value is reset");
  LCD_DisplayStringLine(LCD_LINE_3, " to 0x0.            ");
    
  LCD_DisplayStringLine(LCD_LINE_7, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_8, "continue.           ");

  LCD_GLASS_DisplayString(" STM32L ");
  while (Menu_ReadKey() == NOKEY)
  {}

  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu();
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

