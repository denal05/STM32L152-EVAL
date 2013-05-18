/**
  ******************************************************************************
  * @file    iddmeasurement.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-April-2011
  * @brief   This file includes the idd measurement driver for the STM32L152-EVAL
  *          demonstration.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @defgroup IDDMEASUREMENT
  * @brief    This file includes the idd measurement driver for the STM32L152-EVAL
  *           demonstration.
  * @{
  */

/** @defgroup IDDMEASUREMENT_Private_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup IDDMEASUREMENT_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup IDDMEASUREMENT_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup IDDMEASUREMENT_Private_Variables
  * @{
  */
uint16_t ADCdata = 0;
uint32_t VDD = 0;
uint32_t IDD = 0;
/* Typical zero offset as 0.2uA */
uint32_t Idd_Zero = 200;
extern __IO uint32_t ADCConversionValue;
__IO uint32_t RCC_AHBENR = 0, RCC_APB2ENR = 0, RCC_APB1ENR = 0;
uint8_t Str[20];

uint32_t GPIOA_MODER = 0, GPIOB_MODER = 0, GPIOC_MODER = 0, GPIOD_MODER = 0,
                                        GPIOE_MODER = 0xA8000000, GPIOH_MODER = 0x10;

uint32_t GPIOA_PUPDR = 0x64000000, GPIOB_PUPDR = 0x00000100, GPIOC_PUPDR = 0, GPIOD_PUPDR = 0,
                       GPIOE_PUPDR = 0, GPIOH_PUPDR = 0;

__IO uint8_t LowPowerStatus = 0x00;

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

/** @defgroup IDDMEASUREMENT_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup IDDMEASUREMENT_Private_Functions
  * @{
  */
/**
  * @brief  IDD run mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Run_HCLKTo32(void)
{
  __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0;
  __IO uint8_t pressedkey;
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;

  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);

  LCD_GLASS_DisplayString("  RUN   ");

  while (Menu_ReadKey() != NOKEY)
  {}

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, "Run at System Clock ");
  LCD_DisplayStringLine(LCD_LINE_1, "   32MHz and all    ");
  LCD_DisplayStringLine(LCD_LINE_2, "  peripherals ON    ");
  LCD_DisplayStringLine(LCD_LINE_6, "   To exit press    ");
  LCD_DisplayStringLine(LCD_LINE_7, "      Joystick      ");

  /* Configure IDD measurement pin as analog input ---------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);

  /* ADC1 configuration ------------------------------------------------------*/
  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channelxx configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_48Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {}

   /* Check which key is pressed */
  pressedkey = Menu_ReadKey();

  while (pressedkey == NOKEY)
  {
    /* Check which key is pressed */
    pressedkey = Menu_ReadKey();

    /* Start ADC1 Software Conversion */
    ADC_SoftwareStartConv(ADC1);

    /* Wait until ADC Channel xx end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}

    ADCdata = ADC_GetConversionValue(ADC1);

    /* Calculate voltage value in uV over capacitor  C25 for IDD measurement */
    VDD = (uint32_t)((uint32_t)ADCdata * (uint32_t)ADC_CONVERT_RATIO);

    /* Calculate the current consumption in uA */
    IDD = (uint32_t) ((uint32_t)(((VDD * EVAL_RESISTOR_RATIO) / EVAL_MAX9938_GAIN)) / (uint32_t)(EVAL_RESISTOR_R36));

    /* x  current value*/
    current1000 = (uint8_t)(IDD / 100000);
    /* Hundreds current value */
    current100 = (uint8_t)((IDD % 100000) / 10000);
    /* Tens current value */
    current10 = (uint8_t)((IDD % 10000) / 1000);
    /* 1mA current value */
    current1 = (uint8_t)((IDD % 1000) / 100);
    /* 0.1 mA current value */
    current01 = (uint8_t)((IDD % 100 ) / 10);
    /* 0.01 mA current value */
    current001 =  (uint8_t)(IDD % 10);

    sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%dmA   ", current1000, current100, current10, current1, current01, current001); /** Format the string */
    LCD_DisplayStringLine(LCD_LINE_4, Str);
  }

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  LCD_Clear(LCD_COLOR_WHITE);
  
  LCD_GLASS_DisplayString(" STM32L ");
  
  while (Menu_ReadKey() != NOKEY)
  {}
  
  Demo_SysTickConfig();
  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu();
}

