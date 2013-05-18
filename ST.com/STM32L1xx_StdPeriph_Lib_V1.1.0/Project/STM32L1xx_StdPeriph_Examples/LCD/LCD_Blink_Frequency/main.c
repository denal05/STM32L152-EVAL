/**
  ******************************************************************************
  * @file    LCD/LCD_Blink_Frequency/main.c 
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

#ifdef USE_STM32L152D_EVAL 
  #include "stm32l152d_eval.h"
  #include "stm32l152d_eval_glass_lcd.h"
#elif defined USE_STM32L152_EVAL 
  #include "stm32l152_eval.h"
  #include "stm32l152_eval_glass_lcd.h"
#endif 

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup LCD_Blink_Frequency
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RCC_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
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
       
  /* System clocks configuration ---------------------------------------------*/
  RCC_Config();
    
  /* LCD GLASS Initialization */
  LCD_GLASS_Init();
  
  /* Configure the STM32L1xx-EVAL KEY Push button */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);
  
#ifdef USE_STM32L152D_EVAL 
  /* Configure all LCD segments and Coms to blink at frequency LCDCLK/32 */
  LCD_BlinkConfig(LCD_BlinkMode_AllSEG_AllCOM, LCD_BlinkFrequency_Div32); 
  
  /* Display the "BLF0032" message on the LCD GLASS */
  LCD_GLASS_DisplayString("BLF0032");
#elif defined USE_STM32L152_EVAL 
  /* Configure all LCD segments and Coms to blink at frequency LCDCLK/16 */
  LCD_BlinkConfig(LCD_BlinkMode_AllSEG_AllCOM, LCD_BlinkFrequency_Div16); 
  
  /* Display the "BLF0016" message on the LCD GLASS */
  LCD_GLASS_DisplayString("BLF0016");
#endif 
  
  /* Infinite loop */
  while (1)
  {    
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Config(void)
{  
  /* Enable peripheral clocks ------------------------------------------------*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /*!< LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /*!< Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /*!< LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
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
