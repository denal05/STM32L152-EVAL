/**
  ******************************************************************************
  * @file    CortexM3/BitBand/main.c 
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

#ifdef USE_STM32L152D_EVAL 
  #include "stm32l152d_eval_lcd.h"
#elif defined USE_STM32L152_EVAL 
  #include "stm32l152_eval_lcd.h"
#endif

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM3_BitBand
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RAM_BASE       0x20000000
#define RAM_BB_BASE    0x22000000
 
/* Private macro -------------------------------------------------------------*/
#define  Var_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 0)
   
#define Var_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define Var_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)))
          
/* Private variables ---------------------------------------------------------*/
__IO uint32_t Var, VarAddr = 0, VarBitValue = 0;

/* Private function prototypes -----------------------------------------------*/
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
  LCD_DisplayStringLine( Line2, " CortexM3 BitBand   " );
  LCD_DisplayStringLine( Line3, "                    " );
#endif 

  Var = 0x00005AA5;

/* A mapping formula shows how to reference each word in the alias region to a 
   corresponding bit in the bit-band region. The mapping formula is:
   bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number + 4)

where:
   - bit_word_addr: is the address of the word in the alias memory region that 
                    maps to the targeted bit.
   - bit_band_base is the starting address of the alias region
   - byte_offset is the number of the byte in the bit-band region that contains 
     the targeted bit
   - bit_number is the bit position (0-7) of the targeted bit */

/* Get the variable address --------------------------------------------------*/ 
  VarAddr = (uint32_t)&Var; 

/* Modify variable bit using bit-band access ---------------------------------*/
  /* Modify Var variable bit 0 -----------------------------------------------*/
  Var_ResetBit_BB(VarAddr, 0);  /* Var = 0x00005AA4 */
  Var_SetBit_BB(VarAddr, 0);    /* Var = 0x00005AA5 */
  
  /* Modify Var variable bit 11 ----------------------------------------------*/
  Var_ResetBit_BB(VarAddr, 11);             /* Var = 0x000052A5 */
  /* Get Var variable bit 11 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 11); /* VarBitValue = 0x00000000 */
  
  Var_SetBit_BB(VarAddr, 11);               /* Var = 0x00005AA5 */
  /* Get Var variable bit 11 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 11);    /* VarBitValue = 0x00000001 */
  
  /* Modify Var variable bit 31 ----------------------------------------------*/
  Var_SetBit_BB(VarAddr, 31);               /* Var = 0x80005AA5 */
  /* Get Var variable bit 31 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 31); /* VarBitValue = 0x00000001 */
    
  Var_ResetBit_BB(VarAddr, 31);             /* Var = 0x00005AA5 */
  /* Get Var variable bit 31 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 31); /* VarBitValue = 0x00000000 */
        
  while (1)
  {
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