/**
  * @brief  IDD run mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Run_HCLKToMSI_1MHz(void)
{
    __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0;
    uint8_t pressedkey, i;
    uint32_t result;
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
  
    /* Disable the Wakeup Interrupt */
    RTC_ITConfig(RTC_IT_WUT, DISABLE);
  
    /* Disable the JoyStick interrupts */
    Demo_IntExtOnOffConfig(DISABLE);
  
    LCD_GLASS_DisplayString("  RUN   ");
  
    while (Menu_ReadKey() != NOKEY)
    {}
  
    LCD_Clear(LCD_COLOR_WHITE);
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_SetTextColor(LCD_COLOR_WHITE);
  
    /* Configure IDD Measurement pin (ADC Channelxx) as analog input ---------*/
    GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);
  
    LCD_Clear(LCD_COLOR_WHITE);
    LCD_SetBackColor(LCD_COLOR_BLUE);
  
    LCD_DisplayStringLine(LCD_LINE_0, "Run at MSI 1MHz and ");
    LCD_DisplayStringLine(LCD_LINE_1, " all peripherals ON ");
    LCD_DisplayStringLine(LCD_LINE_5, "   To exit press    ");
    LCD_DisplayStringLine(LCD_LINE_6, "      Joystick      ");
  
    
    /* Check which key is pressed */
    pressedkey = Menu_ReadKey();
  
    while (pressedkey == NOKEY)
    {
    /* Check which key is pressed */
    pressedkey = Menu_ReadKey();  
    
    /* Reconfigure the System Clock at MSI 1 MHz */
    SetHCLKToMSI_1MHz();
    
    /* ADC1 configuration ----------------------------------------------------*/
    /* Enable HSI clock for ADC clock */
    RCC_HSICmd(ENABLE);
  
    /* Disable ADC1 */
    ADC_Cmd(ADC1, DISABLE);
  
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
  
    /* ADC1 regular channelxx configuration */
    ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_48Cycles);
  
    ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);
  
    ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
  
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
    {}

    /* Needed delay to have a correct vlaue on C25 */
    Demo_Delay(1); /* 10ms*32 = 320 */
    
    /* Start ADC1 Conversion using Software trigger */
    ADC_SoftwareStartConv(ADC1); 

    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}

    ADC_GetConversionValue(ADC1);
 
    ADCdata = 0x00;
    result = 0x00;

    for (i = 4; i > 0; i--)
    {
      /* Start ADC1 Conversion using Software trigger */
      ADC_SoftwareStartConv(ADC1); 
  
      /* Wait until ADC Channel 15 end of conversion */
      while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
      {}
      /* read ADC convertion result */
      ADCdata = ADC_GetConversionValue(ADC1);
      result += ADCdata;
    }

    /* Reconfigure the System Clock at 32MHz */
    SetHCLKTo32();
  
    /* Deinitialize the ADC1 */
    ADC_DeInit(ADC1);
    
    /* Calculate voltage value in uV over capacitor  C25 for IDD measurement -*/
    VDD = (uint32_t)((uint32_t)ADCdata * (uint32_t)ADC_CONVERT_RATIO);

    /* Calculate the current consumption in uA */
    IDD = (uint32_t) ((uint32_t)(((VDD * EVAL_RESISTOR_RATIO) / EVAL_MAX9938_GAIN)) / (uint32_t)(EVAL_RESISTOR_R36));

    /* x  current value*/
    current1000 = (uint8_t)(IDD / 100000);
    /* Hundreds current value */
    current100 = (uint8_t)((IDD % 100000) / 10000);
    /* Tens current value */
    current10 = (uint8_t)((IDD % 10000) / 1000);
    /* 1mA current value */
    current1 = (uint8_t)((IDD % 1000) / 100);
    /* 0.1 mA current value */
    current01 = (uint8_t)((IDD % 100 ) / 10);
    /* 0.01 mA current value */
    current001 =  (uint8_t)(IDD % 10);

    sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%dmA   ", current1000, current100, current10, current1, current01, current001); /* Format the string */
    LCD_DisplayStringLine(LCD_LINE_3, Str);
  }

  LCD_Clear(LCD_COLOR_WHITE);

  LCD_GLASS_DisplayString(" STM32L ");
  
  while (Menu_ReadKey() != NOKEY)
  {}

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu();
}

