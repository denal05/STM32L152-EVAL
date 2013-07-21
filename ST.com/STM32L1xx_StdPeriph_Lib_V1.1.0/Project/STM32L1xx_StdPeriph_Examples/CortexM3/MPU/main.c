/**
  ******************************************************************************
  * @file    CortexM3/MPU/main.c 
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
#include "stm32_mpu.h"

#ifdef USE_STM32L152D_EVAL 
  #include "stm32l152d_eval.h"
  #include "stm32l152d_eval_lcd.h"
#elif defined USE_STM32L152_EVAL 
  #include "stm32l152_eval.h"
  #include "stm32l152_eval_lcd.h"
#endif 

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM3_MPU
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ACCESS_PERMISSION

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay (uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
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
#ifdef USE_STM32L152D_EVAL 
  STM32L152D_LCD_Init();
#elif defined USE_STM32L152_EVAL 
  STM32L152_LCD_Init();	
  LCD_Clear( Blue );
  LCD_SetBackColor( Blue );
  LCD_SetTextColor( White );
  LCD_DisplayStringLine( Line0, "   STM32L152-EVAL   " );
  LCD_DisplayStringLine( Line1, " StdPeriphLibV1.1.0 " );
  LCD_DisplayStringLine( Line2, "      CortexM3      " );
  LCD_DisplayStringLine( Line3, "         MPU        " );
  LCD_DisplayStringLine( Line4, "                    " );
#endif 

  /* Configure LED1 and LED2 */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  
  /* Set MPU regions */
  MPU_Config();

 #ifdef ACCESS_PERMISSION
  MPU_AccessPermConfig();
 #endif  

  /* Infinite loop */
  while (1)
  {
    /* Toggle LED1 */
    STM_EVAL_LEDToggle(LED2); 

    /* Insert a delay */
    Delay(0x7FFFF);
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
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
