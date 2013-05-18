/**
  ******************************************************************************
  * @file    TIM/OCInactive/main.c 
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

/** @addtogroup STM32L1xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM_OCInactive
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
uint16_t CCR1_Val = 1000;
uint16_t CCR2_Val = 500;
uint16_t CCR3_Val = 250;
uint16_t CCR4_Val = 125;
uint16_t PrescalerValue = 0;
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
       
  /* ------------------------- System Clocks Configuration ------------------------------*/
  /* GPIOD clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* ------------------------- GPIO Configuration ------------------------------*/ 
  /* GPIOD Configuration: PD.00, PD.01, PD.04, PD.05 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1| GPIO_Pin_4 | GPIO_Pin_5 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  /* ---------------------------------------------------------------
   TIM3 Configuration: 
   The objective is to get TIM3 counter clock at 1 KHz:
    - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
   And generate 4 signals with 4 different delays:
   TIM3_CH1 delay = CCR1_Val/TIM3 counter clock = 1000 ms
   TIM3_CH2 delay = CCR2_Val/TIM3 counter clock = 500 ms
   TIM3_CH3 delay = CCR3_Val/TIM3 counter clock = 250 ms
   TIM3_CH4 delay = CCR4_Val/TIM3 counter clock = 125 ms
  --------------------------------------------------------------- */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;          
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;                   
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;  
  
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);   
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
  TIM_ARRPreloadConfig(TIM3, DISABLE); 

  /* Output Compare Timing Mode configuration: Channel2 */          
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;    
  TIM_OC2Init(TIM3, &TIM_OCInitStructure); 
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* Output Compare Timing Mode configuration: Channel3 */          
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;  
  
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable); 

  /* Output Compare Timing Mode configuration: Channel4 */          
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;  
  
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable); 

  /* TIM Interrupt configuration */
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);

  /* ------------------------- NVIC Configuration ------------------------------ */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Set PD.00, PD.01, PD.04 and PD.05 pins */
  GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5);

  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
  
  while (1)
  {}
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

  while (1)
  {}
}
#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