/**
  * @brief  IDD Run low power mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_RunLP(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0x00;
  uint8_t i;
  uint32_t result = 0x00;
  
  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);

  /* Disable Leds toggling */
  Demo_LedShow(DISABLE);
  SysTick->CTRL = 0;

  LCD_GLASS_DisplayString(" RUN LP ");
  
  while (Menu_ReadKey() != NOKEY)
  {}

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, " MCU in RUN LP Mode ");
  LCD_DisplayStringLine(LCD_LINE_2, "Run at MSI 32KHz    ");
  LCD_DisplayStringLine(LCD_LINE_3, "from Flash memory.  ");
  LCD_DisplayStringLine(LCD_LINE_4, "All peripherals OFF ");
  LCD_DisplayStringLine(LCD_LINE_5, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_6, "input and RTC OFF   ");
 
  /* Save the RCC configuration */
  RCC_AHBENR   = RCC->AHBENR;
  RCC_APB2ENR  = RCC->APB2ENR;
  RCC_APB1ENR  = RCC->APB1ENR;

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

  /* Forces the RTC peripheral reset*/
  RCC_RTCResetCmd(ENABLE);
  /* Releases the RTC peripheral reset*/
  RCC_RTCResetCmd(DISABLE);
  
  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
     Input mode ...*/
  IDD_Measurement_SaveGPIOConfig();

  /* Configure IDD Measurement pin (ADC Channelxx) as analog input -----------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);

  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

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
  
  /* Disable PVD */
  PWR_PVDCmd(DISABLE);

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  RCC->AHBENR = 0x05;
  RCC->APB2ENR = 0x00;
  RCC->APB1ENR = 0x00;
  
  /* Reconfigure the System Clock at MSI 32 KHz */
  SetHCLKToMSI_64KHz(); 
  RCC_HCLKConfig(RCC_SYSCLK_Div2); 
  
  /* Clear IDD_CNT_EN pin */
  GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
  
  /* Enter low power run mode */
  PWR_EnterLowPowerRunMode(ENABLE);
  
  /* Waiting wake-up interrupt */
  while(LowPowerStatus == 0x00)
  {
  }
  
  /* Exit low power run mode before setting the clock to 32MHz */
  PWR_EnterLowPowerRunMode(DISABLE);
  while(PWR_GetFlagStatus(PWR_FLAG_REGLP) != RESET)
  {
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
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);
  
  /* ADC1 configuration ------------------------------------------------------*/
  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}
  
  /* Enable ADC1 */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channelxx configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_192Cycles);

  ADCdata = 0x00;
  result  = 0x00;
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {}

  for (i = 4; i > 0; i--)
  {
    /* Start ADC1 Conversion using Software trigger*/
    ADC_SoftwareStartConv(ADC1);

    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}
    /* read ADC convertion result */
    ADCdata = ADC_GetConversionValue(ADC1);
    result += ADCdata;
  }
  result = result / 4 ;

  /* At this stage the system has resumed from STOP mode --------------------*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IDD_WAKEUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);
 
  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);
  
  /* Enable PVD */
  PWR_PVDCmd(ENABLE);
 
  /* Restore the GPIO Configurations*/
  IDD_Measurement_RestoreGPIOConfig();

  /*!< LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /*!< Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /*!< LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* LCD GLASS Initialization */
  LCD_GLASS_Init();

  /* GPIO Configuration */
  Demo_GPIOConfig();

  /* Interrupt Configuration */
  Demo_InterruptConfig();

  /* Configure the systick */
  Demo_SysTickConfig();

  /*------------------- Drivers Initialization -------------------------------*/
  /* Initialize the LEDs toogling */
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
    
  /* Calculate voltage value in nV over capacitor  C25 for IDD measurement*/
  VDD = (uint32_t)((uint32_t)result * (uint32_t)ADC_CONVERT_RATIO1000);

  /* Calculate the current consumption in nA */
  IDD = (uint32_t) ((uint32_t)((VDD / EVAL_MAX9938_GAIN)*EVAL_RESISTOR_RATIO) / (uint32_t)(EVAL_RESISTOR_R35 + EVAL_RESISTOR_R36 + EVAL_RESISTOR_STT5PF20V));

  /* x  current value*/
  current1000 = (uint8_t)(IDD / 100000);
  /* Hundreds current value */
  current100 = (uint8_t)((IDD % 100000) / 10000);
  /* Tens current value */
  current10 = (uint8_t)((IDD % 10000) / 1000);
  /* 1uA current value */
  current1 = (uint8_t)((IDD % 1000) / 100);
  /* 0.1 uA current value */
  current01 = (uint8_t)((IDD % 100 ) / 10);
  /* 0.01 uA current value */
  current001 =  (uint8_t)(IDD % 10);

  /* Initialize the LCD */
  STM32L152_LCD_Init();

  /* Enable Leds toggling */
  Demo_LedShow(ENABLE);
  Demo_SysTickConfig();
  
  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_1, "     Run LP Mode     ");
  sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%duA   ", current1000, current100, current10, current1, current01, current001); /** Format the string */
  LCD_DisplayStringLine(LCD_LINE_3, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_6, "continue.           ");
  
  LCD_GLASS_DisplayString(" STM32L ");
  while (Menu_ReadKey() == NOKEY)
  {}

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu(); 
}

