/**
  ******************************************************************************
  * @file    USART/HyperTerminal_Interrupt/main.c 
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
  #include "stm32l152d_eval.h"
  #include "stm32l152d_eval_lcd.h"
  #define EVAL_COMX EVAL_COM1
  #define COMX COM1
  #define EVAL_COMX_IRQn EVAL_COM1_IRQn
#elif defined USE_STM32L152_EVAL 
  #include "stm32l152_eval.h"
  #include "stm32l152_eval_lcd.h"
  #define EVAL_COMX EVAL_COM2
  #define COMX COM2
  #define EVAL_COMX_IRQn EVAL_COM2_IRQn
#endif 

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_HyperTerminal_Interrupt
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
extern uint8_t NbrOfDataToTransfer;
extern uint8_t NbrOfDataToRead;
extern __IO uint8_t TxCounter; 
extern __IO uint16_t RxCounter; 
/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
  
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
#endif 
  LCD_Clear( Blue );
  LCD_SetBackColor( Blue );
  LCD_SetTextColor( White );
  LCD_DisplayStringLine( Line0, " DELAY 5 SECONDS... " );
  for(uint32_t Counter = 0; Counter < 0xFFFFFF; Counter++);
  LCD_Clear( Blue );
  LCD_DisplayStringLine( Line0, "   STM32L152-EVAL   " );
  LCD_DisplayStringLine( Line1, " StdPeriphLibV1.1.0 " );
  LCD_DisplayStringLine( Line2, " USART/HyperTerm_IRQ" );
  LCD_DisplayStringLine( Line3, "                    " );
  LCD_DisplayStringLine( Line4, "INSTRUCTIONS:       " );
  LCD_DisplayStringLine( Line5, "Connect CN5 (USART3)" );
  LCD_DisplayStringLine( Line6, "@9600 , 8-bit word  " );
  LCD_DisplayStringLine( Line7, "length, 2 stop bits," );
  LCD_DisplayStringLine( Line8, "odd parity, no hw   " );
  LCD_DisplayStringLine( Line9, "flow control        " );
  
  /* NVIC configuration */
  NVIC_Config();
  
/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - Two Stop Bit
        - Odd parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_2;
  USART_InitStructure.USART_Parity = USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COMX, &USART_InitStructure);

  /* Enable the EVAL_COMX Transmoit interrupt: this interrupt is generated when the 
     EVAL_COM1 transmit data register is empty */  
  USART_ITConfig(EVAL_COMX, USART_IT_TXE, ENABLE);
  
  /* Wait until EVAL_COMX send the TxBuffer */
  while(TxCounter < NbrOfDataToTransfer)
  {}
  
  /* The software must wait until TC=1. The TC flag remains cleared during all data
     transfers and it is set by hardware at the last frame’s end of transmission*/
  while (USART_GetFlagStatus(EVAL_COMX, USART_FLAG_TC) == RESET)
  {}
 
  /* Enable the EVAL_COMX Receive interrupt: this interrupt is generated when the 
     EVAL_COMX receive data register is not empty */
  USART_ITConfig(EVAL_COMX, USART_IT_RXNE, ENABLE);
  
  /* Wait until EVAL_COMX receive the RxBuffer */
  while(RxCounter < NbrOfDataToRead)
  {}

  while (1)
  {
  }
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EVAL_COMX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
