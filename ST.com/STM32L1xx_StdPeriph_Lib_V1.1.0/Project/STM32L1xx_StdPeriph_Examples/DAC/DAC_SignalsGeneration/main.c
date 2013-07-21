/**
  ******************************************************************************
  * @file    DAC/DAC_SignalsGeneration/main.c 
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

#if defined USE_STM32L152D_EVAL
  #include "stm32l152d_eval.h"
  #include "stm32l152d_eval_lcd.h"
#elif defined USE_STM32L152_EVAL
  #include "stm32l152_eval.h"
  #include "stm32l152_eval_lcd.h"
#endif

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DAC_Signals_Generation
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R2_Address      0x40007414
#define DAC_DHR8R1_Address       0x40007410

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
DAC_InitTypeDef            DAC_InitStructure;
DMA_InitTypeDef            DMA_InitStructure;
const uint16_t Sine12bit[32] = {
  2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056,
  4095, 4056, 3939, 3750, 3498, 3185, 2831, 2447,
  2047, 1647, 1263,  909,  599,  344,  155,   38,
     0,   38,  155,  344,  599,  909, 1263, 1647
};
const uint16_t Sine12bitX2[32] = {
  2047, 2831, 3498, 3939, 4095, 3939, 3498, 2831,
  2047, 1263,  599,  155,    0,  155,  599, 1263,
  2047, 2831, 3498, 3939, 4095, 3939, 3498, 2831, 
  2047, 1263,  599,  155,    0,  155,  599, 1263
};
const uint16_t Triangle12bit[32] = {
  2047, 2831, 3498, 3939, 4095, 3939, 3498, 2831,
  2047, 1263,  599,  155,    0,  155,  599, 1263,
  2047, 2831, 3498, 3939, 4095, 3939, 3498, 2831, 
  2047, 1263,  599,  155,    0,  155,  599, 1263
};
const uint8_t Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};
uint8_t Idx = 0;
__IO uint8_t  SelectedWavesForm = 1;
__IO uint8_t WaveChange = 1; 

/* Private function prototypes -----------------------------------------------*/
void DAC_PreConfig(void);

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
  LCD_DisplayStringLine( Line2, "   DAC Signals Gen. " );
  LCD_DisplayStringLine( Line3, "                    " );
  LCD_DisplayStringLine( Line4, "INSTRUCTIONS:       " );
  LCD_DisplayStringLine( Line5, "Use KEY button, open" );
  LCD_DisplayStringLine( Line6, "JP14, connect pins  " );
  LCD_DisplayStringLine( Line7, "PA4 (DAC_OUT1) and  " );
  LCD_DisplayStringLine( Line8, "PA5 (DAC_OUT2) to an" );
  LCD_DisplayStringLine( Line9, "oscilloscope.       " );
#endif 

  /* Preconfiguration before using DAC----------------------------------------*/
  DAC_PreConfig();

  /* TIM2 Configuration ------------------------------------------------------*/
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 0xFF;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
  
  /* Configures Button GPIO and EXTI Line */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);
  
  while (1)
  {
    /* If the wave form is changed */
    if (WaveChange == 1)
    {  
      /* Switch the selected waves forms according the Button status */
      if (SelectedWavesForm == 1)
      {
          /* The sine wave and the escalator wave has been selected */
          /* Sine Wave generator ---------------------------------------------*/
          DAC_DeInit(); 

          /* DAC channel1 and channel2 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

          /* DMA init */
          DMA_DeInit(DMA1_Channel3); 
          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R2_Address;
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
          DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
          DMA_InitStructure.DMA_BufferSize = 32;
          DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
          DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
          DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
          DMA_InitStructure.DMA_Priority = DMA_Priority_High;
          DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
          DMA_Init(DMA1_Channel3, &DMA_InitStructure);

          /* Enable DMA1 Channel 3 */
          DMA_Cmd(DMA1_Channel3, ENABLE);

          /* DAC Channel 2 Init */
          DAC_Init(DAC_Channel_2, &DAC_InitStructure);

          /* Enable DAC Channel 2 */
          DAC_Cmd(DAC_Channel_2, ENABLE);

          /* Enable DMA for DAC Channel 2 */
          DAC_DMACmd(DAC_Channel_2, ENABLE);
          
          /* Escalator Wave generator ----------------------------------------*/

          /* DAC Channel 1 Configuration */
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
          /* DMA1 Channel 2 configuration */
          DMA_DeInit(DMA1_Channel2);
  
          DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
          DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Escalator8bit;
          DMA_InitStructure.DMA_BufferSize = 6;
          DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
          DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
          DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    
          /* Enable DMA1 Channel 2 */
          DMA_Cmd(DMA1_Channel2, ENABLE);
    
          /* Enable DAC Channel 1: Once the DAC Channel 1 is enabled, PA.05 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Enable DMA for DAC Channel 1 */
          DAC_DMACmd(DAC_Channel_1, ENABLE);
      }
          /* The triangle wave and the noise wave has been selected */
      else
      {
          /* Noise Wave generator --------------------------------------------*/
          /* DAC Channel 1 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
          DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits8_0;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          DAC_Init(DAC_Channel_1, &DAC_InitStructure);

          /* Enable DAC Channel 1: Once the DAC Channel 1 is enabled, PA.04 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_1, ENABLE);

          /* Set DAC Channel 1 DHR12L register */
          DAC_SetChannel1Data(DAC_Align_12b_L, 0x7FF0);
    
          /* Enable DAC Channel 1 wave generator */
          DAC_WaveGenerationCmd(DAC_Channel_1, DAC_Wave_Noise , ENABLE);
          
          /* Triangle Wave generator -----------------------------------------*/
          DAC_DeInit();
          
          /* DAC Channel 2 Configuration */
          DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
          DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
          DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
          DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
          DAC_Init(DAC_Channel_2, &DAC_InitStructure);

          /* Enable DAC Channel 2: Once the DAC Channel 2 is enabled, PA.05 is 
             automatically connected to the DAC converter. */
          DAC_Cmd(DAC_Channel_2, ENABLE);

          /* Set DAC Channel 2 DHR12RD register */
          DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);
      }
      WaveChange = !WaveChange;
    }
  }
}
/**
  * @brief  PrecConfiguration: configure PA4 and PA5 in analog,
  *                           enable DAC clock, enable DMA1 clock
  * @param  None
  * @retval None
  */
void DAC_PreConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* DMA1 clock enable (to be used with DAC) */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA.04 (DAC_OUT1), PA.05 (DAC_OUT2) as analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
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