/**
  * @brief  IDD sleep mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Sleep(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0;
  uint8_t i;
  uint32_t result;
  
  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);

  /* Disable Leds toggling */
  Demo_LedShow(DISABLE);
  SysTick->CTRL = 0;

  LCD_GLASS_DisplayString(" SLEEP  ");

  while (Menu_ReadKey() != NOKEY)
  {}

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  
  LCD_DisplayStringLine(LCD_LINE_0, " MCU in SLEEP Mode  ");
  LCD_DisplayStringLine(LCD_LINE_2, "Run at System Clock ");
  LCD_DisplayStringLine(LCD_LINE_3, "16MHz, RTC ON, all  ");
  LCD_DisplayStringLine(LCD_LINE_4, "peripherals OFF and ");
  LCD_DisplayStringLine(LCD_LINE_5, "I/Os in Analog input");

  LCD_DisplayStringLine(LCD_LINE_7, "Press Key button to ");
  LCD_DisplayStringLine(LCD_LINE_8, "wake up             ");

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);
  
  /* Save the RCC configuration */
  RCC_AHBENR   = RCC->AHBENR;
  RCC_APB2ENR  = RCC->APB2ENR;
  RCC_APB1ENR  = RCC->APB1ENR;

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

  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
  Input mode ...*/
  IDD_Measurement_SaveGPIOConfig();
  
  /* Configure IDD Measurement pin (ADC Channelxx) as analog input -----------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);

  /* Configure the EXTI Line */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* initialize result */
  result = 0;
  
  /* Configure the System Clock to 16MHz */
  SetHCLKTo16();
  
  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);
  
  /* Enable Ultra low power mode */
  PWR_UltraLowPowerCmd(ENABLE);
 
  /* Diable FLASH during SLeep LP */
  FLASH_SLEEPPowerDownCmd(ENABLE);

  /* Disable All RCC Peripherals*/
  RCC->AHBENR = 0x00;
  RCC->APB2ENR = 0x00;
  RCC->APB1ENR = 0x00;
  
  PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
  
  /* Request to enter SLEEP mode with regulator on */
  PWR_EnterSleepMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
  
  /* Configure the System Clock to 32MHz */
  SetHCLKTo32();

  /* Restor the RCC configuration */
  RCC->AHBENR = RCC_AHBENR;
  RCC->APB2ENR = RCC_APB2ENR;
  RCC->APB1ENR = RCC_APB1ENR;

  /* ADC1 configuration ------------------------------------------------------*/
  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}
  
  /* Enable ADC1 */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channelxx configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_16Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {}

  for (i = 4; i > 0; i--)
  {
    /* Start ADC1 Conversion using Software trigger*/
    ADC_SoftwareStartConv(ADC1);

    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}
    /* read ADC convertion result */
    ADCdata = ADC_GetConversionValue(ADC1);
    result += ADCdata;
  }
  result = result / 4 ;
   
  /* At this stage the system has resumed from STOP mode ---------------------*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Restore the GPIO Configurations*/
  IDD_Measurement_RestoreGPIOConfig();

  /* LCD GLASS Initialization */
  LCD_GLASS_Init();
  
  /* GPIO Configuration */
  Demo_GPIOConfig();

  /* Interrupt Configuration */
  Demo_InterruptConfig();

  /* Configure the systick */
  Demo_SysTickConfig();

  /*------------------- Drivers Initialization -------------------------------*/
  /* Initialize the LEDs toogling */
  Demo_LedShowInit();

  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  
  /* Initialize Wave player application */
  WavePlayer_Init();

  /* Initialize the Thermometer application */
  LM75_Init();

  /* COMP2 Configuration */
  Demo_COMPConfig();

  /* Calculate voltage value in uV over capacitor  C25 for IDD measurement ---*/
  VDD = (uint32_t)((uint32_t)result * (uint32_t)ADC_CONVERT_RATIO);

  /* Calculate the current consumption in uA */
  IDD = (uint32_t) ((uint32_t)(((VDD * EVAL_RESISTOR_RATIO) / EVAL_MAX9938_GAIN)) / (uint32_t)(EVAL_RESISTOR_R36));

  /* x  current value*/
  current1000 = (uint8_t)(IDD / 100000);
  /* Hundreds current value */
  current100 = (uint8_t)((IDD % 100000) / 10000);
  /* Tens current value */
  current10 = (uint8_t)((IDD % 10000) / 1000);
  /* 1mA current value */
  current1 = (uint8_t)((IDD % 1000) / 100);
  /* 0.1 mA current value */
  current01 = (uint8_t)((IDD % 100 ) / 10);
  /* 0.01 mA current value */
  current001 =  (uint8_t)(IDD % 10);

  /* Initialize the LCD */
  STM32L152_LCD_Init();

  /* Enable Leds toggling */
  Demo_LedShow(ENABLE);
  Demo_SysTickConfig();

  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_1, "     SLEEP Mode     ");
  /* Format the string */
  sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%dmA   ", current1000, current100, current10, current1, current01, current001);
  LCD_DisplayStringLine(LCD_LINE_3, Str);

  LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_6, "continue.           ");
  
  LCD_GLASS_DisplayString(" STM32L ");
  
  while (Menu_ReadKey() == NOKEY)
  {}

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  
  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu();
}

