/**
  ******************************************************************************
  * @file    DMA/FLASH_RAM/main.c 
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

/** @addtogroup FLASH_RAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE  32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef  DMA_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
__IO uint32_t EndOfTransfer = 0;
__IO TestStatus TransferStatus = FAILED;
const uint32_t SrcBuffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
uint32_t DstBuffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);

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
  LCD_DisplayStringLine( Line2, "    DMA/FLASH_RAM   " );
  LCD_DisplayStringLine( Line3, "                    " );
  LCD_DisplayStringLine( Line4, "This example shows  " );
  LCD_DisplayStringLine( Line5, "how to use a DMA ch." );
  LCD_DisplayStringLine( Line6, "to transfer a word  " );
  LCD_DisplayStringLine( Line7, "buffer from FLASH to" );
  LCD_DisplayStringLine( Line8, "embedded SRAM memory" );
#endif 

  /* DMA1 channel6 configuration ---------------------------------------------*/
  /* Enable DMA1 clock --------------------------------------------------------*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SrcBuffer;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DstBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);

  /* Enable DMA1 Channel6 Transfer Complete interrupt */
  DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

  /* Enable DMA1 channel6 IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA1 Channel6 transfer */
  DMA_Cmd(DMA1_Channel6, ENABLE);

  /* Wait the end of transmission */
  while (EndOfTransfer == 0)
  {
  }

  /* Check if the destination "DstBuffer" and source "SrcBuffer" buffers are equal */
  TransferStatus = Buffercmp(SrcBuffer, DstBuffer, BUFFER_SIZE);
  /* TransferStatus = PASSED, destination and source buffers are the same */
  /* TransferStatus = FAILED, destination and source buffers are different */

  while (1)
  {
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAILED;
    }
    
    pBuffer++;
    pBuffer1++;
  }

  return PASSED;  
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