/**
  * @brief  IDD sleep low power mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_SleepLP(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0x00;
  uint8_t i;
  uint32_t result = 0x00;
  
  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);

  /* Disable Leds toggling */
  Demo_LedShow(DISABLE);
  SysTick->CTRL = 0;
  LCD_GLASS_DisplayString("SLEEP LP");
  
  while (Menu_ReadKey() != NOKEY)
  {}

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, "MCU will be in SLEEP"); 
  LCD_DisplayStringLine(LCD_LINE_1, "       LP Mode      ");
  LCD_DisplayStringLine(LCD_LINE_3, "Run at MSI 32KHz all");
  LCD_DisplayStringLine(LCD_LINE_4, "peripherals OFF,    ");
  LCD_DisplayStringLine(LCD_LINE_5, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_6, "input and  RTC OFF  ");

  LCD_DisplayStringLine(LCD_LINE_8, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_9, "continue.           ");
  while (Menu_ReadKey() == NOKEY)
  {}
  
  /* Save the RCC configuration */
  RCC_AHBENR   = RCC->AHBENR;
  RCC_APB2ENR  = RCC->APB2ENR;
  RCC_APB1ENR  = RCC->APB1ENR;

  /* Deinitialize all peripherals used in the Demo */
  DAC_DeInit();
  ADC_DeInit(ADC1);
  DMA_DeInit(DMA1_Channel2);
  SPI_DeInit(SPI1);
  TIM_DeInit(TIM2);
  EXTI_DeInit();
  TIM_DeInit(TIM6);
  SYSCFG_DeInit();
  I2C_DeInit(I2C1);
  COMP_DeInit();

  /* Forces the RTC peripheral reset*/
  RCC_RTCResetCmd(ENABLE);
  /* Releases the RTC peripheral reset*/
  RCC_RTCResetCmd(DISABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}
  
  /* Enable ADC1 */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channelxx configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_16Cycles);

  ADCdata = 0x00;
   
  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
     Input mode ...*/
  IDD_Measurement_SaveGPIOConfig();

  /* Reset the RCC Clock Peripherals */
  RCC->AHBENR  = 0x05;
  RCC->APB2ENR = 0x00;
  RCC->APB1ENR = 0x00;
  
  /* Configure IDD Measurement pin (ADC Channelxx) as analog input -----------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);

  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

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

  /* Disable PVD */
  PWR_PVDCmd(DISABLE);

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Enable Ultra low power mode */
  PWR_UltraLowPowerCmd(ENABLE);
 
  /* Diable FLASH during SLeep LP */
  FLASH_SLEEPPowerDownCmd(ENABLE);
  
  /* Disable HSI clock before entering Sleep LP mode */ 
  RCC_HSICmd(DISABLE);
  /* Disable HSE clock */
  RCC_HSEConfig(RCC_HSE_OFF);
  /* Disable LSE clock */
  RCC_LSEConfig(RCC_LSE_OFF);
  /* Disable LSI clock */
  RCC_LSICmd(DISABLE);

  /* Clear IDD_CNT_EN pin */
  GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
   
  RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_CLOCK, ENABLE);
  RCC_APB1PeriphClockLPModeCmd(RCC_APB1Periph_CLOCK, ENABLE);
  RCC_AHBPeriphClockLPModeCmd(RCC_AHBPeriph_CLOCK, ENABLE);
    
  /* Reconfigure the System Clock at MSI 64 KHz */
  SetHCLKToMSI_64KHz();
  
  RCC_HCLKConfig(RCC_SYSCLK_Div2); 
 
  /* Request to enter SLEEP mode with regulator low power */
  PWR_EnterSleepMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

  RCC->AHBENR = RCC_AHBENR;
  RCC->APB2ENR = RCC_APB2ENR;
  RCC->APB1ENR = RCC_APB1ENR;

  /* Configure the System Clock to 32MHz */
  SetHCLKTo32();

  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);

  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {}

  for (i = 4; i > 0; i--)
  {
    /* Start ADC1 Conversion using Software trigger*/
    ADC_SoftwareStartConv(ADC1);

    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}
    /* read ADC convertion result */
    ADCdata = ADC_GetConversionValue(ADC1);
    result += ADCdata;
  }
  result = result / 4 ;
  
  /* Reset the counter by programming IDD_CNT_EN High in less than 70ms after
  the wakeup to avoid 1Kohm to be connected later on VDD_MCU */
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);

  /* At this stage the system has resumed from SLEEP LP mode -----------------*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IDD_WAKEUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);
  
  /* Disable PVD */
  PWR_PVDCmd(ENABLE);
 
  /* Restore the GPIO Configurations*/
  IDD_Measurement_RestoreGPIOConfig();

  /*!< LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /*!< Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /*!< LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* LCD GLASS Initialization */
  LCD_GLASS_Init();

  /* GPIO Configuration */
  Demo_GPIOConfig();

  /* Interrupt Configuration */
  Demo_InterruptConfig();

  /* Configure the systick */
  Demo_SysTickConfig();

  /*------------------- Drivers Initialization -------------------------------*/
  /* Initialize the LEDs toogling */
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

  /* Calculate voltage value in nV over capacitor  C25 for IDD measurement*/
  VDD = (uint32_t)((uint32_t)result * (uint32_t)ADC_CONVERT_RATIO1000);

  /* Calculate the current consumption in nA */
  IDD = (uint32_t) ((uint32_t)((VDD / EVAL_MAX9938_GAIN)*EVAL_RESISTOR_RATIO) / (uint32_t)(EVAL_RESISTOR_R35 + EVAL_RESISTOR_R36 + EVAL_RESISTOR_STT5PF20V));

  /* x  current value*/
  current1000 = (uint8_t)(IDD / 100000);
  /* Hundreds current value */
  current100 = (uint8_t)((IDD % 100000) / 10000);
  /* Tens current value */
  current10 = (uint8_t)((IDD % 10000) / 1000);
  /* 1uA current value */
  current1 = (uint8_t)((IDD % 1000) / 100);
  /* 0.1 uA current value */
  current01 = (uint8_t)((IDD % 100 ) / 10);
  /* 0.01 uA current value */
  current001 =  (uint8_t)(IDD % 10);

  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_1, "    Sleep LP Mode    ");
  sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%duA   ", current1000, current100, current10, current1, current01, current001); /** Format the string */
  LCD_DisplayStringLine(LCD_LINE_3, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_6, "continue.           ");

  LCD_GLASS_DisplayString(" STM32L ");
  while (Menu_ReadKey() == NOKEY)
  {}

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu();
}

/**
  * @brief  IDD stop mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Stop(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0x00;
  uint8_t i;
  __IO uint32_t result = 0x00;

  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, "MCU will be in STOP "); 
  LCD_DisplayStringLine(LCD_LINE_1, "        Mode        ");
  LCD_DisplayStringLine(LCD_LINE_3, "All peripherals OFF ");
  LCD_DisplayStringLine(LCD_LINE_4, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_5, "input and  RTC OFF. ");

  LCD_DisplayStringLine(LCD_LINE_7, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_8, "continue.           ");
  
  LCD_GLASS_DisplayString("  STOP  ");
  
  while (Menu_ReadKey() == NOKEY)
  {}
  /* Save the RCC configuration*/
  RCC_AHBENR   = RCC->AHBENR;
  RCC_APB2ENR  = RCC->APB2ENR;
  RCC_APB1ENR  = RCC->APB1ENR;

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

  /* Forces the RTC peripheral reset*/
  RCC_RTCResetCmd(ENABLE);
  /* Releases the RTC peripheral reset*/
  RCC_RTCResetCmd(DISABLE);
  
  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
     Input mode ...*/
  IDD_Measurement_SaveGPIOConfig();

  /* Configure IDD Measurement pin (ADC Channelxx) as analog input -----------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);

  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

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

  /* Disable PVD */
  PWR_PVDCmd(DISABLE);

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Enable Ultra low power mode */
  PWR_UltraLowPowerCmd(ENABLE);
  
  /* Clear IDD_CNT_EN pin */
  GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
  
  /* Request to enter STOP mode with regulator in low power */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
  
  /* Reset the counter by programming IDD_CNT_EN High in less than 70ms after
  the wakeup to avoid 1Kohm to be connected later on VDD_MCU */
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
  
  /* Configure the System Clock to 32MHz */
  SetHCLKTo32();
 
  /* ADC1 configuration ------------------------------------------------------*/
  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);

  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}
  
  /* SYSCFG and ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG | RCC_APB2Periph_ADC1, ENABLE);
  
  /* disable ADC1*/
  ADC_Cmd(ADC1, DISABLE);
  
  /* Enable ADC1 */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channelxx configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_48Cycles);

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
  
  ADCdata = 0x00;

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {}
  
  /* Start ADC1 Conversion using Software trigger*/
  ADC_SoftwareStartConv(ADC1);
    
  /* Wait until ADC Channel 15 end of conversion */
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
  {}
  /* read ADC convertion result */
  ADCdata = ADC_GetConversionValue(ADC1);
  
  for (i = 4; i > 0; i--)
  {
    /* Start ADC1 Conversion using Software trigger*/
    ADC_SoftwareStartConv(ADC1);
    
    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}
    /* read ADC convertion result */
    ADCdata = ADC_GetConversionValue(ADC1);
    result += ADCdata;
  }
  result = result / 4 ;
  
  RCC->AHBENR = RCC_AHBENR;
  RCC->APB2ENR = RCC_APB2ENR;
  RCC->APB1ENR = RCC_APB1ENR;
  
  /* Disable the Bandgap */
  PWR_UltraLowPowerCmd(DISABLE);
  
  /* At this stage the system has resumed from STOP mode ---------------------*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line = IDD_WAKEUP_EXTI_LINE;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IDD_WAKEUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

  /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
     as system clock source (HSE and PLL are disabled in STOP mode) */
  IDD_Measurement_SYSCLKConfig_STOP();

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Restore the GPIO Configurations*/
  IDD_Measurement_RestoreGPIOConfig();

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);
    
  /*!< LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /*!< Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /*!< LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* GPIO Configuration */
  Demo_GPIOConfig();

  /* Interrupt Configuration */
  Demo_InterruptConfig();

  /* Configure the systick */
  Demo_SysTickConfig();

  /*------------------- Drivers Initialization -------------------------------*/
  /* Initialize the LEDs toogling */
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

  /* Calculate voltage value in nV over capacitor  C25 for IDD measurement */
  VDD = (uint32_t)((uint32_t)result * (uint32_t)ADC_CONVERT_RATIO1000);

  /* Calculate the current consumption in nA */
  IDD = (uint32_t) ((uint32_t)((VDD / EVAL_MAX9938_GAIN)*EVAL_RESISTOR_RATIO) / (uint32_t)(EVAL_RESISTOR_R35 + EVAL_RESISTOR_R36 + EVAL_RESISTOR_STT5PF20V));
 
  if (IDD > 600)
  {
    /* Remove the zero offset current  */
    IDD -=Idd_Zero;
  } 
  /* x  current value*/
  current1000 = (uint8_t)(IDD / 100000);
  /* Hundreds current value */
  current100 = (uint8_t)((IDD % 100000) / 10000);
  /* Tens current value */
  current10 = (uint8_t)((IDD % 10000) / 1000);
  /* 1uA current value */
  current1 = (uint8_t)((IDD % 1000) / 100);
  /* 0.1 uA current value */
  current01 = (uint8_t)((IDD % 100 ) / 10);
  /* 0.01 uA current value */
  current001 =  (uint8_t)(IDD % 10);

  /* Initialize the LCD */
  STM32L152_LCD_Init();

  /* LCD GLASS Initialization */
  LCD_GLASS_Init();
  
  /* COMP2 Configuration */
  Demo_COMPConfig(); 

  /* Enable Leds toggling */
  Demo_LedShow(ENABLE);
  Demo_SysTickConfig();

  /* Clear the LCD */
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_1, "     Stop Mode      ");
  sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%duA   ", current1000, current100, current10, current1, current01, current001); /** Format the string */
  LCD_DisplayStringLine(LCD_LINE_3, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_6, "continue.           ");

  LCD_GLASS_DisplayString(" STM32L ");
  while (Menu_ReadKey() == NOKEY)
  {}
  
  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  /* Enable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(ENABLE);
  
  /* Display the previous menu */
  Menu_DisplayMenu();
}

/**
  * @brief  Enter the MCU in standby mode.
  * @param  None
  * @retval None
  */
void IDD_Enter_Standby(void)
{
  __IO uint8_t pressedkey = 0x00;
  
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);
  
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, "Time and Date must  "); 
  LCD_DisplayStringLine(LCD_LINE_1, "be configured before");
  LCD_DisplayStringLine(LCD_LINE_2, "entering in standby ");
  LCD_DisplayStringLine(LCD_LINE_3, "mode.               ");
  LCD_DisplayStringLine(LCD_LINE_5, "- To configure RTC  ");
  LCD_DisplayStringLine(LCD_LINE_6, "press joystick SEL. ");
  LCD_DisplayStringLine(LCD_LINE_7, "- If RTC already    ");
  LCD_DisplayStringLine(LCD_LINE_8, "configured press any");
  LCD_DisplayStringLine(LCD_LINE_9, "other joystick.     ");
  
  LCD_GLASS_DisplayString("STANDBY ");
  
  pressedkey = Menu_ReadKey();
  while (pressedkey == NOKEY)
  {
    pressedkey = Menu_ReadKey();
  }
   
  if (pressedkey == SEL)
  {
   /* Clear the LCD */
   LCD_Clear(LCD_COLOR_WHITE);
  
   /* Initialize the Calendar */
   Calendar_Init();
  } 
  
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, "   MCU will be in   "); 
  LCD_DisplayStringLine(LCD_LINE_1, "    STANDBY Mode    ");
  LCD_DisplayStringLine(LCD_LINE_2, "All peripherals OFF ");
  LCD_DisplayStringLine(LCD_LINE_3, "all I/O in Analog   ");
  LCD_DisplayStringLine(LCD_LINE_4, "input and RTC ON.  ");

  LCD_DisplayStringLine(LCD_LINE_6, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_7, "continue.           ");
  
  while (Menu_ReadKey() == NOKEY)
  {}

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
   
  /* Save the GPIO pins current configuration then put all GPIO pins in Analog
     Input mode ...*/
  IDD_Measurement_SaveGPIOConfig();

  /* Configure IDD_CNT_EN pin as output push-pull ----------------------------*/
  GPIO_SetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);

  GPIO_InitStructure.GPIO_Pin = IDD_CNT_EN_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_CNT_EN_GPIO_PORT, &GPIO_InitStructure);

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

  /* Disable PVD */
  PWR_PVDCmd(DISABLE);

  /* Clear Wake Up flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Enable Ultra low power mode */
  PWR_UltraLowPowerCmd(ENABLE);
  
  /* Clear IDD_CNT_EN pin */
  GPIO_ResetBits(IDD_CNT_EN_GPIO_PORT, IDD_CNT_EN_PIN);
  
  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
  PWR_EnterSTANDBYMode();
  
 /* This stage will not be reached */
  while(1);
}

/**
  * @brief  IDD standby mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Measurement_Standby(void)
{
  uint8_t i = 0;
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;

  /* Configure IDD Measurement pin (ADC Channelxx) as analog input -----------*/
  GPIO_InitStructure.GPIO_Pin = IDD_MEASUREMENT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IDD_MEASUREMENT_GPIO_PORT, &GPIO_InitStructure);  
  
  /* ADC1 configuration ------------------------------------------------------*/

  /* Enable HSI clock for ADC clock */
  RCC_HSICmd(ENABLE);

  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}

  /* Enable ADC1 */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channelxx configuration */
  ADC_RegularChannelConfig(ADC1, IDD_MEASUREMENT_ADC_CHANNEL, 1, ADC_SampleTime_16Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {}

  ADCdata = 0x00;

  for (i = 4; i > 0; i--)
  {
    /* Start ADC1 Conversion using Software trigger*/
    ADC_SoftwareStartConv(ADC1);

    /* Wait until ADC Channel 15 end of conversion */
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
    {}
    /* read ADC convertion result */
    ADCdata = ADC_GetConversionValue(ADC1);
    ADCConversionValue += ADCdata;
  }
  ADCConversionValue = ADCConversionValue / 4 ;
}

/**
  * @brief  IDD standby mode measurement program.
  * @param  None
  * @retval None
  */
void IDD_Display_Standby(void)
{
  __IO uint8_t current1000, current100, current10, current1, current01, current001 = 0x00;
  
  while (Menu_ReadKey() != NOKEY)
  {}
  
  /* Calculate voltage value in nV over capacitor  C25 for IDD measurement */
  VDD = (uint32_t)((uint32_t)ADCConversionValue * (uint32_t)ADC_CONVERT_RATIO1000);

  /* Calculate the current consumption in nA */
  IDD = (uint32_t) ((uint32_t)((VDD / EVAL_MAX9938_GAIN)*EVAL_RESISTOR_RATIO) / (uint32_t)(EVAL_RESISTOR_R35 + EVAL_RESISTOR_R36 + EVAL_RESISTOR_STT5PF20V));
  /* Remove the zero offset current  */
  IDD -=Idd_Zero;

  /* x  current value*/
  current1000 = (uint8_t)(IDD / 100000);
  /* Hundreds current value */
  current100 = (uint8_t)((IDD % 100000) / 10000);
  /* Tens current value */
  current10 = (uint8_t)((IDD % 10000) / 1000);
  /* 1uA current value */
  current1 = (uint8_t)((IDD % 1000) / 100);
  /* 0.1 uA current value */
  current01 = (uint8_t)((IDD % 100 ) / 10);
  /* 0.01 uA current value */
  current001 =  (uint8_t)(IDD % 10);

  /* Clear the LCD */
  LCD_Clear(White);
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(LCD_LINE_1, "    Standby Mode    ");
  sprintf ((char*)Str, "   IDD: %d%d%d,%d%d%duA   ", current1000, current100, current10, current1, current01, current001); /** Format the string */
  LCD_DisplayStringLine(LCD_LINE_3, Str);
  LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick to   ");
  LCD_DisplayStringLine(LCD_LINE_6, "continue.           ");
  
  while (Menu_ReadKey() == NOKEY)
  {}

  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);
}

/**
  * @brief  Save all GPIOs Configurations.
  * @param  None
  * @retval None
  */
void IDD_Measurement_SaveGPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

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

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
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
}
/**
  * @brief  Restores all GPIOs Configurations.
  * @param  None
  * @retval None
  */
void IDD_Measurement_RestoreGPIOConfig(void)
{
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
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

